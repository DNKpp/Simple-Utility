//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_VIEW_HPP
#define SIMPLE_UTILITY_GRAPH_VIEW_HPP

#pragma once

#include "Simple-Utility/graph/Edge.hpp"
#include "Simple-Utility/graph/Node.hpp"

namespace sl::graph::view
{
	template <class>
	struct traits;
}

namespace sl::graph::concepts
{
	template <class T, class Node>
	concept view_for = node<Node>
						&& sl::concepts::unqualified<T>
						&& requires(const T& view, const Node& node)
						{
							typename view::traits<T>::edge_type;
							requires edge_for<typename view::traits<T>::edge_type, Node>;
							{ view.edges(node) } -> std::ranges::input_range;
							requires std::convertible_to<
								std::ranges::range_value_t<decltype(view.edges(node))>,
								typename view::traits<T>::edge_type>;
						};
}

namespace sl::graph::view
{
	template <class T>
	using edge_t = typename traits<T>::edge_type;

	template <class T>
		requires requires { typename T::edge_type; }
				&& concepts::edge<typename T::edge_type>
	struct traits<T>
	{
		using edge_type = typename T::edge_type;
	};
}

#endif
