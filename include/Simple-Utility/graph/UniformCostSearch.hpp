//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_UNIFORM_COST_SEARCH_HPP
#define SIMPLE_UTILITY_GRAPH_UNIFORM_COST_SEARCH_HPP

#pragma once

#include "Simple-Utility/graph/Traverse.hpp"
#include "Simple-Utility/graph/mixins/queue/std_priority_queue.hpp"
#include "Simple-Utility/graph/mixins/tracker/std_unordered_map.hpp"

namespace sl::graph::ucs
{
	template <concepts::ranked_node Node>
	using NodeFactory = CommonNodeFactory<Node>;

	template <
		class View,
		concepts::ranked_node Node = CommonRankedNode<edge::vertex_t<view::edge_t<View>>, edge::weight_t<view::edge_t<View>>>,
		concepts::node_factory_for<Node, view::edge_t<View>> NodeFactory = NodeFactory<Node>,
		concepts::tracker_for<node::vertex_t<Node>> Tracker = tracker::CommonHashMap<node::vertex_t<Node>>>
		requires concepts::view_for<View, Node>
	using BasicTraverser = Traverser<
		View,
		detail::BasicTraverseDriver<
			Node,
			detail::BasicState<Node, queue::CommonPriorityQueue<Node>>,
			Tracker,
			NodeFactory>>;
}

#endif
