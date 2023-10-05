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
	struct NodeFactory
		: public detail::NodeFactory<Node>
	{
	};

	template <concepts::vertex Vertex, concepts::rank Rank>
	using CommonNode = CommonRankedNode<Vertex, Rank>;

	template <
		class View,
		concepts::ranked_node Node = CommonNode<edge::vertex_t<view::edge_t<View>>, edge::weight_t<view::edge_t<View>>>,
		concepts::tracker_for<node::vertex_t<Node>> Tracker = tracker::CommonHashMap<node::vertex_t<Node>>>
		requires concepts::view_for<View, Node>
	using Range = IterableTraverser<
		detail::BasicTraverser<
			Node,
			View,
			queue::CommonPriorityQueue<Node>,
			Tracker,
			detail::default_kernel_t<Node, NodeFactory<Node>>>>;
}

#endif
