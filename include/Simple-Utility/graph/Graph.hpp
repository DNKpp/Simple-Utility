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

namespace sl::graph::graph
{
	template <typename>
	struct traits;

	template <typename T>
	using edge_t = typename traits<T>::edge_type;

	template <typename T>
	using vertex_t = typename traits<T>::vertex_type;

	template <typename T>
		requires concepts::readable_vertex_type<T>
				&& requires { requires concepts::edge<typename T::edge_type>; }
	struct traits<T>
	{
		using vertex_type = typename T::vertex_type;
		using edge_type = typename T::edge_type;
	};
}

namespace sl::graph::customize
{
	template <typename>
	struct out_edges_fn;
}

namespace sl::graph::detail
{
	template <typename Graph>
		requires requires { customize::out_edges_fn<Graph>{}; }
				&& std::ranges::input_range<std::invoke_result_t<
					customize::out_edges_fn<Graph>, const Graph&, const graph::vertex_t<Graph>&>>
				&& std::convertible_to<
					std::ranges::range_reference_t<std::invoke_result_t<
						customize::out_edges_fn<Graph>, const Graph&, const graph::vertex_t<Graph>&>>,
					graph::edge_t<Graph>>
	constexpr decltype(auto) out_edges(
		const Graph& graph,
		const graph::vertex_t<Graph>& vertex,
		const priority_tag<2>
	) noexcept(noexcept(customize::out_edges_fn<Graph>{}(graph, vertex)))
	{
		return customize::out_edges_fn<Graph>{}(graph, vertex);
	}

	// pleases msvc v142
	// ReSharper disable CppRedundantTemplateKeyword
	// ReSharper disable CppRedundantTypenameKeyword
	template <typename Graph>
		requires requires(const Graph& graph, const typename graph::template vertex_t<Graph>& vertex)
		{
			{ graph.out_edges(vertex) } -> std::ranges::input_range;
			requires std::convertible_to<
				std::ranges::range_reference_t<decltype(graph.out_edges(vertex))>,
				typename graph::template edge_t<Graph>>;
		}
	constexpr decltype(auto) out_edges(
		const Graph& graph,
		const graph::vertex_t<Graph>& vertex,
		const priority_tag<1>
	) noexcept(noexcept(graph.out_edges(vertex)))
	{
		return graph.out_edges(vertex);
	}

	template <typename Graph>
		requires requires(const Graph& graph, const typename graph::template vertex_t<Graph>& vertex)
		{
			{ out_edges(graph, vertex) } -> std::ranges::input_range;
			requires std::convertible_to<
				std::ranges::range_reference_t<decltype(out_edges(graph, vertex))>,
				typename graph::template edge_t<Graph>>;
		}
	constexpr decltype(auto) out_edges(
		const Graph& graph,
		const graph::vertex_t<Graph>& vertex,
		const priority_tag<0>
	) noexcept(noexcept(out_edges(graph, vertex)))
	{
		return out_edges(graph, vertex);
	}

	struct out_edges_fn
	{
		template <typename Graph>
			requires requires(const Graph& graph, const typename graph::template vertex_t<Graph>& vertex, const priority_tag<2> tag)
			{
				{ detail::out_edges(graph, vertex, tag) } -> std::ranges::input_range;
				requires std::convertible_to<
					std::ranges::range_reference_t<decltype(detail::out_edges(graph, vertex, tag))>,
					typename graph::template edge_t<Graph>>;
			}
		constexpr decltype(auto) operator ()(
			const Graph& graph,
			const graph::vertex_t<Graph>& vertex
		) const noexcept(noexcept(detail::out_edges(graph, vertex, priority_tag<2>{})))
		{
			return detail::out_edges(graph, vertex, priority_tag<2>{});
		}
	};

	// ReSharper restore CppRedundantTemplateKeyword
	// ReSharper restore CppRedundantTypenameKeyword
}

namespace sl::graph::graph
{
	inline constexpr detail::out_edges_fn out_edges{};
}

namespace sl::graph::concepts
{
	template <typename T>
	concept basic_graph = sl::concepts::unqualified<T>
						&& std::destructible<T>
						&& requires(const T& graph)
						{
							// fixes compile error on msvc v142
							// ReSharper disable CppRedundantTemplateKeyword
							// ReSharper disable CppRedundantTypenameKeyword
							requires vertex<typename graph::template traits<T>::vertex_type>;
							requires edge<typename graph::template traits<T>::edge_type>;
							{ graph::out_edges(graph, std::declval<const typename graph::template vertex_t<T>&>()) } -> std::ranges::input_range;
							requires std::convertible_to<
								std::ranges::range_value_t<std::invoke_result_t<
									detail::out_edges_fn, const T&, const typename graph::template vertex_t<T>&>>,
								typename graph::template edge_t<T>>;
							// ReSharper restore CppRedundantTemplateKeyword
							// ReSharper restore CppRedundantTypenameKeyword
						};
}

#endif
