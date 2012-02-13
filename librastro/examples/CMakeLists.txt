if(POLICY CMP0015)
  cmake_policy(SET CMP0015 NEW)
endif(POLICY CMP0015)
include_directories (../src/) 
link_directories (../src/)

add_executable(write1 write1.c auto-generated.c)
target_link_libraries(write1 rastro)

add_executable(write2 write2.c auto-generated.c)
target_link_libraries(write2 rastro)