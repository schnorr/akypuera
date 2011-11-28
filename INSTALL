#
# Akypuera installation file
#

1/ Dependencies, recent versions of 
  - gcc compiler (or the clang compiler)
  - cmake

2/ Out-of-source compilation
  $ mkdir build
  $ cmake ..
  $ make
  $ make install

  => If you need to customize something, you can use 
  $ ccmake .. # inside the build directory

  => You can optionally configure it with TAU, to get tau2paje converter
  $ cmake .. -DTAU_PATH=/path/to/tau/

  => You can optionally configure it to work with SimGrid's SMPI instead of MPI.
  $ cmake .. -DSMPI_PATH=/path/to/smpi

Note: Akypuera has support for both OMPI (a real MPI implementation)
and SMPI (the SimGrid's MPI interface to simulate MPI applications),
but not at the same time. So if you want to trace OMPI, you need to
-DSMPI=OFF and use mpicc to compile your application. If you want to
trace SMPI, you need to -DSMPI=ON and then use smpicc.