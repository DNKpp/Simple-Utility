//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_NODE_HPP
#define SIMPLE_UTILITY_GRAPH_NODE_HPP

#pragma once

#include "Simple-Utility/Utility.hpp"
#include "Simple-Utility/concepts/stl_extensions.hpp"
#include "Simple-Utility/graph/Common.hpp"

namespace sl::graph::customize
{
	template <class>
	struct vertex_fn;

	template <class>
	struct rank_fn;
}

namespace sl::graph::node::detail
{
	template <class Node>
		requires requires(const Node& node, customize::rank_fn<Node> fn)
		{
			requires concepts::rank<std::remove_cvref_t<decltype(fn(node))>>;
		}
	constexpr decltype(auto) rank(const Node& node, const priority_tag<3>) noexcept(noexcept(customize::rank_fn<Node>{}(node)))
	{
		return customize::rank_fn<Node>{}(node);
	}

	template <class Node>
		requires requires(const Node& node)
		{
			requires concepts::rank<std::remove_cvref_t<decltype(node.rank)>>;
		}
	constexpr auto& rank(const Node& node, const priority_tag<2>) noexcept
	{
		return node.rank;
	}

	template <class Node>
		requires requires(const Node& node)
		{
			requires concepts::rank<std::remove_cvref_t<decltype(node.rank())>>;
		}
	constexpr decltype(auto) rank(const Node& node, const priority_tag<1>) noexcept(noexcept(node.rank()))
	{
		return node.rank();
	}

	template <class Node>
		requires requires(const Node& node)
		{
			requires concepts::rank<std::remove_cvref_t<decltype(rank(node))>>;
		}
	constexpr decltype(auto) rank(const Node& node, const priority_tag<0>) noexcept(noexcept(rank(node)))
	{
		return rank(node);
	}

	struct rank_fn
	{
		template <class Node>
			requires requires(const Node& node, const priority_tag<3> tag)
			{
				requires concepts::rank<std::remove_cvref_t<decltype(detail::rank(node, tag))>>;
			}
		constexpr decltype(auto) operator ()(const Node& node) const noexcept(noexcept(detail::rank(node, priority_tag<3>{})))
		{
			return detail::rank(node, priority_tag<3>{});
		}
	};
}

namespace sl::graph::node
{
	inline constexpr graph::detail::vertex_fn vertex{};
	inline constexpr detail::rank_fn rank{};
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
									{ factory.make_successor_node(node, {}) } -> std::convertible_to<Node>;
								};

	template <class T, class Other>
	concept compatible_with = node<T>
							&& node<Other>
							&& std::same_as<
								feature_category_t<T>,
								common_feature_category_t<feature_category_t<T>, feature_category_t<Other>>>
							&& std::convertible_to<feature_vertex_t<Other>, feature_vertex_t<T>>;

	template <class T, class Node>
	concept graph_for = sl::concepts::unqualified<T>
						&& node<Node>
						&& requires(const T& graph, const Node& node)
						{
							{ graph.neighbor_infos(node) } -> std::ranges::input_range;
							requires compatible_with<
								Node,
								std::ranges::range_value_t<decltype(graph.neighbor_infos(node))>>;
						};
}
#endif
