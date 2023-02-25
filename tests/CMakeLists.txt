cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include(CTest)

CPMAddPackage("gh:catchorg/Catch2@3.3.1")
include("${Catch2_SOURCE_DIR}/extras/Catch.cmake")

add_executable(
	${PROJECT_NAME}-Tests
	"bind_back.cpp"
	"math.cpp"
	"unique_handle.cpp"
	"tuple_utility.cpp"
)

add_subdirectory("nullables")
add_subdirectory("concepts")
add_subdirectory("functional")

target_link_libraries(
	${PROJECT_NAME}-Tests
	PRIVATE
	${PROJECT_NAME}
	Catch2::Catch2WithMain
)

catch_discover_tests(${PROJECT_NAME}-Tests)
