//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <catch2/catch_test_macros.hpp>
#include <catch2/trompeloeil.hpp>

#include "Simple-Utility/graph/Common.hpp"
#include "Simple-Utility/graph/Node.hpp"
#include "Simple-Utility/graph/Tracker.hpp"

namespace sg = sl::graph;

template <sg::concepts::vertex Vertex>
struct BasicTestNode
{
	using vertex_type = Vertex;

	struct Mock
	{
		MAKE_CONST_MOCK1(vertex, vertex_type(const BasicTestNode&));
	} inline static mock{};

	friend constexpr vertex_type vertex(const BasicTestNode& node)
	{
		return mock.vertex(node);
	}
};

template <sg::concepts::vertex Vertex, sg::concepts::rank Rank>
struct RankedTestNode
{
	using vertex_type = Vertex;
	using rank_type = Rank;

	struct Mock
	{
		MAKE_CONST_MOCK1(vertex, vertex_type(const RankedTestNode&));
		MAKE_CONST_MOCK1(rank, rank_type(const RankedTestNode&));
	} inline static mock{};

	friend constexpr vertex_type vertex(const RankedTestNode& node)
	{
		return mock.vertex(node);
	}

	friend constexpr rank_type rank(const RankedTestNode& node)
	{
		return mock.rank(node);
	}
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

template <sg::concepts::node Node, sg::concepts::node VertexInfo>
	requires sg::concepts::compatible_with<Node, VertexInfo>
class BasicNodeFactoryMock
{
public:
	inline static constexpr bool trompeloeil_movable_mock = true;

	using vertex_type = sg::feature_vertex_t<Node>;

	MAKE_MOCK1(make_init_node, Node(const vertex_type&));
	MAKE_MOCK2(make_successor_node, Node(const Node&, const VertexInfo&));
};

template <sg::concepts::node Node>
class BasicGraph
{
public:
	struct info
	{
		using vertex_type = sg::feature_vertex_t<Node>;

		vertex_type vertex;

		friend bool operator ==(const info&, const info&) = default; 
	};

	MAKE_CONST_MOCK1(neighbor_infos, std::vector<info>(const Node&));
};
