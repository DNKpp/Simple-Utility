cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include(CTest)

CPMGetPackage(Catch2)
include("${Catch2_SOURCE_DIR}/extras/Catch.cmake")

add_executable(
	${PROJECT_NAME}-Tests
	"bind_back.cpp"
	"unique_handle.cpp"
	"tuple_utility.cpp"
	"concepts/access_operators.cpp"
	"concepts/shift_operators.cpp"
	"concepts/logical_arithmetic_operators.cpp"
	"concepts/utility.cpp"
	"concepts/stl_extensions.cpp"
	"functional/base.cpp"
	"functional/transform.cpp"
	"functional/predicate.cpp"
	"functional/tuple.cpp"
)

add_subdirectory("nullables")

target_link_libraries(
	${PROJECT_NAME}-Tests
	PRIVATE
	${PROJECT_NAME}
	Catch2::Catch2WithMain
)

catch_discover_tests(${PROJECT_NAME}-Tests)
