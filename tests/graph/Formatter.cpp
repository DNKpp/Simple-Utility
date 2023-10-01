//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/graph/Formatter.hpp"

#include <catch2/catch_test_macros.hpp>

#include "Defines.hpp"

#ifdef SL_UTILITY_HAS_STD_FORMAT

TEST_CASE("node types can be formatted.", "[graph][graph::node][graph::format]")
{
	using TestType = GenericBasicNode<std::string>;

	REQUIRE("{vertex: Hello, World!}" == std::format("{}", TestType{.vertex = "Hello, World!"}));
}

TEST_CASE("ranked_node types can be formatted.", "[graph][graph::node][graph::format]")
{
	using TestType = GenericRankedNode<std::string, int>;

	REQUIRE("{vertex: Hello, World!, rank: 42}" == std::format("{}", TestType{.vertex = "Hello, World!", .rank = 42}));
}

TEST_CASE("predecessor decorated ranked_node types can be formatted.", "[graph][graph::node][graph::format]")
{
	using TestType = sg::PredecessorNodeDecorator<GenericRankedNode<std::string, int>>;

	REQUIRE("{vertex: 42, rank: 1337, predecessor: 41}" == std::format("{}", TestType{{.vertex = "42", .rank = 1337}, "41"}));
	REQUIRE(
		"{vertex: 42, rank: 1337, predecessor: null}" == std::format("{}", TestType{{.vertex = "42", .rank = 1337}, std::nullopt}));
}

TEST_CASE("depth decorated ranked_node types can be formatted.", "[graph][graph::node][graph::format]")
{
	using TestType = sg::DepthNodeDecorator<GenericRankedNode<std::string, int>>;

	REQUIRE("{vertex: -42, rank: 1337, depth: 42}" == std::format("{}", TestType{{.vertex = "-42", .rank = 1337}, 42}));
}

TEST_CASE("edge types can be formatted.", "[graph][graph::edge][graph::format]")
{
	using TestType = GenericBasicEdge<std::string>;

	REQUIRE("{destination: Hello, World!}" == std::format("{}", TestType{.destination = "Hello, World!"}));
}

TEST_CASE("weighted_edge types can be formatted.", "[graph][graph::edge][graph::format]")
{
	using TestType = GenericWeightedEdge<std::string, int>;

	REQUIRE("{destination: Hello, World!, weight: 42}" == std::format("{}", TestType{.destination = "Hello, World!", .weight = 42}));
}

#endif
