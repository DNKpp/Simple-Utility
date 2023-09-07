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

		template <sg::concepts::node Node>
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
	} inline constexpr graph{};
}

TEST_CASE("ucs::BasicTraverser visits all reachable vertices.", "[graph][graph::ucs]")
{
	const auto& [expected, origin] = GENERATE(
		(table<std::vector<Node>, std::string>)({
			{{{"5", 2}, {"6", 3}, {"2", 7}}, "3"},
			{{{"2", 4}}, "6"},
			{{{"2", 1}, {"3", 2}, {"6", 5}, {"5", 4}}, "1"},
			{{{"7", 1}, {"4", 4}, {"9", 1}}, "8"},
			}));

	sg::ucs::BasicTraverser<View> traverser{graph, origin};
	STATIC_CHECK(std::ranges::input_range<decltype(traverser)>);

	std::vector<Node> nodes{};
	std::ranges::copy(traverser, std::back_inserter(nodes));

	REQUIRE_THAT(nodes, Catch::Matchers::UnorderedRangeEquals(expected));
}
