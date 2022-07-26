set(LIB_VERSION ${CMAKE_PROJECT_VERSION})
configure_file(Doxyfile.in "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile" @ONLY)
message(NOTICE "Configured Doxyfile")
