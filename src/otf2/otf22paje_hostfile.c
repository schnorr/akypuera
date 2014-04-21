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

static char **types = NULL;
static int n_types = 0;

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

static int nf_type_declared (const char *type)
{
  int i;

  //check if already declared
  for (i = 0; i < n_types; i++){
    if (strcmp (types[i], type) == 0) return 1;
  }

  return 0;
}

static void nf_type_declare (const char *type)
{
  //add
  n_types++;
  types = realloc (types, n_types*sizeof(char*));
  types[n_types-1] = strdup (type);
}

void nf_container_type_declare (const char *new_container_type, const char *container_type)
{
  if (nf_type_declared (new_container_type)) return;
  nf_type_declare (new_container_type);

  if (container_type == NULL){
    container_type = "0";
  }
  poti_DefineContainerType (new_container_type, container_type, new_container_type);
}

void nf_state_type_declare (const char *new_state_type, const char *container_type)
{
  if (nf_type_declared (new_state_type)) return;
  nf_type_declare (new_state_type);

  poti_DefineStateType (new_state_type, container_type, new_state_type);
}

void nf_link_type_declare (const char *new_link_type, const char *container_type, const char *source_cont_type, const char *dest_cont_type)
{
  if (nf_type_declared (new_link_type)) return;
  nf_type_declare (new_link_type);

  poti_DefineLinkType(new_link_type, container_type, source_cont_type, dest_cont_type, new_link_type);
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
  if (container == NULL){
    container = "0";
  }
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
	fprintf(stderr,
		"[%s] at %s,"
		"different number of hierarchy fields in hostfile.\n"
		"Hierarchy fields are separated by a dot.\n",
		PROGRAM, __FUNCTION__);
	exit(1);
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
	snprintf (new_container_type, 100, "L%d", j);
	if (j+1 == depth){
	  nf_container_type_declare (new_container_type, NULL);
	}else{
	  char container_type[100];
	  snprintf (container_type, 100, "L%d", j+1);
	  nf_container_type_declare (new_container_type, container_type);
	}
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
      fprintf(stderr,
	      "[%s] at %s,"
	      "different number of fields expected for cluster-host pair\n"
	      "Cluster-host pair fields are separated by a dash.\n",
            PROGRAM, __FUNCTION__);
      exit(1);
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
