//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_NODE_HPP
#define SIMPLE_UTILITY_GRAPH_NODE_HPP

#pragma once

#include "Simple-Utility/concepts/stl_extensions.hpp"
#include "Simple-Utility/graph/Common.hpp"

namespace sl::graph::node::detail
{
	struct vertex_fn
	{
		constexpr auto& operator ()(const auto& node) const noexcept
			requires concepts::vertex<std::remove_cvref_t<decltype(node.vertex)>>
		{
			return node.vertex;
		}

		constexpr decltype(auto) operator ()(const auto& node) const noexcept
			requires concepts::vertex<std::remove_cvref_t<decltype(node.vertex())>>
		{
			return node.vertex();
		}

		constexpr decltype(auto) operator ()(const auto& node) const noexcept
			requires concepts::vertex<std::remove_cvref_t<decltype(vertex(node))>>
		{
			return vertex(node);
		}
	};
}

namespace sl::graph::node
{
	inline constexpr detail::vertex_fn vertex{};
}

namespace sl::graph::concepts
{
	template <class T>
	concept node = sl::concepts::unqualified<T>
					&& std::copyable<T>
					&& std::destructible<T>
					&& feature_category<typename feature_traits<T>::category_type>
					&& vertex<typename feature_traits<T>::vertex_type>
					&& requires(const T& node)
					{
						requires concepts::vertex<std::remove_cvref_t<decltype(node::vertex(node))>>;
					};

	template <class T, class Node>
	concept node_factory_for = sl::concepts::unqualified<T>
							&& node<Node>
							&& std::destructible<T>
							&& requires(T& factory, const Node& node)
							{
								{ factory.make_init_node(node::vertex(node)) } -> std::convertible_to<Node>;
								{ factory.make_successor_node(node, node::vertex(node)) } -> std::convertible_to<Node>;
							};
}
#endif
