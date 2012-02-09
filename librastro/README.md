librastro README file
=====================

1. Application Instrumentation

The application to be traced must be instrumented with functions that
start with **rst_event_**. All events are timestamped according to the
local timestamp of the machine in the moment the tracing function is
executed.  New functions can be created arbitrary by a
`rastro_generate` (see below for additional information, or
`rastro_generate --help`), so the user is not limited by existing
functions. A **rst_event_** function is on the following form:

rst_event_X where a X can be one or a combination of:

      c - 8 bits
      w - 16 bits
      i - 32 bits
      l - 64 bits
      f - float
      d - double
      s - string

The size of each event is unlimited (actually, hard-coded to 1000
bytes, but you can change that if you need even bigger events), so
users can provide as many parameters as desired. Examples:

      rst_event_iicilf(...)
      rst_event_siicicffll(...)

2. Instrumentation Functions Generation

**librastro** has a program to automatically implement functions
defined by the user and that are not present in the library. The user
is encouraged to launch the program with the help parameter to obtain
more information:

     $ rastro_generate --help

One possible way to pass the functions to be implemented to
rastro_generate is to create a textual file with one combination of X
(as described above) per line, like this:

     $ cat used_functions.txt
     ii
     fl
     ds

Then, the user can call `rastro_generate` passing the -i parameter and
the corresponding file:

     $ rastro_generate -h code.h -c code.c -i used_functions.txt

3. Compiling generated functions and librastro linking

The user has to add to the compilation chain the files created by
`rastro_generate`. Then, link the final binary to the librastro library
using `-lrastro` as parameter to the linker.
