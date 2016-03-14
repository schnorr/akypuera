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
#include <poti.h>
#include "aky.h"
#include "aky_config.h"
#include "aky_rastro.h"

void aky_insert(MPI_Request * req);
int aky_check(MPI_Request * req);
void aky_remove(MPI_Request * req);
int AKY_translate_rank(MPI_Comm comm, int rank);

int aky_key_init(void);
void aky_key_free(void);
char *aky_put_key(const char *type, int src, int dst, char *key, int n);
char *aky_get_key(const char *type, int src, int dst, char *key, int n);
void aky_paje_hierarchy(void);
int aky_dump_version (const char *program, char **argv, int argc);
int aky_dump_comment_file (const char *program, const char *filename);
int aky_dump_comment (const char *program, const char *comment);

//prototypes for aky_names.c
void name_init (void);
char *name_get (u_int16_t id);

#define AKY_DEFAULT_STR_SIZE 200
#define AKY_INPUT_SIZE 10000


#endif //__AKY_PRIVATE_H_
