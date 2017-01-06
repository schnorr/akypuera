#include <mpi.h>
int
main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int sbuff[4],
      rbuff[4];
  sbuff[0] = rank;
  MPI_Scatter(sbuff, 1, MPI_INT, rbuff, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(sbuff, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Gather(sbuff, 1, MPI_INT, rbuff, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Reduce(sbuff, rbuff, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  MPI_Finalize();
  return 0;
}
