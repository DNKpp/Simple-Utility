
block(SCOPE_FOR VARIABLES)
	message(TRACE "Begin generating docs")

	find_package(Doxygen REQUIRED COMPONENTS dot)
	if (NOT DOXYGEN_FOUND)
		message(FATAL_ERROR "Doxygen not found.")
	endif()

	get_target_property(DOXY_DOT_PATH Doxygen::dot LOCATION)
	message(NOTICE "Using dot at: ${DOXY_DOT_PATH}")

	CPMAddPackage("gh:jothepro/doxygen-awesome-css@2.2.0")

	set(DOXY_PROJECT_VERSION ${PROJECT_VERSION})
	set(DOXY_INPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../include")
	set(DOXY_EXAMPLE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../tests")
	set(DOXY_README_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../README.md")
	set(DOXY_OUTPUT_DIR ".")
	set(DOXY_HTML_EXTRA_STYLESHEET "${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome.css")
	
	configure_file(Doxyfile.in Doxyfile @ONLY)

	add_custom_target(
		GenerateDocs
		COMMAND "$<TARGET_FILE:Doxygen::doxygen>" Doxyfile
		COMMENT "Generate Documentation"
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
		COMMAND_EXPAND_LISTS
		VERBATIM
	)
	
	message(TRACE "Finished generating docs")
endblock()
