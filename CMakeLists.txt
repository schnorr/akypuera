cmake_minimum_required (VERSION 2.6)
if(POLICY CMP0015)
  cmake_policy(SET CMP0015 NEW)
endif(POLICY CMP0015)


project (aky)

#uncomment to enable debugging
#set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_C_FLAGS "-O0 -g")


add_subdirectory(${CMAKE_HOME_DIRECTORY}/libRastro)
add_subdirectory(${CMAKE_HOME_DIRECTORY}/examples)

find_package(MPI REQUIRED)
include(cmake/FindTAU.cmake)
include(cmake/FindMPI.cmake)

include_directories(${CMAKE_HOME_DIRECTORY}/include/)
include_directories(${CMAKE_HOME_DIRECTORY}/src/)
include_directories(${CMAKE_HOME_DIRECTORY}/libRastro/include/)
include_directories(${CMAKE_HOME_DIRECTORY}/libRastro/src/)
include_directories(${CMAKE_BINARY_DIR}/include/)
include_directories(${MPI_INCLUDE_PATH})

add_library (aky SHARED src/aky.c src/aky_aux.c src/aky_rastro.c)
target_link_libraries(aky Rastro)

add_executable(aky_converter src/aky_converter.c src/aky_paje.c src/aky_names.c src/aky_keys.c)
target_link_libraries(aky_converter Rastro)

if(TAU_FOUND)
  include_directories(${TAU_INCLUDE_PATH})
  link_directories(${TAU_LIBRARY_PATH})
  add_executable (tau2paje src/tau2paje.c src/aky_paje.c src/aky_keys.c)
  target_link_libraries (tau2paje TAU TAU_tf)
  install(PROGRAMS tau2paje
          DESTINATION bin)
endif(TAU_FOUND)

install(TARGETS aky
        DESTINATION lib)
install(FILES include/aky.h
        DESTINATION include)
install(PROGRAMS aky_converter
        DESTINATION bin)

#packaging
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_GENERATOR "TGZ")
SET(CPACK_SOURCE_GENERATOR "TGZ")
include(CPack)
