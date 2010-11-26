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
#include <mpi.h>
#include <stdio.h>
#include <search.h>
#include <stdlib.h>
#include "rastro.h"
#include "aky.h"
#include "aky_rastro.h"

void *aky_ptp_root = NULL;

typedef struct aky {
  char key[100];
}aky_t;

static int aky_compare (const void *a, const void *b) 
{
  aky_t *aa, *ab;
  aa = (aky_t*)a;
  ab = (aky_t*)b;
  return strcmp (aa->key, ab->key);
}

void aky_insert (MPI_Request *req)
{
  aky_t *new;
  new = (aky_t*)calloc(1, sizeof(aky_t));
  snprintf (new->key, 100, "%p", req);
  tsearch (new, &aky_ptp_root, aky_compare);
}

void aky_remove (MPI_Request *req)
{
  aky_t *new;
  new = (aky_t*)calloc(1, sizeof(aky_t));
  snprintf (new->key, 100, "%p", req);
  tdelete (new, &aky_ptp_root, aky_compare);
}

int aky_check (MPI_Request *req)
{
  aky_t *new;
  new = (aky_t*)calloc(1, sizeof(aky_t));
  snprintf (new->key, 100, "%p", req);
  const void *ret = tfind (new, &aky_ptp_root, aky_compare);
  if (ret){
    return 1;
  }else{
    return 0;
  }
}

int AKY_translate_rank (MPI_Comm comm, int rank)
{
  MPI_Group group1, group2;
  PMPI_Comm_group (comm, &group1);
  PMPI_Comm_group (MPI_COMM_WORLD, &group2);
  int crank;
  PMPI_Group_translate_ranks (group1, 1, &rank, group2, &crank);
  return crank;
}
