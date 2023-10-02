//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/graph/DepthFirstSearch.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include "Defines.hpp"

namespace
{
	inline const std::vector<
		std::tuple<
			std::vector<sg::DepthNodeDecorator<sg::PredecessorNodeDecorator<sg::CommonBasicNode<std::string>>>>,
			std::string
		>> testResults{
		{
			{{{{"3"}, std::nullopt}, 0}, {{{"6"}, "3"}, 1}, {{{"2"}, "6"}, 2}, {{{"5"}, "3"}, 1}},
			"3"
		},
		{
			{{{{"6"}, std::nullopt}, 0}, {{{"2"}, "6"}, 1}},
			"6"
		},
		{
			{{{{"1"}, std::nullopt}, 0}, {{{"3"}, "1"}, 1}, {{{"6"}, "3"}, 2}, {{{"2"}, "6"}, 3}, {{{"5"}, "3"}, 2}},
			"1"
		},
		{
			{{{{"8"}, std::nullopt}, 0}, {{{"9"}, "8"}, 1}, {{{"4"}, "9"}, 2}, {{{"7"}, "4"}, 3}},
			"8"
		}
	};
}


TEMPLATE_TEST_CASE(
	"dfs::Range visits all reachable vertices.",
	"[graph][graph::dfs]",
	BasicViewStub,
	WeightedViewStub
)
{
	using Node = sg::CommonBasicNode<std::string>;
	const auto& [expected, origin] = GENERATE(from_range(convert_test_expectations(testResults, toCommonBasicNode)));

	sg::dfs::Range<TestType, Node> range{origin, std::tuple{TestType{}}, std::tuple{}, std::tuple{}, std::tuple{}, std::tuple{}};
	STATIC_CHECK(std::ranges::input_range<decltype(range)>);

	REQUIRE_THAT(buffer_nodes(range), Catch::Matchers::RangeEquals(expected));
}

TEMPLATE_TEST_CASE(
	"dfs::Range can be used with depth decorated nodes.",
	"[graph][graph::dfs]",
	BasicViewStub,
	WeightedViewStub
)
{
	using Node = sg::DepthNodeDecorator<sg::CommonBasicNode<std::string>>;
	const auto& [expected, origin] = GENERATE(from_range(convert_test_expectations(testResults, toDepthBasicNode)));

	sg::dfs::Range<TestType, Node> range{origin, std::tuple{TestType{}}, std::tuple{}, std::tuple{}, std::tuple{}, std::tuple{}};
	STATIC_CHECK(std::ranges::input_range<decltype(range)>);

	REQUIRE_THAT(buffer_nodes(range), Catch::Matchers::RangeEquals(expected));
}

TEMPLATE_TEST_CASE(
	"dfs::Range can be used with predecessor decorated nodes.",
	"[graph][graph::dfs]",
	BasicViewStub,
	WeightedViewStub
)
{
	using Node = sg::PredecessorNodeDecorator<sg::CommonBasicNode<std::string>>;
	const auto& [expected, origin] = GENERATE(from_range(convert_test_expectations(testResults, toPredecessorBasicNode)));

	sg::dfs::Range<TestType, Node> range{origin, std::tuple{TestType{}}, std::tuple{}, std::tuple{}, std::tuple{}, std::tuple{}};
	STATIC_CHECK(std::ranges::input_range<decltype(range)>);

	REQUIRE_THAT(buffer_nodes(range), Catch::Matchers::RangeEquals(expected));
}

TEMPLATE_TEST_CASE(
	"dfs::Range can be used with arbitrary decorated nodes.",
	"[graph][graph::dfs]",
	BasicViewStub,
	WeightedViewStub
)
{
	using Node = sg::DepthNodeDecorator<sg::PredecessorNodeDecorator<sg::CommonBasicNode<std::string>>>;
	const auto& [expected, origin] = GENERATE(from_range(testResults));

	sg::dfs::Range<TestType, Node> range{origin, std::tuple{TestType{}}, std::tuple{}, std::tuple{}, std::tuple{}, std::tuple{}};
	STATIC_CHECK(std::ranges::input_range<decltype(range)>);

	REQUIRE_THAT(buffer_nodes(range), Catch::Matchers::RangeEquals(expected));
}
