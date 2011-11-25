CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(Rastro)

OPTION(THREADED
  "Thread support"
  OFF)

CONFIGURE_FILE("${PROJECT_SOURCE_DIR}/include/rastro_config.h.in"
               "${CMAKE_BINARY_DIR}/include/rastro_config.h")

SET(RASTRO_SRC
  src/list.c
  src/rastro_read.c 
  src/rastro_write.c
  src/rastro_write_functions.c
)

SET(RASTRO_HEADERS
  include/rastro.h
  include/rastro_config.h
)

SET(RASTRO_GENERATE_SRC
  src/generate/rastro_generate.c
)

SET(RASTRO_READ_SRC
  src/read/rastro_read.c
)

SET(RASTRO_TIMESYNC_SRC
  src/timesync/rastro_timesync.c
  src/timesync/rastro_timesync_f.c
)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include/)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/libRastro/include/)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/libRastro/src/)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/include/)

ADD_LIBRARY(Rastro SHARED ${RASTRO_SRC})
IF(LIBRASTRO_THREADED)
  TARGET_LINK_LIBRARIES(Rastro pthread)
ENDIF(LIBRASTRO_THREADED)
ADD_EXECUTABLE(rastro_generate ${RASTRO_GENERATE_SRC})
TARGET_LINK_LIBRARIES(rastro_generate Rastro)
ADD_EXECUTABLE(rastro_timesync ${RASTRO_TIMESYNC_SRC})
TARGET_LINK_LIBRARIES(rastro_timesync Rastro)
ADD_EXECUTABLE(rastro_read ${RASTRO_READ_SRC})
TARGET_LINK_LIBRARIES(rastro_read Rastro)

INSTALL(PROGRAMS rastro_generate rastro_timesync rastro_read
        DESTINATION bin)
INSTALL(TARGETS Rastro
        DESTINATION lib)
INSTALL(FILES ${RASTRO_HEADERS}
        DESTINATION include)

ADD_SUBDIRECTORY(examples)

MARK_AS_ADVANCED(CMAKE_BUILD_TYPE)