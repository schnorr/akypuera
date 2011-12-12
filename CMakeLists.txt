cmake_minimum_required (VERSION 2.6)
if(POLICY CMP0015)
  cmake_policy(SET CMP0015 NEW)
endif(POLICY CMP0015)

PROJECT(aky)
SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

OPTION(SMPI
  "Enable to trace MPI applications linked against SimGrid's SMPI"
  OFF)

OPTION(TAU
  "Enable to create tau2paje converter"
  ON)

OPTION(OTF2
  "Enable to create otf2paje converter"
  ON)

OPTION(DEBUG
  "Debug akypuera"
  OFF)

# aky_config.h.in file
CONFIGURE_FILE("${aky_SOURCE_DIR}/include/aky_config.h.in"
               "${CMAKE_BINARY_DIR}/include/aky_config.h" @ONLY IMMEDIATE)

# debugging flags
IF(DEBUG)
  set(CMAKE_BUILD_TYPE Debug)
  set(CMAKE_C_FLAGS "-O0 -g")
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

# otf2paje converter
IF(OTF2)
  FIND_PACKAGE(OTF2 REQUIRED)
ENDIF(OTF2)

add_subdirectory(librastro)
add_subdirectory(examples)

include_directories(${aky_SOURCE_DIR}/include/)
include_directories(${aky_SOURCE_DIR}/src/)
include_directories(${rastro_SOURCE_DIR}/include/)
include_directories(${rastro_SOURCE_DIR}/src/)
include_directories(${aky_BINARY_DIR}/include/)

add_library (aky SHARED src/aky.c src/aky_aux.c src/aky_rastro.c)
target_link_libraries(aky rastro)

add_executable(aky_converter src/aky_converter.c src/aky_paje.c src/aky_names.c src/aky_keys.c)
target_link_libraries(aky_converter rastro)

if(OTF2_FOUND)
  INCLUDE_DIRECTORIES(${OTF2_INCLUDE_PATH})
  LINK_DIRECTORIES(${OTF2_LIBRARY_PATH})
  add_executable(otf2paje src/otf2paje.c src/aky_paje.c)
  target_link_libraries(otf2paje otf2)
  target_link_libraries(otf2paje scorep_utilities)
  target_link_libraries(otf2paje m)
endif(OTF2_FOUND)

if(TAU_FOUND)
  INCLUDE_DIRECTORIES(${TAU_INCLUDE_PATH})
  LINK_DIRECTORIES(${TAU_LIBRARY_PATH})
  add_executable (tau2paje src/tau2paje.c src/aky_paje.c src/aky_keys.c)
  target_link_libraries (tau2paje TAU TAU_tf)  
endif(TAU_FOUND)

# Installation part
INSTALL(TARGETS aky DESTINATION lib)
INSTALL(PROGRAMS ${aky_BINARY_DIR}/aky_converter DESTINATION bin)
IF(TAU_FOUND)
  INSTALL(PROGRAMS ${aky_BINARY_DIR}/tau2paje DESTINATION bin)
ENDIF(TAU_FOUND)

MARK_AS_ADVANCED(CMAKE_BUILD_TYPE)

#packaging
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_GENERATOR "TGZ")
SET(CPACK_SOURCE_GENERATOR "TGZ")
include(CPack)
