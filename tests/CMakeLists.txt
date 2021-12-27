cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include(CTest)

CPMAddPackage("gh:catchorg/Catch2#v2.x")

add_executable(
	${PROJECT_NAME}-Tests
	"main.cpp"
	"concepts/shift_operators.cpp"
	"concepts/logical_operators.cpp"
)

target_link_libraries(
	${PROJECT_NAME}-Tests
	PRIVATE
	${PROJECT_NAME}
	Catch2::Catch2
)

enable_testing()
add_test(${PROJECT_NAME}-TestSuite ${PROJECT_NAME}-Tests)
