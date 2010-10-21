#
# Find the TAU libraries and include dir
#

# TAU_INCLUDE_DIR  - Directories to include to use TAU
# TAU_LIBRARY    - Files to link against to use TAU
# TAU_FOUND        - When false, don't try to use TAU
#
# TAU_DIR can be used to make it simpler to find the various include
# directories and compiled libraries when TAU was not installed in the
# usual/well-known directories (e.g. because you made an in tree-source
# compilation or because you installed it in an "unusual" directory).
# Just set TAU_DIR it to your specific installation directory
#
FIND_LIBRARY(TAU_LIBRARY TAU_traceinput
  PATHS
  /usr/lib
  /usr/local/lib
  ${TAU_DIR}/lib
)

IF(TAU_LIBRARY)
  GET_FILENAME_COMPONENT(TAU_GUESSED_INCLUDE_DIR_tmp "${TAU_LIBRARY}" PATH)
  STRING(REGEX REPLACE "lib$" "include" TAU_GUESSED_INCLUDE_DIR "${TAU_GUESSED_INCLUDE_DIR_tmp}")
ENDIF(TAU_LIBRARY)

FIND_PATH( TAU_INCLUDE_DIR TAU_tf.h
  PATHS
  ${TAU_GUESSED_INCLUDE_DIR}
  ${TAU_DIR}/include
  /usr/include
  /usr/local/include
)

IF( NOT TAU_FOUND )
  MESSAGE("TAU installation was not found. Please provide TAU_DIR:")
  MESSAGE("  - through the GUI when working with ccmake, ")
  MESSAGE("  - as a command line argument when working with cmake e.g. ")
  MESSAGE("    cmake .. -DTAU_DIR:PATH=/usr/local/tau ")
  MESSAGE("Note: the following message is triggered by cmake on the first ")
  MESSAGE("    undefined necessary PATH variable (e.g. TAU_INCLUDE_DIR).")
  MESSAGE("    Providing TAU_DIR (as above described) is probably the")
  MESSAGE("    simplest solution unless you have a really customized/odd")
  MESSAGE("    TAU installation...")
  SET(TAU_DIR "" CACHE PATH "Root of TAU install tree." )
ENDIF( NOT TAU_FOUND )
