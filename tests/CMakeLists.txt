cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include(CTest)

CPMAddPackage("gh:catchorg/Catch2@2.13.7")

add_executable(
	${PROJECT_NAME}-Tests
	"main.cpp"
	"concepts/access_operators.cpp"
	"concepts/shift_operators.cpp"
	"concepts/logical_arithmetic_operators.cpp"
	"unique_handle.cpp"
	"concepts/stl_counterparts.cpp"
	"unique_handle_algorithms.cpp"
)

target_link_libraries(
	${PROJECT_NAME}-Tests
	PRIVATE
	${PROJECT_NAME}
	Catch2::Catch2
)

add_test(${PROJECT_NAME}-TestSuite ${PROJECT_NAME}-Tests)
