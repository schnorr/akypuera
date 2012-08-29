#
# Find the OTF library
#

# OTF_INCLUDE_PATH  - Directories to include to use OTF
# OTF_LIBRARY_PATH    - Files to link against to use OTF
# OTF_FOUND        - When false, don't try to use OTF
#
# OTF_PATH can be used to make it simpler to find the various include
# directories and compiled libraries when OTF was not installed in the
# usual/well-known directories (e.g. because you made an in tree-source
# compilation or because you installed it in an "unusual" directory).
# Just set OTF_PATH it to your specific installation directory
#
FIND_LIBRARY(OTF_LIBRARY
  NAMES otf
  PATHS /usr/lib /usr/local/lib ${OTF_PATH}/lib)

IF(OTF_LIBRARY)
  MESSAGE ( STATUS "Found OTF library: ${OTF_LIBRARY}" )
  GET_FILENAME_COMPONENT(OTF_LIBRARY_tmp "${OTF_LIBRARY}" PATH)
  SET (OTF_LIBRARY_PATH ${OTF_LIBRARY_tmp} CACHE PATH "")
ELSE(OTF_LIBRARY)
  SET (OTF_LIBRARY_PATH "OTF_LIBRARY_PATH-NOTFOUND")
  unset(LIBRARY_PATH CACHE)
ENDIF(OTF_LIBRARY)


FIND_PATH( OTF_INCLUDE_tmp otf.h
  PATHS
  ${OTF_GUESSED_INCLUDE_PATH}
  ${OTF_PATH}/include/
  ${OTF_PATH}/include/otf2/
  /usr/include/
  /usr/include/otf2/
  /usr/local/include/
  /usr/local/include/otf2/
)

IF(OTF_INCLUDE_tmp)
  SET (OTF_INCLUDE_PATH "${OTF_INCLUDE_tmp}" CACHE PATH "")
ELSE(OTF_INCLUDE_tmp)
  SET (OTF_INCLUDE_PATH "OTF_INCLUDE_PATH-NOTFOUND")
ENDIF(OTF_INCLUDE_tmp)

IF( OTF_INCLUDE_PATH )
  IF( OTF_LIBRARY_PATH )
    SET( OTF_FOUND TRUE )
  ENDIF ( OTF_LIBRARY_PATH )
ENDIF( OTF_INCLUDE_PATH )

IF( NOT OTF_FOUND )
  MESSAGE(STATUS "OTF installation was not found. Please provide OTF_PATH:")
  MESSAGE(STATUS "  - through the GUI when working with ccmake, ")
  MESSAGE(STATUS "  - as a command line argument when working with cmake e.g.")
  MESSAGE(STATUS "    cmake .. -DOTF_PATH:PATH=/usr/local/otf/ ")
  SET(OTF_PATH "" CACHE PATH "Root of OTF install tree." )
ENDIF( NOT OTF_FOUND )

unset(OTF_INCLUDE_tmp CACHE)
mark_as_advanced(OTF_LIBRARY)
