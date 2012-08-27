cmake_minimum_required (VERSION 2.6)
if(POLICY CMP0015)
  cmake_policy(SET CMP0015 NEW)
endif(POLICY CMP0015)

PROJECT(aky)
SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

OPTION(SMPI
  "Enable to trace MPI applications linked against SMPI (THREADED required)"
  OFF)

OPTION(TAU
  "Enable to build tau2paje converter"
  OFF)

OPTION(OTF2
  "Enable to build otf22paje converter"
  OFF)

OPTION(OTF
  "Enable to build otf2paje converter"
  OFF)

OPTION(DEBUG
  "Debug akypuera"
  OFF)

EXEC_PROGRAM("git --git-dir=${CMAKE_HOME_DIRECTORY}/.git log --oneline -1"
  OUTPUT_VARIABLE "GITVERSION")
EXEC_PROGRAM("git --git-dir=${CMAKE_HOME_DIRECTORY}/.git log -n 1 --format=%ai"
  OUTPUT_VARIABLE "GITDATE")

SET(CMAKE_C_FLAGS "-Wall -Wunused -Wmissing-prototypes -Wmissing-declarations -Wpointer-arith -Wchar-subscripts -Wcomment -Wformat -Wwrite-strings -Wno-unused-function -Wno-unused-parameter -Wno-strict-aliasing -Wno-format-nonliteral -Werror ")

# aky_config.h.in file
CONFIGURE_FILE("${aky_SOURCE_DIR}/include/aky_config.h.in"
               "${CMAKE_BINARY_DIR}/include/aky_config.h" @ONLY IMMEDIATE)

SET(OTF_SOURCES src/otf/otf2paje.c src/otf/otf2paje_arguments.c src/otf/otf2paje_handlers.c src/aky_paje.c)
SET(OTF2_SOURCES src/otf2/otf22paje.c src/aky_paje.c)
SET(TAU_SOURCES src/tau/tau2paje.c src/aky_paje.c src/aky_keys.c)

# debugging flags
IF(DEBUG)
  set(CMAKE_BUILD_TYPE Debug)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g")
ENDIF(DEBUG)

# akypuera configuration for SMPI or MPI (but not both)
IF(SMPI)
  FIND_PACKAGE(SMPI REQUIRED)
  IF(SMPI_FOUND)
    INCLUDE_DIRECTORIES(${SMPI_INCLUDE_PATH})
  ENDIF(SMPI_FOUND)
ELSE(SMPI)
  FIND_PACKAGE(MPI REQUIRED)
  IF(MPI_C_FOUND)
    INCLUDE_DIRECTORIES(${MPI_C_INCLUDE_PATH})
  ENDIF(MPI_C_FOUND)
ENDIF(SMPI)

# tau2paje converter
IF(TAU)
  FIND_PACKAGE(TAU REQUIRED)
ENDIF(TAU)

# otf22paje converter
IF(OTF2)
  FIND_PACKAGE(OTF2 REQUIRED)
ENDIF(OTF2)

# otf2paje converter
IF(OTF)
  FIND_PACKAGE(OTF REQUIRED)
ENDIF(OTF)

add_subdirectory(librastro)
add_subdirectory(libpoti)
add_subdirectory(examples)

include_directories(${aky_SOURCE_DIR}/include/)
include_directories(${aky_SOURCE_DIR}/src/)
include_directories(${poti_SOURCE_DIR}/include/)
include_directories(${rastro_SOURCE_DIR}/include/)
include_directories(${rastro_SOURCE_DIR}/src/)
include_directories(${aky_BINARY_DIR}/include/)

add_library (aky SHARED src/aky.c src/aky_aux.c src/aky_rastro.c)
target_link_libraries(aky rastro)
target_link_libraries(aky m)

add_executable(aky_converter src/aky_converter.c src/aky_paje.c src/aky_names.c src/aky_keys.c)
target_link_libraries(aky_converter rastro)
target_link_libraries(aky_converter m)
target_link_libraries(aky_converter poti)

if(OTF2_FOUND)
  INCLUDE_DIRECTORIES(${OTF2_INCLUDE_PATH})
  LINK_DIRECTORIES(${OTF2_LIBRARY_PATH})
  add_executable(otf22paje ${OTF2_SOURCES})
  target_link_libraries(otf22paje otf2)
  target_link_libraries(otf22paje scorep_utilities)
  target_link_libraries(otf22paje m)
  target_link_libraries(otf22paje poti)
endif(OTF2_FOUND)

if(TAU_FOUND)
  INCLUDE_DIRECTORIES(${TAU_INCLUDE_PATH})
  LINK_DIRECTORIES(${TAU_LIBRARY_PATH})
  add_executable (tau2paje ${TAU_SOURCES})
  target_link_libraries (tau2paje TAU TAU_tf stdc++ m)
  target_link_libraries (tau2paje poti)
endif(TAU_FOUND)

if(OTF_FOUND)
  INCLUDE_DIRECTORIES (${OTF_INCLUDE_PATH})
  LINK_DIRECTORIES (${OTF_LIBRARY_PATH})
  ADD_EXECUTABLE (otf2paje ${OTF_SOURCES})
  TARGET_LINK_LIBRARIES (otf2paje otf)
  TARGET_LINK_LIBRARIES (otf2paje poti)
endif(OTF_FOUND)

# Installation part
INSTALL(TARGETS aky DESTINATION lib)
INSTALL(PROGRAMS ${aky_BINARY_DIR}/aky_converter DESTINATION bin)
IF(TAU_FOUND)
  INSTALL(PROGRAMS ${aky_BINARY_DIR}/tau2paje DESTINATION bin)
ENDIF(TAU_FOUND)
IF(OTF2_FOUND)
  INSTALL(PROGRAMS ${aky_BINARY_DIR}/otf22paje DESTINATION bin)
ENDIF(OTF2_FOUND)

MARK_AS_ADVANCED(CMAKE_BUILD_TYPE)

#packaging
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_GENERATOR "TGZ")
SET(CPACK_SOURCE_GENERATOR "TGZ")
include(CPack)
