if(POLICY CMP0015)
  cmake_policy(SET CMP0015 NEW)
endif(POLICY CMP0015)
include_directories (../src/) 
link_directories (../src/)

add_executable(write rastro_write_example.c saida.c)
target_link_libraries(write rastro)

add_executable(write2 write2.c saida.c)
target_link_libraries(write2 rastro)