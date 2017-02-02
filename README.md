Akypuera 
========

Akypuera is a tracing library for MPI applications released under
GPLv3.  The library, simplified as aky (linked with `-laky` to MPI
apps), uses librastro to provide low memory footprint and low
intrusion during tracing. The binary trace files are converted to the
[Paje file
format](http://paje.sourceforge.net/download/publication/lang-paje.pdf)
using the tool called `aky_converter`. Akypuera is a word from the
tupi language that means trace.

### Installation


Akypuera is a tracing library for MPI applications released under
GPLv3. The only dependencies are __git__, a compiler (__gcc__ or
clang) and __cmake__. Other dependencies are included in this
repository as git submodules so cloning as below should be sufficient
to take them automatically. We recommend an out-of-source compilation:

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

*Note:* Akypuera has support for both OMPI (a real MPI implementation)
and SMPI (the [SimGrid's MPI interface to simulate MPI
applications](http://simgrid.gforge.inria.fr/simgrid/3.7/doc/group__SMPI__API.html)),
but __not at the same time__. So if you want to trace OMPI, you need
to -DSMPI=OFF and use mpicc to compile your application. If you want
to trace SMPI, you need to -DSMPI=ON and then use smpicc. [More
details on this page of the Akypuera's
wiki](https://github.com/schnorr/akypuera/wiki/SMPIWithAkypuera) to
know how to configure aky with SMPI.

### Utilization

You can check the INSTALL.md file for synthetic compilation
instructions. You can also refer to [Akypuera's
wiki](https://github.com/schnorr/akypuera/wiki) for a long description
of all aky-related things.

MPI applications can be linked with libaky this way:

    $ mpicc source_file.c -o mpi_app -laky 
    or
    $ mpicc -c source_file.c
    $ mpicc source_file.o -o mpi_app -laky

You might want to specify the LD_LIBRARY_PATH to contain the path to
`libaky.so`, when executing the MPI application. The execution will
create a trace file for each MPI process. A run with 3 processes
generate this list of files:

    rastro-0-0.rst
    rastro-1-0.rst
    rastro-2-0.rst

Alternatively, you can link an already compiled MPI application with libaky
using `LD_PRELOAD`:

    LD_PRELOAD=/path/to/libaky.so:/path/to/libmpi.so.1 mpirun -np n ./myapp

aky_converter
-------------

Trace files created with akypuera are converted to the [Paje generic
format](http://paje.sf.net) using `aky_converter`, passing as
parameter __all the rst__ files generated by the mpi execution.  The
corresponding paje trace file is printed on __stdout__.

    $ aky_converter rastro*.rst

### Trace file converters

Akypuera is bundled with a bunch of other tools for trace file
conversions towards the Paje file format. 

tau2paje
--------

Converts trace files from the [TAU trace file
format](http://www.cs.uoregon.edu/Research/tau/) to the Paje's generic
file format.
[More about __tau2paje__ on Akypuera's wiki](https://github.com/schnorr/akypuera/wiki/TAUWithAkypuera).

otf2paje
--------

Converts trace files from the [OTF trace file format](http://www.tu-dresden.de/zih/otf/) to the Paje's generic
file format.
[More about __otf2paje__ on Akypuera's wiki](https://github.com/schnorr/akypuera/wiki/OTFWithAkypuera).

otf22paje
---------

Converts trace files from the [OTF2 trace file
format](http://www.vi-hps.org/projects/score-p/) to Paje's generic
file format.
[More about __otf22paje__ on Akypuera's wiki](https://github.com/schnorr/akypuera/wiki/OTF2WithAkypuera).
