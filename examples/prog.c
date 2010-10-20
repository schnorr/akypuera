#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {

   MPI_Init(&argc, &argv);
   MPI_Finalize();
   return 0;
}
