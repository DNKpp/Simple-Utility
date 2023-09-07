//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_DEPTH_FIRST_SEARCH_HPP
#define SIMPLE_UTILITY_GRAPH_DEPTH_FIRST_SEARCH_HPP

#pragma once

#include "Simple-Utility/graph/Traverse.hpp"
#include "Simple-Utility/graph/mixins/queue/std_stack.hpp"
#include "Simple-Utility/graph/mixins/tracker/std_unordered_map.hpp"

namespace sl::graph::dfs
{
	template <concepts::node Node>
	struct NodeFactory;

	template <concepts::vertex Vertex>
	struct NodeFactory<CommonBasicNode<Vertex>>
	{
		using node_type = CommonBasicNode<Vertex>;
		using vertex_type = node::vertex_t<node_type>;

		static constexpr node_type make_init_node(vertex_type origin)
		{
			return node_type{.vertex = std::move(origin)};
		}

		template <concepts::edge_for<node_type> Edge>
		[[nodiscard]]
		static constexpr node_type make_successor_node([[maybe_unused]] const node_type& current, const Edge& edge)
		{
			return node_type{.vertex = edge::destination(edge)};
		}
	};

	template <
		class View,
		concepts::node Node = CommonBasicNode<edge::vertex_t<view::edge_t<View>>>,
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
}

#endif
