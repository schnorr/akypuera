Akypuera INSTALL
================

Akypuera is a tracing library for MPI applications released under
GPLv3. The only dependencies are __git__, a compiler (__gcc__ or
clang) and __cmake__. We recommend an out-of-source compilation:

           $ git clone --recursive git://github.com/schnorr/akypuera.git
           $ mkdir build
           $ cmake ..
           $ make
           $ make install

Run `ccmake .` on the build directory if you need to customize
something (such as the installation directory).

Akypuera is bundled with a bunch of other tools for trace file
conversions towards the Paje file format. See the [Akypuera's
wiki](https://github.com/schnorr/akypuera/wiki) for a detailed
description of how to configure, build, and use each of them.

### Note

Akypuera has support for both OMPI (a real MPI implementation) and
SMPI (the [SimGrid's MPI interface to simulate MPI
applications](http://simgrid.gforge.inria.fr/simgrid/3.7/doc/group__SMPI__API.html)),
but __not at the same time__. So if you want to trace OMPI, you need
to -DSMPI=OFF and use mpicc to compile your application. If you want
to trace SMPI, you need to -DSMPI=ON and then use smpicc. [More
details on this page of the Akypuera's
wiki](https://github.com/schnorr/akypuera/wiki/SMPIWithAkypuera) to
know how to configure aky with SMPI.