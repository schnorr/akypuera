# pkgconfig CmakeLists.txt

configure_file (${poti_SOURCE_DIR}/pkgconfig/poti.pc.cmake ${poti_BINARY_DIR}/pkgconfig/poti.pc @ONLY)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/poti.pc DESTINATION lib/pkgconfig)
