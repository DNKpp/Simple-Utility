//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/graph/AStarSearch.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include "Defines.hpp"

namespace
{
	using Node = sg::astar::Node<std::string, int>;
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

	struct manhattan_distance
	{
		[[nodiscard]]
		int operator ()(const std::string& dest, const std::string& cur) const noexcept
		{
			return std::abs(std::stoi(dest) - std::stoi(cur));
		}
	};

	using Heuristic = sg::astar::SingleDestinationHeuristic<std::string, manhattan_distance>;
}

TEST_CASE("astar::Range visits all reachable vertices.", "[graph][graph::astar]")
{
	const auto& [expected, origin, destination] = GENERATE(
		(table<std::vector<Node>, std::string, std::string>)({
			{{{"3", 0, 6}, {"5", 2, 4}, {"6", 3, 3}, {"2", 7, 7}}, "3", "9"},
			{{{"6", 0, 3}, {"2", 4, 7}}, "6", "9"},
			{{{"1", 0, 8}, {"2", 1, 7}, {"3", 2, 6}, {"6", 5, 3}, {"5", 4, 4}}, "1", "9"},
			{{{"8", 0, 1}, {"7", 1, 2}, {"4", 4, 5}, {"9", 1, 0}}, "8", "9"}
			}));

	sg::astar::Range<View, Heuristic> range{
		origin,
		std::tuple{View{}},
		std::tuple{},
		std::tuple{},
		std::tuple{Heuristic{destination}},
		std::tuple{}
	};
	STATIC_CHECK(std::ranges::input_range<decltype(range)>);

	std::vector<Node> nodes{};
	std::ranges::copy(range, std::back_inserter(nodes));

	REQUIRE_THAT(nodes, Catch::Matchers::UnorderedRangeEquals(expected));
}

TEST_CASE("astar::Range node can be decorated with PredecessorNodeDecorator.", "[graph][graph::ucs]")
{
	using DecoratedNode = sg::PredecessorNodeDecorator<::Node>;

	const auto& [expected, origin, destination] = GENERATE(
		(table<std::vector<DecoratedNode>, std::string, std::string>)({
			{{{{"3", 0, 6}, std::nullopt}, {{"5", 2, 4}, "3"}, {{"6", 3, 3}, "3"}, {{"2", 7, 7}, "6"}}, "3", "9"},
			{{{{"6", 0, 3}, std::nullopt}, {{"2", 4, 7}, "6"}}, "6", "9"},
			{{{{"8", 0, 1}, std::nullopt}, {{"7", 1, 2}, "8"}, {{"4", 4, 5}, "7"}, {{"9", 1, 0}, "8"}}, "8", "9"}
			}));

	sg::astar::Range<View, Heuristic, DecoratedNode> range{
		origin,
		std::tuple{View{}},
		std::tuple{},
		std::tuple{},
		std::tuple{Heuristic{destination}},
		std::tuple{}
	};
	STATIC_CHECK(std::ranges::input_range<decltype(range)>);

	std::vector<DecoratedNode> nodes{};
	std::ranges::copy(range, std::back_inserter(nodes));

	REQUIRE_THAT(nodes, Catch::Matchers::UnorderedRangeEquals(expected));
}
