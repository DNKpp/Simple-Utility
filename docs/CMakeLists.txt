option(${PROJECT_NAME}_CONFIGURE_DOXYGEN "Enables configure_file on the Doxyfile.in" OFF)

if (${PROJECT_NAME}_CONFIGURE_DOXYGEN)

	set(LIB_VERSION ${CMAKE_PROJECT_VERSION})
	configure_file(Doxyfile.in "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile" @ONLY)
	message(NOTICE "Configured Doxyfile")

endif()
