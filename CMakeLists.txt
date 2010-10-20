cmake_minimum_required (VERSION 2.6)
project (Cuia)

set(CMAKE_BUILD_TYPE Debug)

find_file(HAVE_RASTRO_PUBLIC_H rastro_public.h)
add_subdirectory(libRastro)

find_package(MPI REQUIRED)

include_directories(${MPI_INCLUDE_PATH})
add_library (Cuia SHARED cuia.c cuia.h)
target_link_libraries(Cuia Rastro)

install(TARGETS Cuia
        DESTINATION lib)

