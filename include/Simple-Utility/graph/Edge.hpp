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
}

#endif
