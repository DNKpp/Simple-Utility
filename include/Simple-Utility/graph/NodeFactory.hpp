//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_NODE_FACTORY_HPP
#define SIMPLE_UTILITY_GRAPH_NODE_FACTORY_HPP

#pragma once

#include "Simple-Utility/graph/Edge.hpp"
#include "Simple-Utility/graph/Node.hpp"

namespace sl::graph::concepts
{
	template <class T, class Node, class Edge>
	concept node_factory_for = sl::concepts::unqualified<T>
								&& node<Node>
								&& edge_for<Edge, Node>
								&& std::destructible<T>
								&& requires(T& factory, const Node& node, const Edge& edge)
								{
									{ factory.make_init_node(node::vertex(node)) } -> std::convertible_to<Node>;
									{ factory.make_successor_node(node, edge) } -> std::convertible_to<Node>;
								};
}

#endif
