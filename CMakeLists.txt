cmake_minimum_required (VERSION 2.6)
project (aky)

set(CMAKE_BUILD_TYPE Debug)

find_file(HAVE_RASTRO_PUBLIC_H rastro_public.h)
add_subdirectory(libRastro)
add_subdirectory(examples)

find_package(MPI REQUIRED)
include(cmake/FindTAU.cmake)

include_directories(${MPI_INCLUDE_PATH} include/)
include_directories(${MPI_INCLUDE_PATH} libRastro/include/)
add_library (aky SHARED src/aky.c)
target_link_libraries(aky Rastro)

add_executable(aky_converter src/aky_converter.c src/aky_paje.c src/aky_names.c)
target_link_libraries(aky_converter Rastro)

if(TAU_FOUND)
  include_directories(${TAU_INCLUDE_PATH})
  link_directories(${TAU_LIBRARY_PATH})
  add_executable (tau2paje src/tau2paje.c src/aky_paje.c)
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
