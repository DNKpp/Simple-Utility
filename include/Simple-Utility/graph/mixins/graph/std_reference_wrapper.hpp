//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_MIXINS_GRAPH_STD_REFERENCE_WRAPPER_HPP
#define SIMPLE_UTILITY_GRAPH_MIXINS_GRAPH_STD_REFERENCE_WRAPPER_HPP

#pragma once

#include <functional>

#include "Simple-Utility/graph/Graph.hpp"

template <typename Graph>
	requires sl::graph::concepts::basic_graph<std::remove_const_t<Graph>>
struct sl::graph::graph::traits<std::reference_wrapper<Graph>>
{
	using edge_type = edge_t<std::remove_const_t<Graph>>;
	using vertex_type = vertex_t<std::remove_const_t<Graph>>;
};

template <typename Graph>
	requires sl::graph::concepts::basic_graph<std::remove_const_t<Graph>>
struct sl::graph::customize::out_edges_fn<std::reference_wrapper<Graph>>
{
	using edge_type = graph::edge_t<std::reference_wrapper<Graph>>;
	using vertex_type = graph::vertex_t<std::reference_wrapper<Graph>>;

	[[nodiscard]]
	constexpr auto operator ()(const Graph& graph, const vertex_type& current) const
	{
		return graph::out_edges(graph, current);
	}
};

#endif
