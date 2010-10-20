cmake_minimum_required (VERSION 2.6)
project (Rastro)

set(RASTRO_SRC
  src/list.c
  src/rastro_read.c 
  src/rastro_write.c
  src/rastro_write_functions.c
)

set(RASTRO_HEADERS
  include/rastro.h
)

set(RASTRO_GENERATE_SRC
  src/generate/rastro_generate.c
)

set(RASTRO_TIMESYNC_SRC
  src/timesync/rastro_timesync.c
  src/timesync/rastro_timesync_f.c
)

include_directories(include/ src/)

add_library(Rastro SHARED ${RASTRO_SRC})
add_executable(rastro_generate ${RASTRO_GENERATE_SRC})
target_link_libraries(rastro_generate Rastro)
add_executable(rastro_timesync ${RASTRO_TIMESYNC_SRC})
target_link_libraries(rastro_timesync Rastro)

install(PROGRAMS rastro_generate rastro_timesync
        DESTINATION bin)
install(TARGETS Rastro
        DESTINATION lib)
install(FILES ${RASTRO_HEADERS}
        DESTINATION include)

add_subdirectory (examples)
add_subdirectory (scripts)
