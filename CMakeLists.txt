cmake_minimum_required (VERSION 2.6)
project (aky)

set(CMAKE_BUILD_TYPE Debug)

find_file(HAVE_RASTRO_PUBLIC_H rastro_public.h)
add_subdirectory(libRastro)
add_subdirectory(examples)

find_package(MPI REQUIRED)
include(cmake/FindTAU.cmake)

include_directories(${MPI_INCLUDE_PATH} include/)
add_library (aky SHARED src/aky.c)
target_link_libraries(aky Rastro)

add_executable(aky_converter src/aky_converter.c src/aky_paje.c)
target_link_libraries(aky_converter Rastro)

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
