function(get_version FILE_PATH)

	set(SUB_REGEX "[ \t]*=[ \t]*([0-9]+)")

	file(READ ${FILE_PATH} FILE_CONTENT)
	string(REGEX MATCH "major${SUB_REGEX}" _ ${FILE_CONTENT})
	set(VERSION_MAJOR ${CMAKE_MATCH_1})

	string(REGEX MATCH "minor${SUB_REGEX}" _ ${FILE_CONTENT})
	set(VERSION_MINOR ${CMAKE_MATCH_1})

	string(REGEX MATCH "patch${SUB_REGEX}" _ ${FILE_CONTENT})
	set(VERSION_PATCH ${CMAKE_MATCH_1})

	if (NOT VERSION_MAJOR AND NOT VERSION_MAJOR EQUAL 0)
		message(AUTHOR_WARNING "get_version failed - Can not retrieve major value.")
		return()
	endif()

	if (NOT VERSION_MINOR AND NOT VERSION_MINOR EQUAL 0)
		message(AUTHOR_WARNING "get_version failed - Can not retrieve minor value.")
		return()
	endif()

	if (NOT VERSION_PATCH AND NOT VERSION_PATCH EQUAL 0)
		message(AUTHOR_WARNING "get_version failed - Can not retrieve patch value.")
		return()
	endif()

	set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

	message(VERBOSE "Simple-Utility extracted version ${VERSION_STRING}")

	return(PROPAGATE VERSION_STRING VERSION_MAJOR VERSION_MINOR VERSION_PATCH)
endfunction()
