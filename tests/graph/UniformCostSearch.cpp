//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/graph/UniformCostSearch.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include "Defines.hpp"

namespace
{
	using Node = sg::CommonRankedNode<std::string, int>;
	using Edge = sg::CommonWeightedEdge<std::string, int>;

	struct View
	{
		using edge_type = Edge;

		inline static const std::unordered_map<std::string, std::vector<int>> graph{
			{"1", {2, 3}},
			{"2", {6}},
			{"3", {5, 6}},
			{"5", {5}},
			{"6", {2}},

			// begin isolated sub-graph
			{"4", {7}},
			{"7", {4, 7, 9}},
			{"8", {7, 9}},
			{"9", {4}}
		};

		template <sg::concepts::ranked_node Node>
			requires sg::concepts::edge_for<edge_type, Node>
		static std::vector<edge_type> edges(const Node& current)
		{
			const auto& vertices = graph.at(sg::node::vertex(current));
			std::vector<edge_type> infos{};
			infos.reserve(std::ranges::size(vertices));
			std::ranges::transform(
				vertices,
				std::back_inserter(infos),
				[&](const int v)
				{
					return edge_type{
						.destination = std::to_string(v),
						.weight = std::abs(v - std::stoi(current.vertex))
					};
				});
			return infos;
		}
	};
}

TEST_CASE("ucs::Range visits all reachable vertices.", "[graph][graph::ucs]")
{
	const auto& [expected, origin] = GENERATE(
		(table<std::vector<Node>, std::string>)({
			{{{"3", 0}, {"5", 2}, {"6", 3}, {"2", 7}}, "3"},
			{{{"6", 0}, {"2", 4}}, "6"},
			{{{"1", 0}, {"2", 1}, {"3", 2}, {"6", 5}, {"5", 4}}, "1"},
			{{{"8", 0}, {"7", 1}, {"4", 4}, {"9", 1}}, "8"}
			}));

	sg::ucs::Range<View> range{origin, std::tuple{View{}}, std::tuple{}, std::tuple{}, std::tuple{}, std::tuple{}};
	STATIC_CHECK(std::ranges::input_range<decltype(range)>);

	std::vector<Node> nodes{};
	std::ranges::copy(range, std::back_inserter(nodes));

	REQUIRE_THAT(nodes, Catch::Matchers::UnorderedRangeEquals(expected));
}

TEST_CASE("ucs::Range node can be decorated with PredecessorNodeDecorator.", "[graph][graph::ucs]")
{
	using DecoratedNode = sg::PredecessorNodeDecorator<::Node>;

	const auto& [expected, origin] = GENERATE(
		(table<std::vector<DecoratedNode>, std::string>)({
			{{{{"3", 0}, std::nullopt}, {{"5", 2}, "3"}, {{"6", 3}, "3"}, {{"2", 7}, "6"}}, "3"},
			{{{{"6", 0}, std::nullopt}, {{"2", 4}, "6"}}, "6"},
			// non-deterministic, as 6 may have the predecessor 2 or 3
			//{{{{"1", 0}, std::nullopt}, {{"2", 1}, "1"}, {{"3", 2}, "1"}, {{"6", 5}, "2"}, {{"5", 4}, "3"}}, "1"},
			{{{{"8", 0}, std::nullopt}, {{"7", 1}, "8"}, {{"4", 4}, "7"}, {{"9", 1}, "8"}}, "8"}
			}));

	sg::ucs::Range<View, DecoratedNode> range{origin, std::tuple{View{}}, std::tuple{}, std::tuple{}, std::tuple{}, std::tuple{}};
	STATIC_CHECK(std::ranges::input_range<decltype(range)>);

	std::vector<DecoratedNode> nodes{};
	std::ranges::copy(range, std::back_inserter(nodes));

	REQUIRE_THAT(nodes, Catch::Matchers::UnorderedRangeEquals(expected));
}
