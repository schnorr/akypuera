/*
    This file is part of librastro

    librastro is free software: you can redistribute it and/or modify
    it under the terms of the GNU Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    librastro is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Public License for more details.

    You should have received a copy of the GNU Public License
    along with librastro. If not, see <http://www.gnu.org/licenses/>.
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
#include <argp.h>

/* configuration structure */
#define RASTRO_INPUT_SIZE 300
struct arguments {
  char *slaves[RASTRO_INPUT_SIZE];
  int number_of_slaves;
  int slave_mode;
  char *master_port;
  int sample_size;
  char *master_host;
  char *remote_login;
  char *program_name;
};

/* return current time */
static long long getCurrentTime(void)
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (long long) tv.tv_sec * 1000000 +  (long long) tv.tv_usec;
}

static long long timerabs(struct timeval a)
{
  return (long long)a.tv_sec * 1000000 + a.tv_usec;
}

static long timerdiff(struct timeval a, struct timeval b)
{
  return (a.tv_sec - b.tv_sec) * 1000000 + (a.tv_usec - b.tv_usec);
}

static void receive_data(int socket, char *buffer, int size)
{
  int received = 0;
  while (received != size){
    received = recv(socket, (void *) buffer, size, 0);
    if (received < size){
      fprintf(stdout, "received less...\n");
      buffer += received;
      size -= received;
      received = 0;
    }
  }
}

static void ping1(int socket, long long *local, long long *remote, long *delta)
{
  struct timeval t0, t1, tremote;
  int test;
  gettimeofday(&t0, NULL);
  test = send(socket, (void *) &t0, sizeof(t0), 0);
  if (test == -1) {
    fprintf(stderr, "[ping]: send failed at socket %d!\n", socket);
    exit(1);
  }
  receive_data(socket, (char *) &tremote, sizeof(tremote));
  gettimeofday(&t1, NULL);
  *delta = timerdiff(t1, t0);
  *local = timerabs(t0) + *delta / 2;
  *remote = timerabs(tremote);
}

static void ping(int socket, long long *mlocal, long long *mremote,
          char *remotename, int sample_size)
{
  long delta;
  long long local, remote;
  long mdelta = 1e9;
  int i;
  struct timeval t0 = {0, 0};
  int remotenamelen;
    
  receive_data(socket, (char *) &remotenamelen, sizeof(remotenamelen));
  receive_data(socket, remotename, remotenamelen);
  remotename[remotenamelen] = '\0';

  for (i = 0; i < sample_size; i++) {
    ping1(socket, &local, &remote, &delta);
    if (delta <= mdelta) {
      mdelta = delta;
      *mlocal = local;
      *mremote = remote;
    }
  }
  send(socket, (void *) &t0, sizeof(t0), 0);
}


static void pong(int socket)
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
    receive_data(socket, (char *) &tvi, sizeof(tvi));
    gettimeofday(&tvo, NULL); 
    teste = send(socket, (void *) &tvo, sizeof(tvo), 0);
    if (teste == -1) {
      fprintf(stderr, "[pong]: send failed at socket %d!\n", socket);
      exit(1);
    }
  } while (tvi.tv_sec != 0);
}

static int open_connection(int *pport)
{
  struct sockaddr_in connection;
  int sock, result;
  int port = 1024;
	
  sock = socket(AF_INET, SOCK_STREAM, 0);
  do {
    port++;
    connection.sin_family = AF_INET;
    connection.sin_port = htons(port);
    connection.sin_addr.s_addr = htons(INADDR_ANY);
    bzero(connection.sin_zero, 8);
    result = bind(sock, (struct sockaddr *) &connection,
                  sizeof(connection)); 
  } while (result != 0);
  listen(sock, 2);
  *pport = port;
  return sock;
}

static int wait_connection(int sock)
{
  struct sockaddr_in connection;
  int new_socket;
  socklen_t size;
  size = sizeof(connection);
  new_socket = accept(sock, (struct sockaddr *) &connection, &size);
  return new_socket;
}

static int establish_connection(char *host, char *port)
{
  struct hostent *h;
  struct sockaddr_in connection;
  int new_socket;
  h = gethostbyname(host);
  new_socket = socket(AF_INET, SOCK_STREAM, 0);
  connection.sin_family = AF_INET;
  connection.sin_port = htons(atoi(port));
  memcpy(&connection.sin_addr.s_addr, h->h_addr, 4);
  bzero(connection.sin_zero, 8);
  while ( (connect(new_socket, (struct sockaddr *) 
                   &connection,sizeof(connection)) ) != 0); 
  return new_socket;
}


