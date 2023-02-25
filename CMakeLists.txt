cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(
	Simple-Utility
	VERSION 2.0.1
	LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")
include("get_cpm")

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(
	${PROJECT_NAME}
	INTERFACE
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_features(
	${PROJECT_NAME}
	INTERFACE
	cxx_std_20
)

OPTION(${PROJECT_NAME}_BUILD_TESTS "Determines wether tests will be build or not." OFF)
if (${PROJECT_NAME}_BUILD_TESTS)
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tests")
endif()

option(${PROJECT_NAME}_CONFIGURE_DOXYGEN "Enables configure_file on the Doxyfile.in" OFF)
if (${PROJECT_NAME}_CONFIGURE_DOXYGEN)
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/docs")
endif()
