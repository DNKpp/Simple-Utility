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
#include "Simple-Utility/graph/Node.hpp"

namespace sl::graph::customize
{
	template <class>
	struct weight_fn;

	template <class>
	struct destination_fn;
}

namespace sl::graph::edge::detail
{
	template <class Node>
		requires requires(const Node& node, customize::destination_fn<Node> fn)
		{
			requires concepts::vertex<std::remove_cvref_t<decltype(fn(node))>>;
		}
	constexpr decltype(auto) destination(
		const Node& node,
		const priority_tag<3>
	) noexcept(noexcept(customize::destination_fn<Node>{}(node)))
	{
		return customize::destination_fn<Node>{}(node);
	}

	template <class Node>
		requires requires(const Node& node)
		{
			requires concepts::vertex<std::remove_cvref_t<decltype(node.destination)>>;
		}
	constexpr auto& destination(const Node& node, const priority_tag<2>) noexcept
	{
		return node.destination;
	}

	template <class Node>
		requires requires(const Node& node)
		{
			requires concepts::vertex<std::remove_cvref_t<decltype(node.destination())>>;
		}
	constexpr decltype(auto) destination(const Node& node, const priority_tag<1>) noexcept(noexcept(node.destination()))
	{
		return node.destination();
	}

	template <class Node>
		requires requires(const Node& node)
		{
			requires concepts::vertex<std::remove_cvref_t<decltype(destination(node))>>;
		}
	constexpr decltype(auto) destination(const Node& node, const priority_tag<0>) noexcept(noexcept(destination(node)))
	{
		return destination(node);
	}

	struct destination_fn
	{
		template <class Node>
			requires requires(const Node& node, const priority_tag<3> tag)
			{
				requires concepts::vertex<std::remove_cvref_t<decltype(detail::destination(node, tag))>>;
			}
		constexpr decltype(auto) operator ()(const Node& node) const noexcept(noexcept(detail::destination(node, priority_tag<3>{})))
		{
			return detail::destination(node, priority_tag<3>{});
		}
	};

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
	inline constexpr detail::destination_fn destination{};
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
						// fixes compile error on msvc v142
						// ReSharper disable once CppRedundantTemplateKeyword
						{ edge::destination(edge) } -> std::convertible_to<typename edge::template traits<T>::vertex_type>;
					};

	template <class T>
	concept weighted_edge = edge<T>
							&& weight<typename edge::traits<T>::weight_type>
							&& requires(const T& edge)
							{
								// fixes compile error on msvc v142
								// ReSharper disable once CppRedundantTemplateKeyword
								{ edge::weight(edge) } -> std::convertible_to<typename edge::template traits<T>::weight_type>;
							};
}

namespace sl::graph::edge
{
	template <concepts::edge Edge>
	using vertex_t = typename traits<Edge>::vertex_type;

	template <concepts::edge Edge>
	using weight_t = typename traits<Edge>::weight_type;
}

namespace sl::graph::concepts
{
	template <class Edge, class Node>
	concept edge_for = node<Node>
						&& edge<Edge>
						&& std::same_as<edge::vertex_t<Edge>, node::vertex_t<Node>>
						&& (!ranked_node<Node>
							|| requires(const Edge& edge)
							{
								requires weighted_edge<Edge>;
								{ edge::weight(edge) } -> std::convertible_to<node::rank_t<Node>>;
							});
}

namespace sl::graph
{
	template <concepts::vertex Vertex>
	struct CommonBasicEdge
	{
		using vertex_type = Vertex;

		vertex_type destination;

		[[nodiscard]]
		friend bool operator==(const CommonBasicEdge&, const CommonBasicEdge&) = default;
	};

	template <concepts::vertex Vertex, concepts::weight Weight>
	struct CommonWeightedEdge
	{
		using vertex_type = Vertex;
		using weight_type = Weight;

		vertex_type destination;
		weight_type weight;

		[[nodiscard]]
		friend bool operator==(const CommonWeightedEdge&, const CommonWeightedEdge&) = default;
	};
}

#endif
