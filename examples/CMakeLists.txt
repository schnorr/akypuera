cmake_minimum_required(VERSION 2.8)

link_directories (../)

find_package(MPI REQUIRED)

SET(CMAKE_C_COMPILER mpicc)
SET(CMAKE_CXX_COMPILER mpicxx) 

add_executable(ring ring.c)
add_executable(prog prog.c)
add_executable(scatter scatter.c)
target_link_libraries(ring aky)
target_link_libraries(prog aky)
target_link_libraries(scatter aky)
