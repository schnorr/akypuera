#
# Find the OTF2 library
#

# OTF2_INCLUDE_PATH  - Directories to include to use OTF2
# OTF2_LIBRARY_PATH    - Files to link against to use OTF2
# OTF2_FOUND        - When false, don't try to use OTF2
#
# OTF2_PATH can be used to make it simpler to find the various include
# directories and compiled libraries when OTF2 was not installed in the
# usual/well-known directories (e.g. because you made an in tree-source
# compilation or because you installed it in an "unusual" directory).
# Just set OTF2_PATH it to your specific installation directory
#
FIND_LIBRARY(OTF2_LIBRARY
  NAMES otf2
  PATHS /usr/lib /usr/local/lib ${OTF2_PATH}/lib)

IF(OTF2_LIBRARY)
  MESSAGE ( STATUS "Found OTF2 library: ${OTF2_LIBRARY}" )
  GET_FILENAME_COMPONENT(OTF2_LIBRARY_tmp "${OTF2_LIBRARY}" PATH)
  SET (OTF2_LIBRARY_PATH ${OTF2_LIBRARY_tmp} CACHE PATH "")
ELSE(OTF2_LIBRARY)
  SET (OTF2_LIBRARY_PATH "OTF2_LIBRARY_PATH-NOTFOUND")
  unset(LIBRARY_PATH CACHE)
ENDIF(OTF2_LIBRARY)

FIND_PATH( OTF2_INCLUDE_tmp otf2/otf2.h
  PATHS
  ${OTF2_GUESSED_INCLUDE_PATH}
  ${OTF2_PATH}/include/
  ${OTF2_PATH}/include/otf2/
  /usr/include/
  /usr/include/otf2/
  /usr/local/include/
  /usr/local/include/otf2/
)

IF(OTF2_INCLUDE_tmp)
  SET (OTF2_INCLUDE_PATH "${OTF2_INCLUDE_tmp}" CACHE PATH "")
ELSE(OTF2_INCLUDE_tmp)
  SET (OTF2_INCLUDE_PATH "OTF2_INCLUDE_PATH-NOTFOUND")
ENDIF(OTF2_INCLUDE_tmp)

IF( OTF2_INCLUDE_PATH )
  IF( OTF2_LIBRARY_PATH )
    SET( OTF2_FOUND TRUE )
  ENDIF ( OTF2_LIBRARY_PATH )
ENDIF( OTF2_INCLUDE_PATH )

IF( NOT OTF2_FOUND )
  MESSAGE(STATUS "OTF2 installation was not found. Please provide OTF2_PATH:")
  MESSAGE(STATUS "  - through the GUI when working with ccmake, ")
  MESSAGE(STATUS "  - as a command line argument when working with cmake e.g.")
  MESSAGE(STATUS "    cmake .. -DOTF2_PATH:PATH=/usr/local/otf2 ")
  SET(OTF2_PATH "" CACHE PATH "Root of OTF2 install tree." )
ENDIF( NOT OTF2_FOUND )

unset(OTF2_INCLUDE_tmp CACHE)
mark_as_advanced(OTF2_LIBRARY)
