#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) 
{
  int rank, x=1;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);

  if (rank == 0){
    MPI_Send (&x, 1, MPI_INT, 1, 9999, MPI_COMM_WORLD);
  }else if (rank == 1){
    MPI_Recv (&x, 1, MPI_INT, 0, 9999, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  MPI_Finalize();
  return 0;
}
