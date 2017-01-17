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
#include "aky_private.h"
#include <search.h>

void *aky_irecv_root = NULL;
void *aky_isend_root = NULL;

typedef struct aky {
  char key[AKY_DEFAULT_STR_SIZE];
  int mark;
} aky_t;

static int aky_compare(const void *a, const void *b)
{
  aky_t *aa, *ab;
  aa = (aky_t *) a;
  ab = (aky_t *) b;
  return strcmp(aa->key, ab->key);
}

void aky_insert(MPI_Request * req, void **root, int mark)
{
  aky_t *new;
  new = (aky_t *) calloc(1, sizeof(aky_t));
  snprintf(new->key, AKY_DEFAULT_STR_SIZE, "%p", req);
  new->mark = mark;
  tsearch(new, root, aky_compare);
}

void aky_remove(MPI_Request * req, void **root)
{
  aky_t *new;
  new = (aky_t *) calloc(1, sizeof(aky_t));
  snprintf(new->key, AKY_DEFAULT_STR_SIZE, "%p", req);
  tdelete(new, root, aky_compare);
}

int aky_check(MPI_Request * req, void **root)
{
  aky_t *new;
  new = (aky_t *) calloc(1, sizeof(aky_t));
  snprintf(new->key, AKY_DEFAULT_STR_SIZE, "%p", req);
  const void *ret = tfind(new, root, aky_compare);
  if (ret) {
    aky_t *ans = (*(aky_t **)ret);
    return ans->mark ? ans->mark : -1;
  } else {
    return 0;
  }
}

int AKY_translate_rank(MPI_Comm comm, int rank)
{
  MPI_Group group1, group2;
  PMPI_Comm_group(comm, &group1);
  PMPI_Comm_group(MPI_COMM_WORLD, &group2);
  int crank;
  PMPI_Group_translate_ranks(group1, 1, &rank, group2, &crank);
  return crank;
}

void
aky_insert_irecv(MPI_Request *req)
{
  aky_insert(req, &aky_irecv_root, 1);
}

void
aky_remove_irecv(MPI_Request *req)
{
  aky_remove(req, &aky_irecv_root);
}

int
aky_check_irecv(MPI_Request *req)
{
  return aky_check(req, &aky_irecv_root);
}

void
aky_insert_isend(MPI_Request *req, int mark)
{
  aky_insert(req, &aky_isend_root, mark);
}

void
aky_remove_isend(MPI_Request *req)
{
  aky_remove(req, &aky_isend_root);
}

int
aky_check_isend(MPI_Request *req)
{
  return aky_check(req, &aky_isend_root);
}
