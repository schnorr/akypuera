cmake_minimum_required (VERSION 2.6)
project (Rastro)

option(LIBRASTRO_THREADED "Enable if traced applications execute with threads" off)

configure_file("${PROJECT_SOURCE_DIR}/include/rastro_config.h.in"
               "${CMAKE_BINARY_DIR}/include/rastro_config.h")

set(RASTRO_SRC
  src/list.c
  src/rastro_read.c 
  src/rastro_write.c
  src/rastro_write_functions.c
)

set(RASTRO_HEADERS
  include/rastro.h
  include/rastro_config.h
)

set(RASTRO_GENERATE_SRC
  src/generate/rastro_generate.c
)

set(RASTRO_READ_SRC
  src/read/rastro_read.c
)

set(RASTRO_TIMESYNC_SRC
  src/timesync/rastro_timesync.c
  src/timesync/rastro_timesync_f.c
)

include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(${PROJECT_SOURCE_DIR}/src/)
include_directories(${PROJECT_SOURCE_DIR}/libRastro/include/)
include_directories(${PROJECT_SOURCE_DIR}/libRastro/src/)
include_directories(${CMAKE_BINARY_DIR}/include/)

add_library(Rastro SHARED ${RASTRO_SRC})
if(LIBRASTRO_THREADED)
  target_link_libraries(Rastro pthread)
endif(LIBRASTRO_THREADED)
add_executable(rastro_generate ${RASTRO_GENERATE_SRC})
target_link_libraries(rastro_generate Rastro)
add_executable(rastro_timesync ${RASTRO_TIMESYNC_SRC})
target_link_libraries(rastro_timesync Rastro)
add_executable(rastro_read ${RASTRO_READ_SRC})
target_link_libraries(rastro_read Rastro)


install(PROGRAMS rastro_generate rastro_timesync rastro_read
        DESTINATION bin)
install(TARGETS Rastro
        DESTINATION lib)
install(FILES ${RASTRO_HEADERS}
        DESTINATION include)

add_subdirectory (examples)
