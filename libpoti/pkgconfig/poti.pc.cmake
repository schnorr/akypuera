prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: poti
Description: A library to generate paje traces
Version: @POTI_APIVERSI@
Libs: -L${libdir} -lpoti
Cflags: -I${includedir}
