Akypuera 
========

Akypuera is a tracing library for MPI applications released under
GPLv3. Throughout the years, it has become also a repository for
a plethora of converters from OTF2, TAU, Paraver file formats to the Paje
file format. The library, simplified as aky (linked with `-laky` to MPI
apps), uses librastro to provide low memory footprint and low
intrusion during tracing. The binary trace files are converted to the
[Paje file
format](http://paje.sourceforge.net/download/publication/lang-paje.pdf)
using the tool called `aky_converter`. Akypuera is a word from the
tupi language that means trace.

Akypuera has support for both OMPI (a real MPI implementation)
and SMPI (the [SimGrid's MPI interface to simulate MPI
applications](http://simgrid.gforge.inria.fr/)),
but __not at the same time__. So if you want to trace OMPI, you need
to -DSMPI=OFF and use mpicc to compile your application. If you want
to trace SMPI, you need to -DSMPI=ON and then use smpicc. [More
details on this page of the Akypuera's
wiki](https://github.com/schnorr/akypuera/wiki/SMPIWithAkypuera) to
know how to configure aky with SMPI.

Below we list a quick hands-on with aky. Refer to [Akypuera's
wiki](https://github.com/schnorr/akypuera/wiki) for extra details.

## Installation

No special dependencies are required.  Other dependencies are included
in this repository as git submodules so cloning as below should be
sufficient to take them automatically. We recommend an out-of-source
compilation (run `ccmake` on the build directory to customize the
compilation/installation):

    git clone --recursive git://github.com/schnorr/akypuera.git
    mkdir build
    cd build
    cmake ..
    make

## Utilization

There are many ways. If your MPI application is already compiled simply do:

    LD_PRELOAD=/path/to/libaky.so:/path/to/libmpi.so.1 mpirun -np n ./myapp

If you have access to the MPI application source code, you can use aky like this:

    mpicc source_file.c -o mpi_app -laky 

When executing, you might want to specify the `LD_LIBRARY_PATH` to
contain the path to `libaky.so`. The execution will create a binary
trace file in the rastro format for each MPI process. A run with 3
processes generate these files: `rastro-0-0.rst`, `rastro-1-0.rst`,
and `rastro-2-0.rst`. Trace files created with akypuera are converted
to the [Paje generic format](http://paje.sf.net) using
`aky_converter`, passing as parameter __all the rst__ files generated
by the mpi execution.  The corresponding paje trace file is printed on
__stdout__.

    aky_converter rastro*.rst

## Trace file converters

Akypuera is bundled with a bunch of other tools for trace file
conversions towards the Paje file format. See the [Akypuera's
wiki](https://github.com/schnorr/akypuera/wiki) for a detailed
description of how to configure, build, and use each of them. Here's a
quick list of them:

- `tau2paje`: Converts from the [TAU trace file
format](http://www.cs.uoregon.edu/Research/tau/) to the Paje's generic
file format. [More about __tau2paje__](https://github.com/schnorr/akypuera/wiki/TAUWithAkypuera).

#### Deprecated converters

- `otf2paje`: Converts from the [OTF trace file format](http://www.tu-dresden.de/zih/otf/) to the Paje's generic file format. 
[More about __otf2paje__](https://github.com/schnorr/akypuera/wiki/OTFWithAkypuera). This tool is currently deprecated and kept only for historical reasons. You might want to consider the adoption of the Open Trace Format 2 (OTF2) trace file format if you are coming from the OTF world (see next point).

- `otf22paje`: Converts from the [OTF2 trace file
format](http://www.vi-hps.org/projects/score-p/) to Paje's generic
file format.  [More about
__otf22paje__](https://github.com/schnorr/akypuera/wiki/OTF2WithAkypuera). The
tool relies on the OTF2 library released with
[ScoreP](http://score-p.org). Since the API for reading the binary
format of OTF2 traces change very often, any help to keep it updated
is very appreciated.  As of now, we rely on custom scripts (written in
bash, perl, and julia) that exploit the more stable output of the
`otf2-print` tool. So far, we have been using such scripts to convert
OpenMP traces created with ScoreP; they are available at [this
directory](./src/otf2-omp-print/). The perl and julia versions are
recommended.
