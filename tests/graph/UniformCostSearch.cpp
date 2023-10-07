//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/graph/UniformCostSearch.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include "Defines.hpp"

namespace
{
	inline const std::vector<
		std::tuple<
			std::vector<sg::decorator::DepthNode<sg::decorator::PredecessorNode<sg::CommonRankedNode<std::string, int>>>>,
			std::string
		>> testResults{
		{
			{{{{"42", 0}, std::nullopt}, 0}},
			"42"
		},
		{
			{{{{"3", 0}, std::nullopt}, 0}, {{{"5", 2}, "3"}, 1}, {{{"6", 3}, "3"}, 1}, {{{"2", 7}, "6"}, 2}},
			"3"
		},
		{
			{{{{"6", 0}, std::nullopt}, 0}, {{{"2", 4}, "6"}, 1}},
			"6"
		},
		// non-deterministic, as 6 may have the predecessor 2 or 3
		//{
		//	{{{{"1", 0}, std::nullopt}, 0}, {{{"2", 1}, "1"}, 1}, {{{"3", 2}, "1"}, 1}, {{{"6", 5}, "2"}, 2}, {{{"5", 4}, "3"}}, 2},
		//	"1"
		//},
		{
			{{{{"8", 0}, std::nullopt}, 0}, {{{"7", 1}, "8"}, 1}, {{{"4", 4}, "7"}, 2}, {{{"9", 1}, "8"}, 1}},
			"8"
		}
	};
}

TEMPLATE_TEST_CASE(
	"ucs::Range visits all reachable vertices.",
	"[graph][graph::ucs]",
	WeightedViewStub
)
{
	using Node = sg::CommonRankedNode<std::string, int>;
	const auto& [expected, origin] = GENERATE(from_range(slice_test_expectations(testResults, toCommonRankedNode)));

	sg::ucs::Range<TestType, Node> range{origin, std::tuple{TestType{}}, std::tuple{}, std::tuple{}, std::tuple{}};
	STATIC_CHECK(std::ranges::input_range<decltype(range)>);

	REQUIRE_THAT(buffer_nodes(range), Catch::Matchers::UnorderedRangeEquals(expected));
}

TEMPLATE_TEST_CASE(
	"ucs::Range node can be decorated with DepthNode.",
	"[graph][graph::ucs]",
	WeightedViewStub
)
{
	using Node = sg::decorator::DepthNode<sg::CommonRankedNode<std::string, int>>;
	const auto& [expected, origin] = GENERATE(from_range(slice_test_expectations(testResults, toDepthRankedNode)));

	sg::ucs::Range<TestType, Node> range{origin, std::tuple{TestType{}}, std::tuple{}, std::tuple{}, std::tuple{}};
	STATIC_CHECK(std::ranges::input_range<decltype(range)>);

	REQUIRE_THAT(buffer_nodes(range), Catch::Matchers::UnorderedRangeEquals(expected));
}

TEMPLATE_TEST_CASE(
	"ucs::Range node can be decorated with PredecessorNode.",
	"[graph][graph::ucs]",
	WeightedViewStub
)
{
	using Node = sg::decorator::PredecessorNode<sg::CommonRankedNode<std::string, int>>;
	const auto& [expected, origin] = GENERATE(from_range(slice_test_expectations(testResults, toPredecessorRankedNode)));

	sg::ucs::Range<TestType, Node> range{origin, std::tuple{TestType{}}, std::tuple{}, std::tuple{}, std::tuple{}};
	STATIC_CHECK(std::ranges::input_range<decltype(range)>);

	REQUIRE_THAT(buffer_nodes(range), Catch::Matchers::UnorderedRangeEquals(expected));
}

TEMPLATE_TEST_CASE(
	"ucs::Range can be used with arbitrary decorated nodes.",
	"[graph][graph::ucs]",
	WeightedViewStub
)
{
	using Node = sg::decorator::DepthNode<sg::decorator::PredecessorNode<sg::CommonRankedNode<std::string, int>>>;
	const auto& [expected, origin] = GENERATE(from_range(testResults));

	sg::ucs::Range<TestType, Node> range{origin, std::tuple{TestType{}}, std::tuple{}, std::tuple{}, std::tuple{}};
	STATIC_CHECK(std::ranges::input_range<decltype(range)>);

	REQUIRE_THAT(buffer_nodes(range), Catch::Matchers::UnorderedRangeEquals(expected));
}
