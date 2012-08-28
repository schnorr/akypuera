CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
PROJECT(aky)

SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Source files
SET(AKY_LIBRARY_SOURCES src/aky.c src/aky_aux.c src/aky_rastro.c)
SET(AKY_CONVERTER_SOURCES src/aky/aky_converter.c src/aky/aky_arguments.c src/aky_utils.c src/aky_names.c src/aky_keys.c)
SET(OTF_SOURCES src/otf/otf2paje.c src/otf/otf2paje_arguments.c src/otf/otf2paje_handlers.c src/aky_utils.c)
SET(OTF2_SOURCES src/otf2/otf22paje.c src/otf2/otf22paje_arguments.c src/otf2/otf22paje_handlers.c src/aky_utils.c)
SET(TAU_SOURCES src/tau/tau2paje.c src/tau/tau2paje_arguments.c src/tau/tau2paje_handlers.c src/aky_utils.c src/aky_keys.c)

# Options
OPTION(SMPI "Enable to trace MPI applications linked against SMPI (THREADED required)" OFF)
OPTION(TAU  "Enable to build tau2paje converter" OFF)
OPTION(OTF2 "Enable to build otf22paje converter" OFF)
OPTION(OTF  "Enable to build otf2paje converter" OFF)

# Sane parameters
SET(CMAKE_C_FLAGS "-Wall -Wunused -Wmissing-prototypes -Wmissing-declarations -Wpointer-arith -Wchar-subscripts -Wcomment -Wformat -Wwrite-strings -Wno-unused-function -Wno-unused-parameter -Wno-strict-aliasing -Wno-format-nonliteral -Werror ")

# aky_config.h.in file
EXEC_PROGRAM("git --git-dir=${CMAKE_HOME_DIRECTORY}/.git log --oneline -1" OUTPUT_VARIABLE "GITVERSION")
EXEC_PROGRAM("git --git-dir=${CMAKE_HOME_DIRECTORY}/.git log -n 1 --format=%ai" OUTPUT_VARIABLE "GITDATE")
CONFIGURE_FILE("${aky_SOURCE_DIR}/include/aky_config.h.in" "${CMAKE_BINARY_DIR}/include/aky_config.h" @ONLY IMMEDIATE)

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

# check for tau2paje dependencies
IF(TAU)
  FIND_PACKAGE(TAU REQUIRED)
ENDIF(TAU)

# check for otf2paje dependencies
IF(OTF)
  FIND_PACKAGE(OTF REQUIRED)
ENDIF(OTF)

# check for otf22paje dependencies
IF(OTF2)
  FIND_PACKAGE(OTF2 REQUIRED)
ENDIF(OTF2)

# Independent subdirectories
ADD_SUBDIRECTORY(librastro)
ADD_SUBDIRECTORY(libpoti)
ADD_SUBDIRECTORY(examples)

# Include directories (shared by everyone)
INCLUDE_DIRECTORIES(${aky_SOURCE_DIR}/include/)
INCLUDE_DIRECTORIES(${aky_SOURCE_DIR}/src/)
INCLUDE_DIRECTORIES(${poti_SOURCE_DIR}/include/)
INCLUDE_DIRECTORIES(${rastro_SOURCE_DIR}/include/)
INCLUDE_DIRECTORIES(${rastro_SOURCE_DIR}/src/)
INCLUDE_DIRECTORIES(${aky_BINARY_DIR}/include/)

###################
##### TARGETS #####
###################

# libaky.so
ADD_LIBRARY(aky SHARED ${AKY_LIBRARY_SOURCES})
TARGET_LINK_LIBRARIES(aky rastro)
TARGET_LINK_LIBRARIES(aky m)

# aky_converter
ADD_EXECUTABLE(aky_converter ${AKY_CONVERTER_SOURCES})
TARGET_LINK_LIBRARIES(aky_converter rastro)
TARGET_LINK_LIBRARIES(aky_converter poti)
TARGET_LINK_LIBRARIES(aky_converter m)

# tau2paje
IF(TAU_FOUND)
  INCLUDE_DIRECTORIES(${TAU_INCLUDE_PATH})
  LINK_DIRECTORIES(${TAU_LIBRARY_PATH})
  ADD_EXECUTABLE(tau2paje ${TAU_SOURCES})
  TARGET_LINK_LIBRARIES(tau2paje TAU)
  TARGET_LINK_LIBRARIES(tau2paje TAU_tf)
  TARGET_LINK_LIBRARIES(tau2paje stdc++)
  TARGET_LINK_LIBRARIES(tau2paje poti)
  TARGET_LINK_LIBRARIES(tau2paje m)
ENDIF(TAU_FOUND)

# otf2paje
IF(OTF_FOUND)
  INCLUDE_DIRECTORIES (${OTF_INCLUDE_PATH})
  LINK_DIRECTORIES (${OTF_LIBRARY_PATH})
  ADD_EXECUTABLE (otf2paje ${OTF_SOURCES})
  TARGET_LINK_LIBRARIES (otf2paje otf)
  TARGET_LINK_LIBRARIES (otf2paje poti)
ENDIF(OTF_FOUND)

# otf22paje
IF(OTF2_FOUND)
  INCLUDE_DIRECTORIES(${OTF2_INCLUDE_PATH})
  LINK_DIRECTORIES(${OTF2_LIBRARY_PATH})
  ADD_EXECUTABLE(otf22paje ${OTF2_SOURCES})
  TARGET_LINK_LIBRARIES(otf22paje otf2)
  TARGET_LINK_LIBRARIES(otf22paje scorep_utilities)
  TARGET_LINK_LIBRARIES(otf22paje poti)
  TARGET_LINK_LIBRARIES(otf22paje m)
ENDIF(OTF2_FOUND)

# Installation part
INSTALL(TARGETS aky DESTINATION lib)
INSTALL(PROGRAMS ${aky_BINARY_DIR}/aky_converter DESTINATION bin)
IF(TAU_FOUND)
  INSTALL(PROGRAMS ${aky_BINARY_DIR}/tau2paje DESTINATION bin)
ENDIF(TAU_FOUND)
IF(OTF_FOUND)
  INSTALL(PROGRAMS ${aky_BINARY_DIR}/otf2paje DESTINATION bin)
ENDIF(OTF_FOUND)
IF(OTF2_FOUND)
  INSTALL(PROGRAMS ${aky_BINARY_DIR}/otf22paje DESTINATION bin)
ENDIF(OTF2_FOUND)