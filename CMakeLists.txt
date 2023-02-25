cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(
	Simple-Utility
	VERSION 2.0.1
	LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(get_cpm)

add_library(Simple-Utility INTERFACE)
add_library(Simple::Utility ALIAS Simple-Utility)
target_include_directories(
	Simple-Utility
	INTERFACE
	"include"
)

target_compile_features(
	Simple-Utility
	INTERFACE
	cxx_std_20
)

OPTION(SIMPLE_UTILITY_BUILD_TESTS "Determines whether tests will be built." OFF)
if (SIMPLE_UTILITY_BUILD_TESTS OR CMAKE_SOURCE_DIR STREQUAL Simple-Utility_SOURCE_DIR)
	include(CTest)
	add_subdirectory("tests")
endif()

option(SIMPLE_UTILITY_CONFIGURE_DOXYGEN "Enables configure_file on the Doxyfile.in" OFF)
if (SIMPLE_UTILITY_CONFIGURE_DOXYGEN)
	add_subdirectory("docs")
endif()
