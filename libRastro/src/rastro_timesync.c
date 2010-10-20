/*
    Copyright (c) 1998--2006 Benhur Stein

    This file is part of Pajé.

    Pajé is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    Pajé is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
    for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Pajé; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02111 USA.
*/



#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/param.h>  /* for MAXHOSTNAMELEN */

#define MAX_HOSTS 256
#define PORT 3000
#define SZ_SAMPLE 1000

char hosttable[MAX_HOSTS][MAXHOSTNAMELEN];
int num_hosts;
FILE *hostfile, *rastro;
char *myname;

/* return current time */
long long getCurrentTime(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (long long) tv.tv_sec * 1000000 +  (long long) tv.tv_usec;
}

long long timerabs(struct timeval a)
{
    return (long long)a.tv_sec * 1000000 + a.tv_usec;
}

long timerdiff(struct timeval a, struct timeval b)
{
    return (a.tv_sec - b.tv_sec) * 1000000 + (a.tv_usec - b.tv_usec);
}

void recebe_mesmo(int socket, char *buffer, int tamanho)
{
	int recebidos = 0;
	while (recebidos != tamanho){
		recebidos = recv(socket, (void *) buffer, tamanho, 0);
		if (recebidos < tamanho){
			fprintf(stdout, "received less...\n");
			buffer += recebidos;
			tamanho -= recebidos;
			recebidos = 0;
		}
	}
}

void ping1(int socket, long long *local, long long *remote, long *delta)
{
    struct timeval t0, t1, tremote;
    int teste;
    gettimeofday(&t0, NULL);
    teste = send(socket, (void *) &t0, sizeof(t0), 0);
    if (teste == -1) {
        fprintf(stderr, "[ping]: send failed at socket %d!\n", socket);
        exit(1);
    }
    recebe_mesmo(socket, (char *) &tremote, sizeof(tremote));
    gettimeofday(&t1, NULL);
    *delta = timerdiff(t1, t0);
    *local = timerabs(t0) + *delta / 2;
    *remote = timerabs(tremote);
}

void ping(int socket, long long *mlocal, long long *mremote, char *remotename)
{
    long delta;
    long long local, remote;
    long mdelta = 1e9;
    int i;
    struct timeval t0 = {0, 0};
    int remotenamelen;
    
    recebe_mesmo(socket, (char *) &remotenamelen, sizeof(remotenamelen));
    recebe_mesmo(socket, remotename, remotenamelen);
    remotename[remotenamelen] = '\0';

    for (i = 0; i < SZ_SAMPLE; i++) {
        ping1(socket, &local, &remote, &delta);
        if (delta <= mdelta) {
            mdelta = delta;
            *mlocal = local;
            *mremote = remote;
        }
    }
    send(socket, (void *) &t0, sizeof(t0), 0);
}


void pong(int socket)
{
	struct timeval tvi, tvo;
	int teste;
        int namesize;
        char hostname[MAXHOSTNAMELEN];

        gethostname(hostname, sizeof(hostname));
        namesize = strlen(hostname);
        send(socket, (char *)&namesize, sizeof(namesize), 0);
        send(socket, hostname, namesize, 0);
	do {
		recebe_mesmo(socket, (char *) &tvi, sizeof(tvi));
		gettimeofday(&tvo, NULL); 
		teste = send(socket, (void *) &tvo, sizeof(tvo), 0);
		if (teste == -1) {
			fprintf(stderr, "[pong]: send failed at socket %d!\n", socket);
			exit(1);
		}
	} while (tvi.tv_sec != 0);
}

int abre_conexao(int *pporta)
{
	struct sockaddr_in connection;
	int sock, resultado;
	int porta = 1024;
	
	sock = socket(AF_INET, SOCK_STREAM, 0);
	do {
		porta++;
		connection.sin_family = AF_INET;
		connection.sin_port = htons(porta);
		connection.sin_addr.s_addr = htons(INADDR_ANY);
		bzero(connection.sin_zero, 8);
		resultado = bind(sock, (struct sockaddr *) &connection,
		            sizeof(connection)); 
	} while (resultado != 0);
	listen(sock, 2);
	*pporta = porta;
	return sock;
}

int wait_connection(int sock)
{
	struct sockaddr_in connection;
	int new_socket;
	socklen_t size;
	size = sizeof(connection);
	new_socket = accept(sock, (struct sockaddr *) &connection, &size);
	return new_socket;
}

int establish_connection(char *host, int porta)
{
	struct hostent *h;
	struct sockaddr_in connection;
	int new_socket;
	h = gethostbyname(host); // deveria ser o nome do mestre, recebido como parametro
	new_socket = socket(AF_INET, SOCK_STREAM, 0);
	connection.sin_family = AF_INET;
	connection.sin_port = htons(porta);
	memcpy(&connection.sin_addr.s_addr, h->h_addr, 4);
	bzero(connection.sin_zero, 8);
	while ( (connect(new_socket, (struct sockaddr *) 
			&connection,sizeof(connection)) ) != 0); 
	return new_socket;
}


void create_slave(char *remotehost, char *localhost, int port) 
{
	char sport[10];
	char *arg[] = {
		"rsh",
		remotehost,
		myname,
		"slave",
                localhost,
		sport,
		NULL
	};

	sprintf(sport, "%d", port);

	if ( fork() == 0 ) {
		//fprintf(stdout, "Launching process at %s... ", remotehost);
		//fprintf(stdout, "Passed!\n");
		if ( execvp(arg[0], arg) == -1 )  { 
			fprintf(stderr, "ERROR: %s\n", strerror(errno));
			exit(-1);
		}
	}
}

// Leitura de arquivo de hosts
void read_hostfile(char *fn)
{
	char buffer[MAXHOSTNAMELEN];
	
	num_hosts = 0;
	hostfile = fopen(fn, "r");
	while ( !feof(hostfile) ) {
		bzero(buffer, MAXHOSTNAMELEN);
		if ( fgets(buffer, MAXHOSTNAMELEN, hostfile) != NULL ) {
			buffer[strlen(buffer) - 1] = '\0';
			strcpy(hosttable[num_hosts++], buffer);
		}
	}
	fclose(hostfile);
}

void sincroniza(char *localhost, char *remotehost)
{
    long long local;
    long long remote;
    int porta;
    int com_socket;
    int new_socket;
    char localname[MAXHOSTNAMELEN];
    char remotename[MAXHOSTNAMELEN];

    gethostname(localname, sizeof(localname));
    com_socket = abre_conexao(&porta);
    create_slave(remotehost, localhost, porta);
    new_socket = wait_connection(com_socket);
    ping(new_socket, &local, &remote, remotename);
    printf("%s %lld %s %lld\n", localname, local, remotename, remote);
    close(new_socket);
    close(com_socket);
}

void slave(char *mastername, char *sport)
{
    int com_socket;
    int port;

    port = atoi(sport);
    com_socket = establish_connection(mastername, port);
    pong(com_socket);
    close(com_socket);
}


int main(int argc, char *argv[])
{
    myname = argv[0];

    if (argc < 3) {
        fprintf(stdout, "%s localhostname remotehostname ...\n", argv[0]);
        return -1;
    }
    if (strcmp(argv[1], "slave") == 0 ) {
        slave(argv[2], argv[3]);
    } else {
        int i;
        for (i = 2; i < argc; i++) {
            sincroniza(argv[1], argv[i]);
        }
    }
    exit(0);
}

