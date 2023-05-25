CPMAddPackage("gh:catchorg/Catch2@3.3.2")
include("${Catch2_SOURCE_DIR}/extras/Catch.cmake")

CPMAddPackage("gh:rollbear/trompeloeil@44")

add_executable(
	Simple-Utility-Tests
	"Config.cpp"
	"CRTPBase.cpp"
	"bind_back.cpp"
	"helper.cpp"
	"math.cpp"
	"unique_handle.cpp"
	"Tuple.cpp"
	"TypeList.cpp"
	"TypeTraits.cpp"
)

add_subdirectory("nullables")
add_subdirectory("concepts")
add_subdirectory("functional")

target_link_libraries(
	Simple-Utility-Tests
	PRIVATE
	Simple::Utility
	Catch2::Catch2WithMain
	trompeloeil::trompeloeil
)

catch_discover_tests(Simple-Utility-Tests)
