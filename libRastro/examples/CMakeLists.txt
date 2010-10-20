include_directories (../src/) 
link_directories (../src/)

add_executable(read rastro_read_example.c)
target_link_libraries(read Rastro)

add_executable(write rastro_write_example.c saida.c)
target_link_libraries(write Rastro)
