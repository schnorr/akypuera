c  Fortran example  
      program hello
      include 'mpif.h'
      integer rank, size, ierror, tag, status(MPI_STATUS_SIZE)
      
      call mpi_init(ierror)
      call mpi_comm_size(MPI_COMM_WORLD, size, ierror)
      call mpi_comm_rank(MPI_COMM_WORLD, rank, ierror)
      print*, 'node', rank, ': Hello world'
      call mpi_finalize(ierror)
      end
