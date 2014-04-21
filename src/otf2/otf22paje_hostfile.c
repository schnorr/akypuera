/*
    This file is part of Akypuera

    Akypuera is free software: you can redistribute it and/or modify
    it under the terms of the GNU Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Akypuera is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Public License for more details.

    You should have received a copy of the GNU Public License
    along with Akypuera. If not, see <http://www.gnu.org/licenses/>.
*/
#include "otf22paje.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char **hostnames = NULL;
static int hostnames_size = 0;

static void nf_hostname_add (char *host)
{
  if (host){
    hostnames_size++;
    hostnames = realloc (hostnames, hostnames_size*sizeof(char*));
    hostnames[hostnames_size-1] = strdup(host);
  }
}

int nf_hostnames_search (char *host)
{
  if (host){
    int j;
    for (j = 0; j < hostnames_size; j++){
      if (strcmp (hostnames[j], host)){
	return 1;
      }
    }
  }
  return 0;
}

void nf_hostnames_clear (void)
{
  int j;
  for (j = 0; j < hostnames_size; j++){
    free (hostnames[j]);
  }
  free (hostnames);
}

static void nf_container_type_declare (const char *new_container_type, const char *container_type)
{
  static char **types = NULL;
  static int n = 0, i;

  //check if already declared
  for (i = 0; i < n; i++){
    if (strcmp (types[i], new_container_type) == 0) return;
  }

  //add
  n++;
  types = realloc (types, n*sizeof(char*));
  types[n-1] = strdup (new_container_type);
  poti_DefineContainerType (new_container_type, container_type, new_container_type);
}

static void nf_container_create (const char *new_container, const char *new_container_type, const char *container)
{
  static char **containers = NULL;
  static int n = 0, i;

  //check if already declared
  for (i = 0; i < n; i++){
    if (strcmp (containers[i], new_container) == 0) return;
  }

  //add
  n++;
  containers = realloc (containers, n*sizeof(char*));
  containers[n-1] = strdup (new_container);
  poti_CreateContainer (0, new_container, new_container_type, container, new_container);
}

int nf_read_and_create_hierarchy (char *filename)
{
  FILE *arq = fopen (filename, "r");
  if (!arq) return 1;

  while (!feof(arq)){
    char *line = malloc(sizeof(char));
    size_t size = 1;

    size_t bytes_read = getline (&line, &size, arq);

    if (feof(arq)){
      free (line);
      break;
    }

    line[bytes_read-1] = '\0';

    //tokenize
    char *token = NULL, *str1 = NULL, *saveptr1 = NULL;
    int j;
    static int depth = -1;
    char **hierarchy = NULL;
    for (j = 0, str1 = line; ; j++, str1 = NULL){
      token = strtok_r (str1, ".", &saveptr1);
      if (token == NULL)
	break;
      hierarchy = (char **) realloc (hierarchy, (j+1)*sizeof(char*));
      hierarchy[j] = token;
    }

    //sanity check on number of fields
    if (depth == -1){
      depth = j;
    }else{
      if (depth != j){
	//ERROR, different number of fields on %s, line
      }
    }

    //create things (hierarchy)
    for (j = depth-1; j > 0; j--){
      char *container;
      if (j+1 == depth){
	container = NULL;
      }else{
	container = hierarchy[j+1];
      }
      char *new_container = hierarchy[j];

      char new_container_type[100];
      {
	//type
	char container_type[100];
	snprintf (new_container_type, 100, "L%d", j);
	snprintf (container_type, 100, "L%d", j+1);
	nf_container_type_declare (new_container_type, container_type);
      }
      {
	//container
	nf_container_create (new_container, new_container_type, container);

      }
    }

    //save hostname for later
    char *host = strdup(hierarchy[0]);
    char *cluster_container = hierarchy[j+1];

    //tokenize cluster-host
    char **cluster = NULL;
    for (j = 0, str1 = hierarchy[0]; ; j++, str1 = NULL){
      token = strtok_r (str1, "-", &saveptr1);
      if (token == NULL)
	break;
      cluster = (char **) realloc (cluster, (j+1)*sizeof(char*));
      cluster[j] = token;
    }

    //sanity check on number of fields for cluster-host
    if (j != 2){
      //ERROR, different number of fields expected for cluster-host pair
    }

    //create things (cluster)
    for (j = 0; j < 1; j++){
      char new_container_type[100];
      {
	char container_type[100];
	//type
	snprintf (new_container_type, 100, "L%d", j);
	snprintf (container_type, 100, "L%d", j+1);
	nf_container_type_declare (new_container_type, container_type);
      }
      {
	//container
	nf_container_create (cluster[j], new_container_type, cluster_container);
      }
    }
    char *host_container = cluster[j-1];

    //create things (host)
    nf_container_type_declare ("H", "L0");
    nf_container_create (host, "H", host_container);
    nf_hostname_add (host);

    //free stuff
    free (hierarchy);
    free (cluster);
    free (host);
    free (line);
    line = NULL;
  }

  fclose (arq);
  return 0;
}
