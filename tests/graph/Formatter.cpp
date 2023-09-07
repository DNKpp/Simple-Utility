#include "Simple-Utility/graph/Formatter.hpp"

#include <catch2/catch_test_macros.hpp>

#include "Defines.hpp"

#ifdef SL_UTILITY_HAS_STD_FORMAT

TEST_CASE("node types can be formatted.", "[graph][graph::node]")
{
	using TestType = GenericBasicNode<std::string>;

	REQUIRE("{vertex: Hello, World!}" == std::format("{}", TestType{.vertex = "Hello, World!"}));
}

TEST_CASE("ranked_node types can be formatted.", "[graph][graph::node]")
{
	using TestType = GenericRankedNode<std::string, int>;

	REQUIRE("{vertex: Hello, World!, rank: 42}" == std::format("{}", TestType{.vertex = "Hello, World!", .rank = 42}));
}

TEST_CASE("decorated ranked_node types can be formatted.", "[graph][graph::node]")
{
	using TestType = sg::PredecessorNodeDecorator<GenericRankedNode<std::string, int>>;

	REQUIRE("{vertex: 42, rank: 1337, predecessor: 41}" == std::format("{}", TestType{{.vertex = "42", .rank = 1337}, "41"}));
	REQUIRE("{vertex: 42, rank: 1337, predecessor: null}" == std::format("{}", TestType{{.vertex = "42", .rank = 1337}, std::nullopt}));
}

TEST_CASE("edge types can be formatted.", "[graph][graph::edge]")
{
	using TestType = GenericBasicEdge<std::string>;

	REQUIRE("{destination: Hello, World!}" == std::format("{}", TestType{.destination = "Hello, World!"}));
}

TEST_CASE("weighted_edge types can be formatted.", "[graph][graph::edge]")
{
	using TestType = GenericWeightedEdge<std::string, int>;

	REQUIRE("{destination: Hello, World!, weight: 42}" == std::format("{}", TestType{.destination = "Hello, World!", .weight = 42}));
}

#endif