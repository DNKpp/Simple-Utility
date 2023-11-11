//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <catch2/catch_test_macros.hpp>
#include <catch2/trompeloeil.hpp>

#include "Simple-Utility/TypeList.hpp"
#include "Simple-Utility/functional/Tuple.hpp"
#include "Simple-Utility/graph/Common.hpp"
#include "Simple-Utility/graph/Edge.hpp"
#include "Simple-Utility/graph/Formatter.hpp"
#include "Simple-Utility/graph/Graph.hpp"
#include "Simple-Utility/graph/Node.hpp"
#include "Simple-Utility/graph/Tracker.hpp"
#include "Simple-Utility/test_util/Catch2Ext.hpp"
#include "Simple-Utility/test_util/TrompeloeilExt.hpp"

namespace sg = sl::graph;

template <sg::concepts::vertex Vertex>
struct GenericBasicNode
{
	using vertex_type = Vertex;

	vertex_type vertex;

	friend bool operator==(const GenericBasicNode&, const GenericBasicNode&) = default;
};

template <sg::concepts::vertex Vertex, sg::concepts::rank Rank>
struct GenericRankedNode
{
	using vertex_type = Vertex;
	using rank_type = Rank;

	vertex_type vertex;
	rank_type rank;

	friend bool operator==(const GenericRankedNode&, const GenericRankedNode&) = default;
};

template <sg::concepts::vertex Vertex>
struct GenericBasicEdge
{
	using vertex_type = Vertex;

	vertex_type destination;

	friend bool operator==(const GenericBasicEdge&, const GenericBasicEdge&) = default;
};

template <sg::concepts::vertex Vertex, sg::concepts::weight Weight>
struct GenericWeightedEdge
{
	using vertex_type = Vertex;
	using weight_type = Weight;

	vertex_type destination;
	weight_type weight;

	friend bool operator==(const GenericWeightedEdge&, const GenericWeightedEdge&) = default;
};

template <sg::concepts::basic_node Node>
class QueueMock
{
public:
	inline static constexpr bool trompeloeil_movable_mock = true;

	MAKE_CONST_MOCK0(empty, bool());
	// Can't expect a template here, so just expect a std::vector
	MAKE_MOCK1(do_insert, void(std::vector<Node>));
	MAKE_MOCK0(next, Node());

	template <std::ranges::input_range Range>
		requires std::convertible_to<std::ranges::range_value_t<Range>, Node>
	friend constexpr void insert(QueueMock& queue, Range&& range)
	{
		std::vector<std::ranges::range_value_t<Range>> vector{};
		std::ranges::copy(range, std::back_inserter(vector));
		queue.do_insert(std::move(vector));
	}
};

template <sg::concepts::basic_node Node>
struct EmptyQueueStub
{
	[[nodiscard]]
	static constexpr bool empty() noexcept { return true; }

	static constexpr void insert(auto&&) noexcept
	{
	}

	static constexpr Node next()
	{
		return {};
	}
};

template <sg::concepts::vertex Vertex>
class TrackerMock
{
public:
	inline static constexpr bool trompeloeil_movable_mock = true;

	MAKE_MOCK1(set_discovered, bool(const Vertex&));
	MAKE_MOCK1(set_visited, bool(const Vertex&));
};

template <sg::concepts::vertex Vertex>
class BasicGraphMock
{
public:
	inline static constexpr bool trompeloeil_movable_mock = true;

	using vertex_type = Vertex;
	using edge_type = GenericBasicEdge<vertex_type>;

	MAKE_CONST_MOCK1(out_edges, std::vector<edge_type>(const vertex_type&));
};

template <sg::concepts::vertex Vertex, sg::concepts::weight Weight>
class WeightedGraphMock
{
public:
	inline static constexpr bool trompeloeil_movable_mock = true;

	using vertex_type = Vertex;
	using weight_type = Weight;
	using edge_type = GenericWeightedEdge<vertex_type, weight_type>;

	MAKE_CONST_MOCK1(out_edges, std::vector<edge_type>(const vertex_type&));
};

template <sg::concepts::vertex Vertex>
class EmptyGraphStub
{
public:
	using vertex_type = Vertex;
	using edge_type = GenericBasicEdge<vertex_type>;

	static constexpr std::array<edge_type, 0> out_edges([[maybe_unused]] const vertex_type&) noexcept
	{
		return {};
	}
};

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

struct BasicGraphStub
{
	using vertex_type = std::string;
	using edge_type = sg::CommonBasicEdge<vertex_type>;

