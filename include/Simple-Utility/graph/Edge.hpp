//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_EDGE_HPP
#define SIMPLE_UTILITY_GRAPH_EDGE_HPP

#pragma once

#include "Simple-Utility/Utility.hpp"
#include "Simple-Utility/concepts/stl_extensions.hpp"
#include "Simple-Utility/graph/Common.hpp"

namespace sl::graph::customize
{
	template <class>
	struct weight_fn;
}

namespace sl::graph::edge::detail
{
	template <class Edge>
		requires requires(const Edge& node, customize::weight_fn<Edge> fn)
		{
			requires concepts::weight<std::remove_cvref_t<decltype(fn(node))>>;
		}
	constexpr decltype(auto) weight(const Edge& node, const priority_tag<3>) noexcept(noexcept(customize::weight_fn<Edge>{}(node)))
	{
		return customize::weight_fn<Edge>{}(node);
	}

	template <class Edge>
		requires requires(const Edge& node)
		{
			requires concepts::weight<std::remove_cvref_t<decltype(node.weight)>>;
		}
	constexpr auto& weight(const Edge& node, const priority_tag<2>) noexcept
	{
		return node.weight;
	}

	template <class Edge>
		requires requires(const Edge& node)
		{
			requires concepts::weight<std::remove_cvref_t<decltype(node.weight())>>;
		}
	constexpr decltype(auto) weight(const Edge& node, const priority_tag<1>) noexcept(noexcept(node.weight()))
	{
		return node.weight();
	}

	template <class Edge>
		requires requires(const Edge& node)
		{
			requires concepts::weight<std::remove_cvref_t<decltype(weight(node))>>;
		}
	constexpr decltype(auto) weight(const Edge& node, const priority_tag<0>) noexcept(noexcept(weight(node)))
	{
		return weight(node);
	}

	struct weight_fn
	{
		template <class Edge>
			requires requires(const Edge& node, const priority_tag<3> tag)
			{
				requires concepts::weight<std::remove_cvref_t<decltype(detail::weight(node, tag))>>;
			}
		constexpr decltype(auto) operator ()(const Edge& node) const noexcept(noexcept(detail::weight(node, priority_tag<3>{})))
		{
			return detail::weight(node, priority_tag<3>{});
		}
	};
}

namespace sl::graph::edge
{
	inline constexpr graph::detail::vertex_fn vertex{};
	inline constexpr detail::weight_fn weight{};

	template <class>
	struct traits;

	template <class T>
		requires concepts::readable_vertex_type<T>
	struct traits<T>
	{
		using vertex_type = typename T::vertex_type;
	};

	template <class T>
		requires concepts::readable_vertex_type<T>
				&& concepts::readable_weight_type<T>
	struct traits<T>
	{
		using vertex_type = typename T::vertex_type;
		using weight_type = typename T::weight_type;
	};
}

namespace sl::graph::concepts
{
	template <class T>
	concept edge = sl::concepts::unqualified<T>
					&& std::copyable<T>
					&& std::destructible<T>
					&& vertex<typename edge::traits<T>::vertex_type>
					&& requires(const T& edge)
					{
						{ edge::vertex(edge) } -> std::convertible_to<typename edge::traits<T>::vertex_type>;
					};

	template <class T>
	concept weighted_edge = edge<T>
							&& weight<typename edge::traits<T>::weight_type>
							&& requires(const T& edge)
							{
								{ edge::weight(edge) } -> std::convertible_to<typename edge::traits<T>::weight_type>;
							};
}

namespace sl::graph::edge
{
	template <concepts::edge Edge>
	using vertex_t = typename traits<Edge>::vertex_type;

	template <concepts::edge Edge>
	using weight_t = typename traits<Edge>::weight_type;
}

#endif
