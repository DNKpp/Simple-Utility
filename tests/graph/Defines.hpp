//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <catch2/catch_test_macros.hpp>
#include <catch2/trompeloeil.hpp>

#include "Simple-Utility/graph/Common.hpp"
#include "Simple-Utility/graph/Edge.hpp"
#include "Simple-Utility/graph/Node.hpp"
#include "Simple-Utility/graph/Tracker.hpp"
#include "Simple-Utility/graph/View.hpp"

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

	vertex_type vertex;

	friend bool operator==(const GenericBasicEdge&, const GenericBasicEdge&) = default;
};

template <sg::concepts::vertex Vertex, sg::concepts::weight Weight>
struct GenericWeightedEdge
{
	using vertex_type = Vertex;
	using weight_type = Weight;

	vertex_type vertex;
	weight_type weight;

	friend bool operator==(const GenericWeightedEdge&, const GenericWeightedEdge&) = default;
};

template <sg::concepts::node Node>
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

template <sg::concepts::node Node>
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

template <sg::concepts::node Node>
class GenericBasicNodeFactoryMock
{
public:
	inline static constexpr bool trompeloeil_movable_mock = true;

	using node_type = Node;
	using vertex_type = sg::node::vertex_t<Node>;

	MAKE_MOCK1(make_init_node, Node(const vertex_type&));
	MAKE_MOCK2(make_successor_node, node_type(const node_type&, const GenericBasicEdge<vertex_type>&));

	template <sg::concepts::edge_for<node_type> Edge>
	node_type make_successor_node(const node_type& current, const Edge& edge)
	{
		return make_successor_node(current, GenericBasicEdge<vertex_type>{.vertex = sg::edge::vertex(edge)});
	}
};

template <sg::concepts::ranked_node Node>
class GenericRankedNodeFactoryMock
{
public:
	inline static constexpr bool trompeloeil_movable_mock = true;

	using node_type = Node;
	using vertex_type = sg::node::vertex_t<Node>;

	MAKE_MOCK1(make_init_node, Node(const vertex_type&));
	MAKE_MOCK2(make_successor_node, node_type(const node_type&, const GenericBasicEdge<vertex_type>&));

	template <sg::concepts::edge_for<node_type> Edge>
	node_type make_successor_node(const node_type& current, const Edge& edge)
	{
		return make_successor_node(current, GenericBasicEdge<vertex_type>{.vertex = sg::edge::vertex(edge)});
	}
};

template <sg::concepts::vertex Vertex>
class BasicViewMock
{
public:
	using edge_type = GenericBasicEdge<Vertex>;

	MAKE_CONST_MOCK1(edges, std::vector<edge_type>(const GenericBasicNode<Vertex>&));

	std::vector<edge_type> edges(const sg::concepts::node auto& node)
	{
		return edges(GenericBasicNode<Vertex>{.vertex = sg::node::vertex(node)});
	}
};
