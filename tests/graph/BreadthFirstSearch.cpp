//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/graph/BreadthFirstSearch.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include "Defines.hpp"

namespace
{
	inline const std::vector<
		std::tuple<
			std::vector<sg::decorator::DepthNode<sg::decorator::PredecessorNode<sg::CommonBasicNode<std::string>>>>,
			std::string
		>> testResults{
		{
			{{{{"42"}, std::nullopt}, 0}},
			"42"
		},
		{
			{{{{"3"}, std::nullopt}, 0}, {{{"5"}, "3"}, 1}, {{{"6"}, "3"}, 1}, {{{"2"}, "6"}, 2}},
			"3"
		},
		{
			{{{{"6"}, std::nullopt}, 0}, {{{"2"}, "6"}, 1}},
			"6"
		},
		{
			{{{{"1"}, std::nullopt}, 0}, {{{"2"}, "1"}, 1}, {{{"3"}, "1"}, 1}, {{{"6"}, "2"}, 2}, {{{"5"}, "3"}, 2}},
			"1"
		},
		{
			{{{{"8"}, std::nullopt}, 0}, {{{"7"}, "8"}, 1}, {{{"9"}, "8"}, 1}, {{{"4"}, "7"}, 2}},
			"8"
		}
	};
}

TEMPLATE_TEST_CASE(
	"bfs::Stream visits all reachable vertices.",
	"[graph][graph::bfs]",
	BasicGraphStub,
	WeightedGraphStub
)
{
	using Node = sg::CommonBasicNode<std::string>;
	const auto& [expected, origin] = GENERATE(from_range(slice_test_expectations(testResults, toCommonBasicNode)));

	sg::dfs::Stream<TestType, Node> stream{origin, std::tuple{TestType{}}, std::tuple{}, std::tuple{}, std::tuple{}};
	STATIC_CHECK(std::ranges::input_range<decltype(stream)>);

	REQUIRE_THAT(buffer_nodes(stream), Catch::Matchers::RangeEquals(expected));
}

TEMPLATE_TEST_CASE(
	"bfs::Stream can be used with depth decorated nodes.",
	"[graph][graph::bfs]",
	BasicGraphStub,
	WeightedGraphStub
)
{
	using Node = sg::decorator::DepthNode<sg::CommonBasicNode<std::string>>;
	const auto& [expected, origin] = GENERATE(from_range(slice_test_expectations(testResults, toDepthBasicNode)));

	sg::dfs::Stream<TestType, Node> stream{origin, std::tuple{TestType{}}, std::tuple{}, std::tuple{}, std::tuple{}};
	STATIC_CHECK(std::ranges::input_range<decltype(stream)>);

	REQUIRE_THAT(buffer_nodes(stream), Catch::Matchers::RangeEquals(expected));
}

TEMPLATE_TEST_CASE(
	"bfs::Stream can be used with predecessor decorated nodes.",
	"[graph][graph::bfs]",
	BasicGraphStub,
	WeightedGraphStub
)
{
	using Node = sg::decorator::PredecessorNode<sg::CommonBasicNode<std::string>>;
	const auto& [expected, origin] = GENERATE(from_range(slice_test_expectations(testResults, toPredecessorBasicNode)));

	sg::dfs::Stream<TestType, Node> stream{origin, std::tuple{TestType{}}, std::tuple{}, std::tuple{}, std::tuple{}};
	STATIC_CHECK(std::ranges::input_range<decltype(stream)>);

	REQUIRE_THAT(buffer_nodes(stream), Catch::Matchers::RangeEquals(expected));
}

TEMPLATE_TEST_CASE(
	"bfs::Stream can be used with arbitrary decorated nodes.",
	"[graph][graph::bfs]",
	BasicGraphStub,
	WeightedGraphStub
)
{
	using Node = sg::decorator::DepthNode<sg::decorator::PredecessorNode<sg::CommonBasicNode<std::string>>>;
	const auto& [expected, origin] = GENERATE(from_range(testResults));

	sg::dfs::Stream<TestType, Node> stream{origin, std::tuple{TestType{}}, std::tuple{}, std::tuple{}, std::tuple{}};
	STATIC_CHECK(std::ranges::input_range<decltype(stream)>);

	REQUIRE_THAT(buffer_nodes(stream), Catch::Matchers::RangeEquals(expected));
}
