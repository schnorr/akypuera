CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(rastro)

OPTION(THREADED
  "Thread support"
  OFF)

IF(THREADED)
  SET(LIBRASTRO_THREADED ON)
ENDIF(THREADED)

CONFIGURE_FILE("${PROJECT_SOURCE_DIR}/include/rastro_config.h.in"
               "${CMAKE_BINARY_DIR}/include/rastro_config.h" @ONLY IMMEDIATE)

SET(RASTRO_SRC
  src/rastro_list.c
  src/rastro_read.c 
  src/rastro_write.c
)

SET(RASTRO_HEADERS
  include/rastro.h
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

INCLUDE_DIRECTORIES(${rastro_SOURCE_DIR}/include/)
INCLUDE_DIRECTORIES(${rastro_SOURCE_DIR}/src/)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/include/)

ADD_LIBRARY(rastro SHARED ${RASTRO_SRC})
IF(THREADED)
  TARGET_LINK_LIBRARIES(rastro pthread)
ENDIF(THREADED)
ADD_EXECUTABLE(rastro_generate ${RASTRO_GENERATE_SRC})
TARGET_LINK_LIBRARIES(rastro_generate rastro)
ADD_EXECUTABLE(rastro_timesync ${RASTRO_TIMESYNC_SRC})
TARGET_LINK_LIBRARIES(rastro_timesync rastro)
ADD_EXECUTABLE(rastro_read ${RASTRO_READ_SRC})
TARGET_LINK_LIBRARIES(rastro_read rastro)

INSTALL(PROGRAMS ${rastro_BINARY_DIR}/rastro_generate
                 ${rastro_BINARY_DIR}/rastro_timesync
                 ${rastro_BINARY_DIR}/rastro_read
        DESTINATION bin)
INSTALL(TARGETS rastro
        DESTINATION lib)
INSTALL(FILES ${RASTRO_HEADERS}
        DESTINATION include)

ADD_SUBDIRECTORY(examples)

MARK_AS_ADVANCED(CMAKE_BUILD_TYPE)