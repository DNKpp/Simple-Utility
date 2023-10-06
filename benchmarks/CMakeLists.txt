CPMAddPackage("gh:catchorg/Catch2@3.4.0")
include("${Catch2_SOURCE_DIR}/extras/Catch.cmake")

CPMAddPackage("gh:kokkos/mdspan#mdspan-0.6.0")
CPMAddPackage(
	name boost
	URL "https://github.com/boostorg/boost/releases/download/boost-1.83.0/boost-1.83.0.zip"
)

add_executable(
	Simple-Utility-Benchmarks
)

add_subdirectory("graph")

target_link_libraries(
	Simple-Utility-Benchmarks
	PRIVATE
	Simple::Utility
	Catch2::Catch2WithMain
	std::mdspan
	Boost::graph
)

catch_discover_tests(Simple-Utility-Benchmarks)
