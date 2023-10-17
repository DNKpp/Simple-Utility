cmake_minimum_required(VERSION 3.15)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(get_cpm)
include(GetVersion)
include(GNUInstallDirs)

get_version("include/Simple-Utility/Config.hpp")

project(
	Simple-Utility
	VERSION ${VERSION_STRING}
	LANGUAGES CXX
)

set(TARGET_NAME "Simple-Utility")
set(PACKAGE_NAME ${TARGET_NAME})

add_library(${TARGET_NAME} INTERFACE)
add_library(${PACKAGE_NAME}::${TARGET_NAME} ALIAS ${TARGET_NAME})

target_include_directories(
	${TARGET_NAME}
	INTERFACE
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_compile_features(
	${TARGET_NAME}
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

# Installation
include(CMakePackageConfigHelpers)
set(LIB_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/${PACKAGE_NAME}")

install(
    TARGETS ${TARGET_NAME}
    EXPORT Simple-UtilityTargets
	ARCHIVE
	PUBLIC_HEADER
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PACKAGE_NAME}"
)

install(DIRECTORY "include/" TYPE INCLUDE)

configure_package_config_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/in/${PACKAGE_NAME}Config.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}Config.cmake"
	INSTALL_DESTINATION "${LIB_INSTALL_DIR}/cmake"
	PATH_VARS LIB_INSTALL_DIR
)

write_basic_package_version_file(
    ${PACKAGE_NAME}ConfigVersion.cmake
    COMPATIBILITY ExactVersion
	ARCH_INDEPENDENT
)

install(
	FILES	"${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}Config.cmake"
			"${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}ConfigVersion.cmake"
	DESTINATION "${LIB_INSTALL_DIR}/cmake"
)

install(
	EXPORT Simple-UtilityTargets
	DESTINATION "${LIB_INSTALL_DIR}/cmake"
	NAMESPACE ${PACKAGE_NAME}::
)