static void create_slave(struct arguments *arg, char *remote_host, int master_port)
{
  char str[10];
  snprintf (str, 10, "%d", master_port);
  char par1[] = "-s";
  char par2[] = "-m";
  char par3[] = "-p";
  char *const command_arg[] = {
    arg->remote_login, remote_host, //connect to the remote host
    arg->program_name,
    par1, //program to execute and parameters
    par2, arg->master_host,
    par3, str,
    NULL
  };

  if (fork()==0){
    execvp(command_arg[0], command_arg);
    fprintf (stderr,
             "[rastro_timesync] at %s, "
             "slave not created, error on exec.\n",
             __FUNCTION__);
    fprintf (stderr,
             "[rastro_timesync] tried to launch slave on\n"
             "[rastro_timesync] (%s)\n"
             "[rastro_timesync] using the following parameters:\n", remote_host);
    int i;
    const char *a = NULL;
    for (i = 0, a = command_arg[0]; a; i++, a = command_arg[i]){
      fprintf (stderr,
               "[rastro_timesync] \t%s\n", a);
    }
    fprintf (stderr,
             "[rastro_timesync] check if %s is capable of executing\n"
             "[rastro_timesync] something on (%s) with this command.\n"
             "[rastro_timesync] $ %s %s ls\n",
             arg->remote_login, remote_host, arg->remote_login, remote_host);
    fprintf (stderr, "\n[rastro_timesync] Type CRTL + C to abort now.\n");
  }
}

static void synchronize_slave (struct arguments *arg, char *remote_host)
{
  long long local_time;
  long long remote_time;
  int port;
  int com_socket;
  int new_socket;

  gethostname(arg->master_host, sizeof(arg->master_host));
  com_socket = open_connection (&port);
  create_slave (arg, remote_host, port);
  new_socket = wait_connection(com_socket);
  ping(new_socket,
       &local_time,
       &remote_time,
       remote_host,
       arg->sample_size);
  printf("%s %lld %s %lld\n",
         arg->master_host, local_time,
         remote_host, remote_time);
  close(new_socket);
  close(com_socket);
}

static void slave (char *master_host, char *master_port)
{
  int com_socket = establish_connection (master_host, master_port);
  pong(com_socket);
  close(com_socket);
}

/* parameters */
static char doc[] = "Calculate the clock difference with other hosts.\n\n"
  "To avoid PATH problems, you might prefer running this program like this:\n"
  "`which rastro_timesync` {hostname_1 hostname_2 ...}\n";
static char args_doc[] = "{hostname_1 hostname_2 ...}";

static struct argp_option options[] = {
  {"slave", 's', 0, OPTION_HIDDEN, NULL},
  {"master_host", 'm', "MASTER", OPTION_HIDDEN, NULL},
  {"master_port", 'p', "PORT", OPTION_HIDDEN, NULL}, 
  {"sample", 'z', "SIZE", 0, "Sampling size (Default is 1000)"},
  {"remote", 'r', "RSH", 0, "Remote login program"},
  { 0 }
};

static int parse_options (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;
  switch (key){
  case 's': arguments->slave_mode = 1; break;
  case 'm': arguments->master_host = arg; break;
  case 'p': arguments->master_port = arg; break;
  case 'r': arguments->remote_login = strdup(arg); break;
  case 'z': arguments->sample_size = atoi(arg); break;
  case ARGP_KEY_ARG:
    if (arguments->number_of_slaves == RASTRO_INPUT_SIZE) {
      /* Too many arguments. */
      argp_usage (state);
    }
    arguments->slaves[state->arg_num] = arg;
    arguments->number_of_slaves++;
    break;
  case ARGP_KEY_END:
    if (state->arg_num < 1 && arguments->slave_mode == 0){
      /* Not enough arguments. */
      argp_usage (state);
    }
    break;
  default: return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = { options, parse_options, args_doc, doc };

/* main */
int main(int argc, char *argv[])
{
  struct arguments arguments;
  bzero (&arguments, sizeof(struct arguments));
  if (argp_parse (&argp, argc, argv, 0, 0, &arguments) == ARGP_KEY_ERROR){
    fprintf(stderr,
            "[rastro_timesync] at %s,"
            "error during the parsing of parameters\n",
            __FUNCTION__);
    return 1;
  }else{
    arguments.program_name = argv[0];
  }

  if (arguments.slave_mode){
    slave (arguments.master_host, arguments.master_port);
  }else{
    //since this reference host, find my hostname
    char hostname[MAXHOSTNAMELEN];
    gethostname(hostname, sizeof(hostname));
    arguments.master_host = strndup (hostname, sizeof(hostname));

    //define the default sampling size
    if (arguments.sample_size == 0)
      arguments.sample_size = 1000;

    //define default for remote login
    if (arguments.remote_login == NULL)
      arguments.remote_login = strdup ("ssh");

    int i;
    for (i = 0; i < arguments.number_of_slaves; i++) {
      synchronize_slave (&arguments, arguments.slaves[i]);
    }

    free (arguments.master_host);
    free (arguments.remote_login);
  }
   return 0;
}

