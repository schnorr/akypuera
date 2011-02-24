if(POLICY CMP0015)
  cmake_policy(SET CMP0015 NEW)
endif(POLICY CMP0015)
include_directories (../src/) 
link_directories (../src/)

add_executable(read rastro_read_example.c)
target_link_libraries(read Rastro)

add_executable(write rastro_write_example.c saida.c)
target_link_libraries(write Rastro)
