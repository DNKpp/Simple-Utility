cmake_minimum_required(VERSION 3.15)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(get_cpm)
include(GetVersion)

get_version("include/Simple-Utility/Config.hpp")

project(
	Simple-Utility
	VERSION ${VERSION_STRING}
	LANGUAGES CXX
)

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

if (CMAKE_SOURCE_DIR STREQUAL Simple-Utility_SOURCE_DIR)
	set(IS_ROOT_PROJECT TRUE)
else()
	set(IS_ROOT_PROJECT FALSE)
endif()

OPTION(SIMPLE_UTILITY_BUILD_TESTS "Determines whether tests will be built." ${IS_ROOT_PROJECT})
if (SIMPLE_UTILITY_BUILD_TESTS)
	include(CTest)
	add_subdirectory("tests")
endif()

OPTION(SIMPLE_UTILITY_BUILD_BENCHMARKS "Determines whether benchmarks will be built." ${IS_ROOT_PROJECT})
if (SIMPLE_UTILITY_BUILD_BENCHMARKS)
	include(CTest)
	add_subdirectory("benchmarks")
endif()

option(SIMPLE_UTILITY_GEN_DOCS_ENABLED "Enables the GenerateDocs target." OFF)
if (SIMPLE_UTILITY_GEN_DOCS_ENABLED)
	add_subdirectory("docs")
endif()
