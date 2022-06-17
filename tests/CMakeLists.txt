cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include(CTest)

CPMAddPackage("gh:catchorg/Catch2@3.0.1")
include("${Catch2_SOURCE_DIR}/extras/Catch.cmake")

add_executable(
	${PROJECT_NAME}-Tests
	"concepts/access_operators.cpp"
	"concepts/shift_operators.cpp"
	"concepts/logical_arithmetic_operators.cpp"
	"unique_handle.cpp"
	"concepts/stl_extensions.cpp"
	"nullables.cpp"
	"functional.cpp"
)

target_link_libraries(
	${PROJECT_NAME}-Tests
	PRIVATE
	${PROJECT_NAME}
	Catch2::Catch2WithMain
)

catch_discover_tests(${PROJECT_NAME}-Tests)
