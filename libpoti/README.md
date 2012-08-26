Poti 
====

Poti is a library to create trace files in the [Paje file
format](http://paje.sf.net). You should not use it directly during the
tracing of your application (I won't stop you from doing this if you
want). I recommend its use after traces are collected in your
preferred file format (probably binary with memory buffers to avoid at
a maximum the tracing intrusion).

*The current version of Poti does not provide any kind of correctness
verification in the parameters you pass for the library functions.*

Dependencies
------------

Poti needs nothing in special, you should be able to compile it using
your preferred compiler, and use it in any C program.

Clone and Compilation
---------------------

No secrets here, just do:

    $ git clone git://github.com/schnorr/poti.git
    $ cd poti
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

Installation
------------

The default installation directory is `/usr/local/`, but you are free
to change that according to your needs. If you want so, you may tell
cmake to use another installation directory with (I'm supposing you
are inside the `build` directory in the cloned directory:

    $ cmake -D CMAKE_INSTALL_PREFIX=/your/installation/directory/ ..

Examples
--------

There is one simple example in the `examples` directory. It is
compiled by default when you compile Poti. Try it with (I'm supposing
you are inside the `build` directory in the cloned directory, after
the `cmake` and `make` command):

    $ ./examples/ex1 

Roadmap
-------

This is the current TODO list:

* Check all function parameters and trace consistency according to the [Paje file format](http://paje.sf.net)
* Use data structures instead of strings for all parameters
* Add variable and instantaneous functions currently not implemented
* Extended events (you'll need one day probably)