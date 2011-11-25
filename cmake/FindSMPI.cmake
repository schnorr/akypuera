#
# Find the SMPI library
#

# SMPI_INCLUDE_PATH  - Directories to include to use SMPI
# SMPI_LIBRARY_PATH    - Files to link against to use SMPI
# SMPI_FOUND        - When false, don't try to use SMPI
#
# SMPI_PATH can be used to make it simpler to find the various include
# directories and compiled libraries when SMPI was not installed in the
# usual/well-known directories (e.g. because you made an in tree-source
# compilation or because you installed it in an "unusual" directory).
# Just set SMPI_PATH it to your specific installation directory
#
FIND_LIBRARY(SMPI_LIBRARY
  NAMES smpi
  PATHS /usr/lib /usr/local/lib ${SMPI_PATH}/lib)

IF(SMPI_LIBRARY)
  MESSAGE ( STATUS "Found SMPI library: ${SMPI_LIBRARY}" )
  GET_FILENAME_COMPONENT(SMPI_LIBRARY_tmp "${SMPI_LIBRARY}" PATH)
  SET (SMPI_LIBRARY_PATH CACHE PATH "" ${SMPI_LIBRARY_tmp})
ELSE(SMPI_LIBRARY)
  SET (SMPI_LIBRARY_PATH CACHE PATH "" "SMPI_LIBRARY_PATH-NOTFOUND")
  unset(LIBRARY_PATH CACHE)
ENDIF(SMPI_LIBRARY)

FIND_PATH( SMPI_INCLUDE_tmp smpi.h
  PATHS
  ${SMPI_GUESSED_INCLUDE_PATH}
  ${SMPI_PATH}/include/
  ${SMPI_PATH}/include/smpi/
  /usr/include/
  /usr/include/smpi/
  /usr/local/include/
  /usr/local/include/smpi/
)

FIND_PATH (MPI_INCLUDE_tmp smpi/mpi.h
  PATHS
  ${SMPI_GUESSED_INCLUDE_PATH}
  ${SMPI_PATH}/include/
  ${SMPI_PATH}/include/smpi/
  /usr/include/
  /usr/include/smpi/
  /usr/local/include/
  /usr/local/include/smpi/
)

IF(SMPI_INCLUDE_tmp)
  SET (SMPI_INCLUDE_PATH
    CACHE
    PATH
    "SMPI_INCLUDE_PATH"
    ${SMPI_INCLUDE_tmp} ${MPI_INCLUDE_tmp})
ELSE(SMPI_INCLUDE_tmp)
  SET (SMPI_INCLUDE_PATH
    CACHE
    PATH
    "SMPI_INCLUDE_PATH"
    "SMPI_INCLUDE_PATH-NOTFOUND")
ENDIF(SMPI_INCLUDE_tmp)

IF( SMPI_INCLUDE_PATH )
  IF( SMPI_LIBRARY_PATH )
    SET( SMPI_FOUND TRUE )
  ENDIF ( SMPI_LIBRARY_PATH )
ENDIF( SMPI_INCLUDE_PATH )

IF( NOT SMPI_FOUND )
  MESSAGE(STATUS "SMPI installation was not found. Please provide SMPI_PATH:")
  MESSAGE(STATUS "  - through the GUI when working with ccmake, ")
  MESSAGE(STATUS "  - as a command line argument when working with cmake e.g.")
  MESSAGE(STATUS "    cmake .. -DSMPI_PATH:PATH=/usr/local/smpi ")
  SET(SMPI_PATH "" CACHE PATH "Root of SMPI install tree." )
ENDIF( NOT SMPI_FOUND )

unset(SMPI_INCLUDE_tmp CACHE)
unset(MPI_INCLUDE_tmp CACHE)
mark_as_advanced(SMPI_LIBRARY)
