CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(rastro)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckSymbolExists)
INCLUDE(CheckFunctionExists)

# the cmake options
OPTION(THREADED "Thread support" OFF)
OPTION(RASTRO_GENERATE "Compile rastro_generate" ON)
OPTION(RASTRO_TIMESYNC "Compile rastro_timesync" ON)
OPTION(RASTRO_READ     "Compile rastro_read" ON)

IF(THREADED)
  SET(LIBRASTRO_THREADED ON)
ENDIF(THREADED)

CHECK_FUNCTION_EXISTS(gettimeofday HAVE_GETTIMEOFDAY)
CHECK_LIBRARY_EXISTS(rt clock_gettime "" HAVE_CLOCKGETTIME)
IF(NOT HAVE_CLOCKGETTIME AND NOT HAVE_GETTIMEOFDAY)
  MESSAGE(FATAL_ERROR "Neither of clock_gettime or gettimeofday are available, while librastro needs at least one of these to work.")
ENDIF(NOT HAVE_CLOCKGETTIME AND NOT HAVE_GETTIMEOFDAY)

CONFIGURE_FILE("${PROJECT_SOURCE_DIR}/include/rastro_config.h.in"
               "${CMAKE_BINARY_DIR}/include/rastro_config.h" @ONLY IMMEDIATE)

SET(RASTRO_SRC
  src/rst_read.c 
  src/rst_write.c
  src/rst_generate.c
)

SET(RASTRO_HEADERS
  include/rastro.h
  ${CMAKE_BINARY_DIR}/include/rastro_config.h
)

SET(RASTRO_GENERATE_SRC
  src/generate/rst_generate.c
)

SET(RASTRO_READ_SRC
  src/read/rst_read.c
)

SET(RASTRO_TIMESYNC_SRC
  src/timesync/rst_timesync.c
)

INCLUDE_DIRECTORIES(${rastro_SOURCE_DIR}/include/)
INCLUDE_DIRECTORIES(${rastro_SOURCE_DIR}/src/)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/include/)

ADD_LIBRARY(rastro SHARED ${RASTRO_SRC})
IF(THREADED)
  TARGET_LINK_LIBRARIES(rastro pthread)
ENDIF(THREADED)
IF(HAVE_CLOCKGETTIME)
  TARGET_LINK_LIBRARIES(rastro rt)
ENDIF(HAVE_CLOCKGETTIME)
INSTALL(TARGETS rastro DESTINATION lib)
INSTALL(FILES ${RASTRO_HEADERS} DESTINATION include)

IF(RASTRO_GENERATE)
  ADD_EXECUTABLE(rastro_generate ${RASTRO_GENERATE_SRC})
  TARGET_LINK_LIBRARIES(rastro_generate rastro)
  INSTALL(PROGRAMS ${rastro_BINARY_DIR}/rastro_generate DESTINATION bin)
ENDIF(RASTRO_GENERATE)

IF(RASTRO_TIMESYNC)
  ADD_EXECUTABLE(rastro_timesync ${RASTRO_TIMESYNC_SRC})
  TARGET_LINK_LIBRARIES(rastro_timesync rastro)
  INSTALL(PROGRAMS ${rastro_BINARY_DIR}/rastro_timesync DESTINATION bin)
ENDIF(RASTRO_TIMESYNC)

IF(RASTRO_READ)
  ADD_EXECUTABLE(rastro_read ${RASTRO_READ_SRC})
  TARGET_LINK_LIBRARIES(rastro_read rastro)
  INSTALL(PROGRAMS ${rastro_BINARY_DIR}/rastro_read DESTINATION bin)
ENDIF(RASTRO_READ)

ADD_SUBDIRECTORY(examples)