//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/graph/BreadthFirstSearch.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include "Defines.hpp"

TEMPLATE_TEST_CASE(
	"bfs::Range visits all reachable vertices.",
	"[graph][graph::bfs]",
	BasicViewStub,
	WeightedViewStub
)
{
	using Node = sg::CommonBasicNode<std::string>;
	const auto& [expected, origin] = GENERATE(
		(table<std::vector<Node>, std::string>)({
			{{{"3"}, {"5"}, {"6"}, {"2"}}, "3"},
			{{{"6"}, {"2"}}, "6"},
			{{{"1"}, {"2"}, {"3"}, {"6"}, {"5"}}, "1"},
			{{{"8"}, {"7"}, {"9"}, {"4"}}, "8"}
			}));

	sg::dfs::Range<TestType, Node> range{origin, std::tuple{TestType{}}, std::tuple{}, std::tuple{}, std::tuple{}, std::tuple{}};
	STATIC_CHECK(std::ranges::input_range<decltype(range)>);

	REQUIRE_THAT(buffer_nodes(range), Catch::Matchers::RangeEquals(expected));
}

TEMPLATE_TEST_CASE(
	"bfs::Range can be used with depth decorated nodes.",
	"[graph][graph::bfs]",
	BasicViewStub,
	WeightedViewStub
)
{
	using Node = sg::DepthNodeDecorator<sg::CommonBasicNode<std::string>>;
	const auto& [expected, origin] = GENERATE(
		(table<std::vector<Node>, std::string>)({
			{{{{"3"}, 0}, {{"5"}, 1}, {{"6"}, 1}, {{"2"}, 2}}, "3"},
			{{{{"6"}, 0}, {{"2"}, 1}}, "6"},
			{{{{"1"}, 0}, {{"2"}, 1}, {{"3"}, 1}, {{"6"}, 2}, {{"5"}, 2}}, "1"},
			{{{{"8"}, 0}, {{"7"}, 1}, {{"9"}, 1}, {{"4"}, 2}}, "8"}
			}));

	sg::dfs::Range<TestType, Node> range{origin, std::tuple{TestType{}}, std::tuple{}, std::tuple{}, std::tuple{}, std::tuple{}};
	STATIC_CHECK(std::ranges::input_range<decltype(range)>);

	REQUIRE_THAT(buffer_nodes(range), Catch::Matchers::RangeEquals(expected));
}

TEMPLATE_TEST_CASE(
	"bfs::Range can be used with predecessor decorated nodes.",
	"[graph][graph::bfs]",
	BasicViewStub,
	WeightedViewStub
)
{
	using Node = sg::PredecessorNodeDecorator<sg::CommonBasicNode<std::string>>;
	const auto& [expected, origin] = GENERATE(
		(table<std::vector<Node>, std::string>)({
			{{{{"3"}, std::nullopt}, {{"5"}, "3"}, {{"6"}, "3"}, {{"2"}, "6"}}, "3"},
			{{{{"6"}, std::nullopt}, {{"2"}, "6"}}, "6"},
			{{{{"1"}, std::nullopt}, {{"2"}, "1"}, {{"3"}, "1"}, {{"6"}, "2"}, {{"5"}, "3"}}, "1"},
			{{{{"8"}, std::nullopt}, {{"7"}, "8"}, {{"9"}, "8"}, {{"4"}, "7"}}, "8"}
			}));

	sg::dfs::Range<TestType, Node> range{origin, std::tuple{TestType{}}, std::tuple{}, std::tuple{}, std::tuple{}, std::tuple{}};
	STATIC_CHECK(std::ranges::input_range<decltype(range)>);

	REQUIRE_THAT(buffer_nodes(range), Catch::Matchers::RangeEquals(expected));
}

TEMPLATE_TEST_CASE(
	"bfs::Range can be used with arbitrary decorated nodes.",
	"[graph][graph::bfs]",
	BasicViewStub,
	WeightedViewStub
)
{
	using Node = sg::DepthNodeDecorator<sg::PredecessorNodeDecorator<sg::CommonBasicNode<std::string>>>;
	const auto& [expected, origin] = GENERATE(
		(table<std::vector<Node>, std::string>)({
			{{{{{"3"}, std::nullopt}, 0}, {{{"5"}, "3"}, 1}, {{{"6"}, "3"}, 1}, {{{"2"}, "6"}, 2}}, "3"},
			{{{{{"6"}, std::nullopt}, 0}, {{{"2"}, "6"}, 1}}, "6"},
			{{{{{"1"}, std::nullopt}, 0}, {{{"2"}, "1"}, 1}, {{{"3"}, "1"}, 1}, {{{"6"}, "2"}, 2}, {{{"5"}, "3"}, 2}}, "1"},
			{{{{{"8"}, std::nullopt}, 0}, {{{"7"}, "8"}, 1}, {{{"9"}, "8"}, 1}, {{{"4"}, "7"}, 2}}, "8"}
			}));

	sg::dfs::Range<TestType, Node> range{origin, std::tuple{TestType{}}, std::tuple{}, std::tuple{}, std::tuple{}, std::tuple{}};
	STATIC_CHECK(std::ranges::input_range<decltype(range)>);

	REQUIRE_THAT(buffer_nodes(range), Catch::Matchers::RangeEquals(expected));
}
