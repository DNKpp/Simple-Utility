function(get_version FILE_PATH)

	set(REGEXS_PREFIX "#define SL_UTILITY_VERSION_")
	set(REGEXS_SUFFIX "[ \t]*([0-9]+)")

	file(READ ${FILE_PATH} FILE_CONTENT)
	string(REGEX MATCH "${REGEXS_PREFIX}MAJOR${REGEXS_SUFFIX}" _ ${FILE_CONTENT})
	set(VERSION_MAJOR ${CMAKE_MATCH_1})

	string(REGEX MATCH "${REGEXS_PREFIX}MINOR${REGEXS_SUFFIX}" _ ${FILE_CONTENT})
	set(VERSION_MINOR ${CMAKE_MATCH_1})

	string(REGEX MATCH "${REGEXS_PREFIX}PATCH${REGEXS_SUFFIX}" _ ${FILE_CONTENT})
	set(VERSION_PATCH ${CMAKE_MATCH_1})

	if (NOT VERSION_MAJOR AND NOT VERSION_MAJOR EQUAL 0)
		message(AUTHOR_WARNING "get_version failed - Unable to retrieve major value.")
		return()
	endif()

	if (NOT VERSION_MINOR AND NOT VERSION_MINOR EQUAL 0)
		message(AUTHOR_WARNING "get_version failed - Unable to retrieve minor value.")
		return()
	endif()

	if (NOT VERSION_PATCH AND NOT VERSION_PATCH EQUAL 0)
		message(AUTHOR_WARNING "get_version failed - Unable to retrieve patch value.")
		return()
	endif()

	set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}" PARENT_SCOPE)
endfunction()
