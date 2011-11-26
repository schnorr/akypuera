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
#ifndef __AKY_PRIVATE_H_
#define __AKY_PRIVATE_H_
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <rastro.h>
#include <string.h>
#include <search.h>
#include "aky.h"
#include "aky_rastro.h"

void aky_insert(MPI_Request * req);
int aky_check(MPI_Request * req);
void aky_remove(MPI_Request * req);
int AKY_translate_rank(MPI_Comm comm, int rank);

char *aky_put_key(const char *type, int src, int dst, char *key, int n);
char *aky_get_key(const char *type, int src, int dst, char *key, int n);

void pajeDefineContainerType(const char *alias,
                             const char *containerType, const char *name);
void pajeDefineStateType(const char *alias,
                         const char *containerType, const char *name);
void pajeDefineLinkType(const char *alias,
                        const char *containerType,
                        const char *sourceContainerType,
                        const char *destContainerType, const char *name);
void pajeCreateContainer(double timestamp,
                         const char *alias,
                         const char *type,
                         const char *container, const char *name);
void pajeDestroyContainer(double timestamp,
                          const char *type, const char *container);
void pajeSetState(double timestamp,
                  const char *container,
                  const char *type, const char *value);
void pajePushState(double timestamp,
                   const char *container,
                   const char *type, const char *value);
void pajePopState(double timestamp,
                  const char *container, const char *type);
void pajeStartLink(double timestamp,
                   const char *container,
                   const char *type,
                   const char *sourceContainer,
                   const char *value, const char *key);
void pajeEndLink(double timestamp,
                 const char *container,
                 const char *type,
                 const char *endContainer,
                 const char *value, const char *key);
void paje_header(void);
void paje_hierarchy(void);


typedef struct paje_event {
  const char *name;
  const char *description;
  int id;
} s_paje_event_t, *paje_event_t;

//prototypes for aky_names.c
void name_init (void);
char *name_get (int id);

#define AKY_DEFAULT_STR_SIZE 200

#endif //__AKY_PRIVATE_H_