	static std::vector<edge_type> out_edges(const vertex_type& current)
	{
		const auto vertex = std::stoi(current);
		if (!graph.contains(vertex))
		{
			return {};
		}

		const auto& vertices = graph.at(vertex);
		std::vector<edge_type> infos{};
		infos.reserve(std::ranges::size(vertices));
		std::ranges::transform(
			vertices,
			std::back_inserter(infos),
			[](const int v) { return edge_type{.destination = std::to_string(v)}; });
		return infos;
	}
};

struct WeightedGraphStub
{
	using vertex_type = std::string;
	using edge_type = sg::CommonWeightedEdge<std::string, int>;

	static std::vector<edge_type> out_edges(const vertex_type& current)
	{
		const auto vertex = std::stoi(current);
		if (!graph.contains(vertex))
		{
			return {};
		}

		const auto& vertices = graph.at(vertex);
		std::vector<edge_type> infos{};
		infos.reserve(std::ranges::size(vertices));
		std::ranges::transform(
			vertices,
			std::back_inserter(infos),
			[&](const int v)
			{
				return edge_type{
					.destination = std::to_string(v),
					.weight = std::abs(v - vertex)
				};
			});
		return infos;
	}
};

template <typename Range>
constexpr auto buffer_nodes(Range& range)
{
	std::vector<typename Range::node_type> nodes{};
	std::ranges::copy(range, std::back_inserter(nodes));
	return nodes;
}

template <std::ranges::input_range Range, typename Transform>
constexpr auto slice_test_expectations(const Range& range, Transform transform)
{
	using TargetNode = std::invoke_result_t<
		Transform,
		std::ranges::range_value_t<std::tuple_element_t<0, std::ranges::range_value_t<Range>>>>;
	std::vector<
		sl::type_list::prepend_t<
			sl::type_list::pop_front_t<
				std::ranges::range_value_t<Range>>,
			std::vector<TargetNode>>> results{};
	std::ranges::transform(
		range,
		std::back_inserter(results),
		sl::functional::envelop<sl::functional::Apply>(
			[&]<typename Nodes, typename... Args>(const Nodes& nodes, Args&&... args)
			{
				std::vector<TargetNode> targetNodes{};
				std::ranges::transform(nodes, std::back_inserter(targetNodes), transform);
				return std::tuple{
					std::move(targetNodes),
					std::forward<Args>(args)...
				};
			}));

	return results;
}

constexpr auto toCommonBasicNode = []<sg::concepts::basic_node Node>(const Node& node)
{
	return sg::CommonBasicNode<sg::node::vertex_t<Node>>{sg::node::vertex(node)};
};

constexpr auto toDepthBasicNode = []<typename Node>(const sg::decorator::DepthNode<Node>& node)
{
	return sg::decorator::DepthNode<sg::CommonBasicNode<sg::node::vertex_t<Node>>>{
		{toCommonBasicNode(node)},
		node.depth
	};
};

constexpr auto toPredecessorBasicNode = []<typename Node>(const sg::decorator::PredecessorNode<Node>& node)
{
	return sg::decorator::PredecessorNode<sg::CommonBasicNode<sg::node::vertex_t<Node>>>{
		{toCommonBasicNode(node)},
		node.predecessor
	};
};

constexpr auto toCommonRankedNode = []<sg::concepts::ranked_node Node>(const Node& node)
{
	return sg::CommonRankedNode<sg::node::vertex_t<Node>, sg::node::rank_t<Node>>{
		sg::node::vertex(node),
		sg::node::rank(node)
	};
};

constexpr auto toDepthRankedNode = []<typename Node>(const sg::decorator::DepthNode<Node>& node)
{
	// leave code as-is, because directly returning the temporary results in an ICE on gcc10
	sg::decorator::DepthNode<sg::CommonRankedNode<sg::node::vertex_t<Node>, sg::node::rank_t<Node>>> sliced{
		{toCommonRankedNode(node)},
		node.depth
	};

	return sliced;
};

constexpr auto toPredecessorRankedNode = []<typename Node>(const sg::decorator::PredecessorNode<Node>& node)
{
	// leave code as-is, because directly returning the temporary results in an ICE on gcc10
	sg::decorator::PredecessorNode<sg::CommonRankedNode<sg::node::vertex_t<Node>, sg::node::rank_t<Node>>> sliced{
		{toCommonRankedNode(node)},
		node.predecessor
	};

	return sliced;
};
