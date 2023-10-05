//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/graph/AStarSearch.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include "Defines.hpp"

namespace
{
	inline const std::vector<
		std::tuple<
			std::vector<sg::decorator::DepthNode<sg::decorator::PredecessorNode<sg::astar::Node<std::string, int>>>>,
			std::string, // origin
			std::string  // destination
		>> testResults{
		{
			{{{{"42", 0, 30}, std::nullopt}, 0}},
			"42",
			"12"
		},
		{
			{{{{"3", 0, 6}, std::nullopt}, 0}, {{{"5", 2, 4}, "3"}, 1}, {{{"6", 3, 3}, "3"}, 1}, {{{"2", 7, 7}, "6"}, 2}},
			"3",
			"9"
		},
		{
			{{{{"6", 0, 3}, std::nullopt}, 0}, {{{"2", 4, 7}, "6"}, 1}},
			"6",
			"9"
		},
		// non-deterministic, as 6 may have the predecessor 2 or 3
		//{
		//	{{{{"1", 0, 8}, std::nullopt}, 0}, {{{"2", 1, 7}, "1"}, 1}, {{{"3", 2, 6}, "1"}, 1}, {{{"6", 5, 3}, "2"}, 2}, {{{"5", 4, 5}, "3"}}, 2},
		//	"1",
		//	"9"
		//},
		{
			{{{{"8", 0, 1}, std::nullopt}, 0}, {{{"7", 1, 2}, "8"}, 1}, {{{"4", 4, 5}, "7"}, 2}, {{{"9", 1, 0}, "8"}, 1}},
			"8",
			"9"
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

	constexpr auto toCommonAStarNode = []<sg::concepts::ranked_node Node>(const Node& node)
	{
		return sg::astar::Node<sg::node::vertex_t<Node>, sg::node::rank_t<Node>>{
			sg::node::vertex(node),
			node.cost,
			node.estimatedPendingCost
		};
	};

	constexpr auto toDepthAStarNode = []<typename Node>(const sg::decorator::DepthNode<Node>& node)
	{
		return sg::decorator::DepthNode<sg::astar::Node<sg::node::vertex_t<Node>, sg::node::rank_t<Node>>>{
			{toCommonAStarNode(node)},
			node.depth
		};
	};

	constexpr auto toPredecessorAStarNode = []<typename Node>(const sg::decorator::PredecessorNode<Node>& node)
	{
		return sg::decorator::PredecessorNode<sg::astar::Node<sg::node::vertex_t<Node>, sg::node::rank_t<Node>>>{
			{toCommonAStarNode(node)},
			node.predecessor
		};
	};
}

TEMPLATE_TEST_CASE(
	"astar::Range visits all reachable vertices.",
	"[graph][graph::astar]",
	WeightedViewStub)
{
	using Node = sg::astar::Node<std::string, int>;
	const auto& [expected, origin, destination] = GENERATE(from_range(slice_test_expectations(testResults, toCommonAStarNode)));

	sg::astar::Range<TestType, Heuristic, Node> range{
		origin,
		std::tuple{TestType{}},
		std::tuple{},
		std::tuple{},
		std::tuple{Heuristic{destination}, sg::astar::NodeFactory<Node>{}},
		std::tuple{}
	};
	STATIC_CHECK(std::ranges::input_range<decltype(range)>);

	REQUIRE_THAT(buffer_nodes(range), Catch::Matchers::UnorderedRangeEquals(expected));
}

TEMPLATE_TEST_CASE(
	"astar::Range node can be decorated with DepthNodeDecorator.",
	"[graph][graph::astar]",
	WeightedViewStub
)
{
	using Node = sg::decorator::DepthNode<sg::astar::Node<std::string, int>>;
	const auto& [expected, origin, destination] = GENERATE(from_range(slice_test_expectations(testResults, toDepthAStarNode)));

	sg::astar::Range<TestType, Heuristic, Node> range{
		origin,
		std::tuple{TestType{}},
		std::tuple{},
		std::tuple{},
		std::tuple{Heuristic{destination}, sg::astar::NodeFactory<Node>{}},
		std::tuple{}
	};
	STATIC_CHECK(std::ranges::input_range<decltype(range)>);

	REQUIRE_THAT(buffer_nodes(range), Catch::Matchers::UnorderedRangeEquals(expected));
}

TEMPLATE_TEST_CASE(
	"astar::Range node can be decorated with PredecessorNodeDecorator.",
	"[graph][graph::astar]",
	WeightedViewStub
)
{
	using Node = sg::decorator::PredecessorNode<sg::astar::Node<std::string, int>>;
	const auto& [expected, origin, destination] = GENERATE(from_range(slice_test_expectations(testResults, toPredecessorAStarNode)));

	sg::astar::Range<TestType, Heuristic, Node> range{
		origin,
		std::tuple{TestType{}},
		std::tuple{},
		std::tuple{},
		std::tuple{Heuristic{destination}, sg::astar::NodeFactory<Node>{}},
		std::tuple{}
	};
	STATIC_CHECK(std::ranges::input_range<decltype(range)>);

	REQUIRE_THAT(buffer_nodes(range), Catch::Matchers::UnorderedRangeEquals(expected));
}

TEMPLATE_TEST_CASE(
	"astar::Range can be used with arbitrary decorated nodes.",
	"[graph][graph::astar]",
	WeightedViewStub
)
{
	using Node = sg::decorator::DepthNode<sg::decorator::PredecessorNode<sg::astar::Node<std::string, int>>>;
	const auto& [expected, origin, destination] = GENERATE(from_range(testResults));

	sg::astar::Range<TestType, Heuristic, Node> range{
		origin,
		std::tuple{TestType{}},
		std::tuple{},
		std::tuple{},
		std::tuple{Heuristic{destination}, sg::astar::NodeFactory<Node>{}},
		std::tuple{}
	};
	STATIC_CHECK(std::ranges::input_range<decltype(range)>);

	REQUIRE_THAT(buffer_nodes(range), Catch::Matchers::UnorderedRangeEquals(expected));
}
