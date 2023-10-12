//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_BREADTH_FIRST_SEARCH_HPP
#define SIMPLE_UTILITY_GRAPH_BREADTH_FIRST_SEARCH_HPP

#pragma once

#include "Simple-Utility/graph/Traverse.hpp"
#include "Simple-Utility/graph/mixins/queue/std_queue.hpp"
#include "Simple-Utility/graph/mixins/tracker/std_unordered_map.hpp"

namespace sl::graph::dfs
{
	template <concepts::basic_node Node>
	struct NodeFactory
		: public detail::NodeFactory<Node>
	{
	};

	template <concepts::vertex Vertex>
	using CommonNode = CommonBasicNode<Vertex>;

	template <
		concepts::basic_graph Graph,
		concepts::basic_node Node = CommonNode<edge::vertex_t<graph::edge_t<Graph>>>,
		concepts::tracker_for<node::vertex_t<Node>> Tracker = tracker::CommonHashMap<node::vertex_t<Node>>>
		requires (!concepts::ranked_node<Node>)
	using Range = IterableTraverser<
		detail::BasicTraverser<
			Node,
			Graph,
			queue::CommonQueue<Node>,
			Tracker,
			detail::default_explorer_t<Node, NodeFactory<Node>>>>;
}

#endif
