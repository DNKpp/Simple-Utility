//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/graph/DepthFirstSearch.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include "Defines.hpp"

namespace
{
	struct View
	{
		inline static const std::unordered_map<int, std::vector<int>> graph{
			{1, {2, 3}},
			{2, {6}},
			{3, {5, 6}},
			{5, {5}},
			{6, {2}},

			// begin isolated sub-graph
			{4, {7}},
			{7, {4, 7, 9}},
			{8, {7, 9}},
			{9, {4}}
		};

		using edge_type = sg::CommonBasicEdge<int>;

		template <sg::concepts::basic_node Node>
			requires sg::concepts::edge_for<edge_type, Node>
		static std::vector<edge_type> edges(const Node& current)
		{
			const auto& vertices = graph.at(sg::node::vertex(current));
			std::vector<edge_type> infos{};
			infos.reserve(std::ranges::size(vertices));
			std::ranges::transform(
				vertices,
				std::back_inserter(infos),
				[](const int v) { return edge_type{.destination = v}; });
			return infos;
		}
	};
}

TEST_CASE("dfs::Range visits all reachable vertices.", "[graph][graph::dfs]")
{
	const auto& [expected, origin] = GENERATE(
		(table<std::vector<sg::CommonBasicNode<int>>, int>)({
			{{{3}, {6}, {2}, {5}}, 3},
			{{{6}, {2}}, 6},
			{{{1}, {3}, {6}, {2}, {5}}, 1},
			{{{8}, {9}, {4}, {7}}, 8},
			}));

	sg::dfs::Range<View> range{origin, std::tuple{View{}}, std::tuple{}, std::tuple{}, std::tuple{}, std::tuple{}};
	STATIC_CHECK(std::ranges::input_range<decltype(range)>);

	std::vector<sg::CommonBasicNode<int>> nodes{};
	std::ranges::copy(range, std::back_inserter(nodes));

	REQUIRE_THAT(nodes, Catch::Matchers::RangeEquals(expected));
}
