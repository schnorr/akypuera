set(RASTRO_SRC
  list.c
  rastro_read.c 
  rastro_write.c
  rastro_write_functions.c
)

set(RASTRO_HEADERS
  rastro_public.h
)

set(RASTRO_GENERATE_SRC
  rastro_generate.c
)

set(RASTRO_TIMESYNC_SRC
  rastro_timesync.c
  rastro_timesync_f.c
)

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
