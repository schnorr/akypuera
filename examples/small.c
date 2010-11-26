#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) 
{
  int rank, x=1;
  MPI_Request request;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);

  int index;
  if (rank == 0){
    MPI_Send (&x, 1, MPI_INT, 1, 9999, MPI_COMM_WORLD);
//    MPI_Waitany (1, &request, &index, MPI_STATUS_IGNORE);
  }else if (rank == 1){
    MPI_Recv (&x, 1, MPI_INT, 0, 9999, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  MPI_Finalize();
  return 0;
/*
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);
  if (rank == 1){
    MPI_Send (&rank, 1, MPI_INT, 0, 9999, MPI_COMM_WORLD);
  }else if (rank == 0){
    MPI_Irecv (&rank, 1, MPI_INT, 1, 9999, MPI_COMM_WORLD, &request);
    MPI_Waitany (1, &request, &index, MPI_STATUS_IGNORE);
  }
*/

  MPI_Finalize();
  return 0;
}
