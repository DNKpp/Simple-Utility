cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(simple_utility)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(
	${PROJECT_NAME}
	INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(
	${PROJECT_NAME}
	INTERFACE
	cxx_std_20
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	include(CTest)

	add_executable(
		test_simple_utility
		${CMAKE_CURRENT_SOURCE_DIR}/test/main.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/test/OperatorTest.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/test/BitmaskTest.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/test/Vector2dTest.cpp
	)

	target_link_libraries(
		test_simple_utility
		PRIVATE
		simple_utility
	)

	enable_testing()
	add_test(
		NAME Simple-Utility_TestSuite
		COMMAND test_simple_utility
	)
endif()
