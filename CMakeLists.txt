cmake_minimum_required (VERSION 2.6)
project (Cuia)

set(CMAKE_BUILD_TYPE Debug)

find_file(HAVE_RASTRO_PUBLIC_H rastro_public.h)
add_subdirectory(libRastro)
add_subdirectory(examples)

find_package(MPI REQUIRED)

include_directories(${MPI_INCLUDE_PATH} include/)
add_library (Cuia SHARED src/cuia.c)
target_link_libraries(Cuia Rastro)

add_executable(cuia_converter src/cuia_converter.c src/cuia_paje.c)
target_link_libraries(cuia_converter Rastro)

install(TARGETS Cuia
        DESTINATION lib)
install(PROGRAMS cuia_converter
        DESTINATION bin)

