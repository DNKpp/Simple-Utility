CPMAddPackage("gh:catchorg/Catch2@3.3.1")
include("${Catch2_SOURCE_DIR}/extras/Catch.cmake")

add_executable(
	Simple-Utility-Tests
	"bind_back.cpp"
	"helper.cpp"
	"math.cpp"
	"unique_handle.cpp"
	"tuple_utility.cpp"
)

add_subdirectory("nullables")
add_subdirectory("concepts")
add_subdirectory("functional")

target_link_libraries(
	Simple-Utility-Tests
	PRIVATE
	Simple::Utility
	Catch2::Catch2WithMain
)

catch_discover_tests(Simple-Utility-Tests)
