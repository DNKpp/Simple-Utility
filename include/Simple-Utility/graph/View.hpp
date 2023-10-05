//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_VIEW_HPP
#define SIMPLE_UTILITY_GRAPH_VIEW_HPP

#pragma once

#include "Simple-Utility/concepts/stl_extensions.hpp"
#include "Simple-Utility/graph/Edge.hpp"
#include "Simple-Utility/graph/Node.hpp"

#include <concepts>
// ReSharper disable once CppUnusedIncludeDirective
#include <ranges>	// std::ranges::input_range, etc.

namespace sl::graph::view
{
	template <typename>
	struct traits;

	template <typename T>
	using edge_t = typename traits<T>::edge_type;

	template <typename T>
		requires requires { typename T::edge_type; }
				&& concepts::edge<typename T::edge_type>
	struct traits<T>
	{
		using edge_type = typename T::edge_type;
	};
}

namespace sl::graph::customize
{
	template <typename>
	struct edges_fn;
}

namespace sl::graph::detail
{
	template <typename View, typename Node>
		requires requires { customize::edges_fn<View>{}; }
				&& std::ranges::input_range<std::invoke_result_t<customize::edges_fn<View>, const View&, const Node&>>
				&& std::convertible_to<
					std::ranges::range_reference_t<std::invoke_result_t<customize::edges_fn<View>, const View&, const Node&>>,
					view::edge_t<View>>
	constexpr decltype(auto) edges(
		const View& view,
		const Node& node,
		const priority_tag<2>
	) noexcept(noexcept(customize::edges_fn<View>{}(view, node)))
	{
		return customize::edges_fn<View>{}(view, node);
	}

	template <typename View, typename Node>
		requires requires(const View& view, const Node& node)
		{
			{ view.edges(node) } -> std::ranges::input_range;
			requires std::convertible_to<
				std::ranges::range_reference_t<decltype(view.edges(node))>,
				view::edge_t<View>>;
		}
	constexpr decltype(auto) edges(const View& view, const Node& node, const priority_tag<1>) noexcept(noexcept(view.edges(node)))
	{
		return view.edges(node);
	}

	template <typename View, typename Node>
		requires requires(const View& view, const Node& node)
		{
			{ edges(view, node) } -> std::ranges::input_range;
			requires std::convertible_to<
				std::ranges::range_reference_t<decltype(edges(view, node))>,
				view::edge_t<View>>;
		}
	constexpr decltype(auto) edges(const View& view, const Node& node, const priority_tag<0>) noexcept(noexcept(edges(view, node)))
	{
		return edges(view, node);
	}

	struct edges_fn
	{
		template <typename View, typename Node>
			requires requires(const View& view, const Node& node, const priority_tag<2> tag)
			{
				{ detail::edges(view, node, tag) } -> std::ranges::input_range;
				requires std::convertible_to<
					std::ranges::range_reference_t<decltype(detail::edges(view, node, tag))>,
					view::edge_t<View>>;
			}
		constexpr decltype(auto) operator ()(
			const View& view,
			const Node& node
		) const noexcept(noexcept(detail::edges(view, node, priority_tag<2>{})))
		{
			return detail::edges(view, node, priority_tag<2>{});
		}
	};
}

namespace sl::graph::view
{
	inline constexpr detail::edges_fn edges{};
}

namespace sl::graph::concepts
{
	template <typename T, typename Node>
	concept view_for = basic_node<Node>
						&& sl::concepts::unqualified<T>
						&& requires(const T& view, const Node& node)
						{
							// fixes compile error on msvc v142
							// ReSharper disable once CppRedundantTemplateKeyword
							typename view::template traits<T>::edge_type;
							requires edge_for<view::edge_t<T>, Node>;
							{ view::edges(view, node) } -> std::ranges::input_range;
							requires std::convertible_to<
								std::ranges::range_value_t<std::invoke_result_t<detail::edges_fn, const T&, const Node&>>,
								view::edge_t<T>>;
						};
}

#endif
