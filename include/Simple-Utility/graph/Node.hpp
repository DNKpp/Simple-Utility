//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_NODE_HPP
#define SIMPLE_UTILITY_GRAPH_NODE_HPP

#pragma once

#include "Simple-Utility/Config.hpp"
#include "Simple-Utility/Utility.hpp"
#include "Simple-Utility/concepts/stl_extensions.hpp"
#include "Simple-Utility/graph/Common.hpp"

#include <optional>

namespace sl::graph::customize
{
	template <class>
	struct vertex_fn;

	template <class>
	struct rank_fn;
}

namespace sl::graph::detail
{
	template <class Node>
		requires requires(const Node& node, customize::vertex_fn<Node> fn)
		{
			requires concepts::vertex<std::remove_cvref_t<decltype(fn(node))>>;
		}
	constexpr decltype(auto) vertex(const Node& node, const priority_tag<3>) noexcept(noexcept(customize::vertex_fn<Node>{}(node)))
	{
		return customize::vertex_fn<Node>{}(node);
	}

	template <class Node>
		requires requires(const Node& node)
		{
			requires concepts::vertex<std::remove_cvref_t<decltype(node.vertex)>>;
		}
	constexpr auto& vertex(const Node& node, const priority_tag<2>) noexcept
	{
		return node.vertex;
	}

	template <class Node>
		requires requires(const Node& node)
		{
			requires concepts::vertex<std::remove_cvref_t<decltype(node.vertex())>>;
		}
	constexpr decltype(auto) vertex(const Node& node, const priority_tag<1>) noexcept(noexcept(node.vertex()))
	{
		return node.vertex();
	}

	template <class Node>
		requires requires(const Node& node)
		{
			requires concepts::vertex<std::remove_cvref_t<decltype(vertex(node))>>;
		}
	constexpr decltype(auto) vertex(const Node& node, const priority_tag<0>) noexcept(noexcept(vertex(node)))
	{
		return vertex(node);
	}

	struct vertex_fn
	{
		template <class Node>
			requires requires(const Node& node, const priority_tag<3> tag)
			{
				requires concepts::vertex<std::remove_cvref_t<decltype(detail::vertex(node, tag))>>;
			}
		constexpr decltype(auto) operator ()(const Node& node) const noexcept(noexcept(detail::vertex(node, priority_tag<3>{})))
		{
			return detail::vertex(node, priority_tag<3>{});
		}
	};

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
	inline constexpr detail::vertex_fn vertex{};
	inline constexpr detail::rank_fn rank{};

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
				&& concepts::readable_rank_type<T>
	struct traits<T>
	{
		using vertex_type = typename T::vertex_type;
		using rank_type = typename T::rank_type;
	};
}

namespace sl::graph::concepts
{
	template <class T>
	concept node = sl::concepts::unqualified<T>
					&& std::copyable<T>
					&& std::destructible<T>
					&& vertex<typename node::traits<T>::vertex_type>
					&& requires(const T& node)
					{
						// fixes compile error on msvc v142
						// ReSharper disable once CppRedundantTemplateKeyword
						{ node::vertex(node) } -> std::convertible_to<typename node::template traits<T>::vertex_type>;
					};

	template <class T>
	concept ranked_node = node<T>
						&& rank<typename node::traits<T>::rank_type>
						&& requires(const T& node)
						{
							// fixes compile error on msvc v142
							// ReSharper disable once CppRedundantTemplateKeyword
							{ node::rank(node) } -> std::convertible_to<typename node::template traits<T>::rank_type>;
						};
}

namespace sl::graph::node
{
	template <concepts::node Node>
	using vertex_t = typename traits<Node>::vertex_type;

	template <concepts::ranked_node Node>
	using rank_t = typename traits<Node>::rank_type;
}

namespace sl::graph
{
	template <concepts::vertex Vertex>
	struct CommonBasicNode
	{
		using vertex_type = Vertex;

		vertex_type vertex;

		[[nodiscard]]
		friend bool operator==(const CommonBasicNode&, const CommonBasicNode&) = default;
	};

	template <concepts::vertex Vertex, concepts::rank Rank>
	struct CommonRankedNode
	{
		using vertex_type = Vertex;
		using rank_type = Rank;

		vertex_type vertex;
		rank_type rank;

		[[nodiscard]]
		friend bool operator==(const CommonRankedNode&, const CommonRankedNode&) = default;
	};

	template <concepts::node Node>
	struct PredecessorNodeDecorator
		: public Node
	{
		using vertex_type = node::vertex_t<Node>;

		std::optional<vertex_type> predecessor{};

		[[nodiscard]]
		friend bool operator==(const PredecessorNodeDecorator&, const PredecessorNodeDecorator&) = default;
	};
}

#ifdef SL_UTILITY_HAS_STD_FORMAT

#include <format>

template <sl::graph::concepts::node Node, class Char>
	requires sl::concepts::formattable<sl::graph::node::vertex_t<Node>, Char>
struct std::formatter<Node, Char> // NOLINT(cert-dcl58-cpp)
{
	static constexpr auto parse(std::basic_format_parse_context<Char>& ctx) noexcept
	{
		return ctx.begin();
	}

	template <class FormatContext>
	auto format(const Node& node, FormatContext& fc) const
	{
		return std::format_to(fc.out(), "{}vertex: {}{}", "{", sl::graph::node::vertex(node), "}");
	}
};

template <sl::graph::concepts::ranked_node Node, class Char>
	requires sl::concepts::formattable<sl::graph::node::vertex_t<Node>, Char>
			&& sl::concepts::formattable<sl::graph::node::rank_t<Node>, Char>
struct std::formatter<Node, Char> // NOLINT(cert-dcl58-cpp)
{
	static constexpr auto parse(std::basic_format_parse_context<Char>& ctx) noexcept
	{
		return ctx.begin();
	}

	template <class FormatContext>
	auto format(const Node& node, FormatContext& fc) const
	{
		return std::format_to(
			fc.out(),
			"{}vertex: {}, rank: {}{}",
			"{",
			sl::graph::node::vertex(node),
			sl::graph::node::rank(node),
			"}");
	}
};

#endif

#endif
