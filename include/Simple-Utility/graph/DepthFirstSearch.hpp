//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "Simple-Utility/graph/Traverse.hpp"
#include "Simple-Utility/graph/mixins/queue/std_stack.hpp"
#include "Simple-Utility/graph/mixins/tracker/std_unordered_map.hpp"

#include <optional>

namespace sl::graph::dfs
{
	template <concepts::vertex Vertex>
	struct Node
	{
		using vertex_type = Vertex;
		vertex_type vertex{};
	};

	template <concepts::vertex Vertex>
	struct ExtendedNode
	{
		using vertex_type = Vertex;
		vertex_type vertex{};
		int depth{};
		std::optional<vertex_type> predecessor{};
	};

	template <concepts::node Node>
	struct NodeFactory;
	
	template <concepts::vertex Vertex>
	struct NodeFactory<Node<Vertex>>
	{
		using node_type = Node<Vertex>;
		using vertex_type = node::vertex_t<node_type>;

		static constexpr node_type make_init_node(vertex_type origin)
		{
			return node_type{.vertex = std::move(origin)};
		}

		template <concepts::edge_for<node_type> Edge>
		[[nodiscard]]
		static constexpr node_type make_successor_node([[maybe_unused]] const node_type& current, const Edge& edge)
		{
			return node_type{.vertex = edge::vertex(edge)};
		}
	};

	template <concepts::vertex Vertex>
	struct NodeFactory<ExtendedNode<Vertex>>
	{
		using node_type = ExtendedNode<Vertex>;
		using vertex_type = node::vertex_t<node_type>;

		static constexpr node_type make_init_node(vertex_type origin)
		{
			return node_type{.vertex = std::move(origin), .depth = 0, .predecessor = std::nullopt};
		}

		template <concepts::edge_for<node_type> Edge>
		[[nodiscard]]
		static constexpr node_type make_successor_node(const node_type& current, const Edge& edge)
		{
			return node_type{.vertex = edge::vertex(edge), .depth = current.depth + 1, .predecessor = current.vertex};
		}
	};

	template <concepts::vertex Vertex>
	struct Edge
	{
		using vertex_type = Vertex;
		vertex_type vertex{};
	};

	template <
		class View,
		concepts::node Node = Node<edge::vertex_t<view::edge_t<View>>>,
		concepts::node_factory_for<Node, view::edge_t<View>> NodeFactory = NodeFactory<Node>,
		concepts::tracker_for<node::vertex_t<Node>> Tracker = std::unordered_map<node::vertex_t<Node>, bool>>
		requires concepts::view_for<View, Node>
	using BasicTraverser = Traverser<
		View,
		detail::BasicTraverseDriver<
			Node,
			detail::BasicState<Node, std::stack<Node>>,
			Tracker,
			NodeFactory>>;

	template <class View>
	using ExtendedTraverser = BasicTraverser<View, ExtendedNode<edge::vertex_t<view::edge_t<View>>>>;
}
