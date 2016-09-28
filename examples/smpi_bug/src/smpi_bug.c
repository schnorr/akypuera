/* Copyright (c) 2009, 2010. The SimGrid Team.
 * All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define DATATOSENT 100000

int main(int argc, char *argv[])
{
  int rank, numprocs, i;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int N = 3;
  int tag = 12345;
  if (rank == 0) {
    MPI_Request req[2 * N];
    MPI_Status sta[2 * N];
    int *r = (int *) malloc(sizeof(int) * DATATOSENT);
    for (i = 0; i < 2 * N; i++) {
      if (i < N) {
        MPI_Irecv(r, DATATOSENT, MPI_INT, 1, tag, MPI_COMM_WORLD,
                  &req[i]);
      } else {
        MPI_Irecv(r, DATATOSENT, MPI_INT, 2, tag, MPI_COMM_WORLD,
                  &req[i]);
      }
    }
    MPI_Waitall(2 * N, req, sta);
    for (i = 0; i < 2 * N; i++){
      fprintf (stderr, "RANK-0: request = %p status = %p status.source = %d\n", 
          &req[i], &sta[i], sta[i].MPI_SOURCE);
    }

  } else if (rank == 1) {
    int *r = (int *) malloc(sizeof(int) * DATATOSENT);
    for (i = 0; i < N; i++) {
      MPI_Send(r, DATATOSENT, MPI_INT, 0, tag, MPI_COMM_WORLD);
    }
  } else if (rank == 2) {
    int *r = (int *) malloc(sizeof(int) * DATATOSENT);
    for (i = 0; i < N; i++) {
      MPI_Send(r, DATATOSENT, MPI_INT, 0, tag, MPI_COMM_WORLD);
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  return 0;
}
