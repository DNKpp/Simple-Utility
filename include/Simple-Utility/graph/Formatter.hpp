//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_FORMATTER_HPP
#define SIMPLE_UTILITY_GRAPH_FORMATTER_HPP

#pragma once

#include "Simple-Utility/Config.hpp"
#include "Simple-Utility/graph/Edge.hpp"
#include "Simple-Utility/graph/Node.hpp"

#ifdef SL_UTILITY_HAS_STD_FORMAT

#include <format>

namespace sl::graph
{
	template <concepts::basic_node Node, class Char>
		requires sl::concepts::formattable<node::vertex_t<Node>, Char>
	class NodeFormatter
	{
	public:
		static constexpr auto parse(std::basic_format_parse_context<Char>& ctx) noexcept
		{
			return ctx.begin();
		}

		template <class FormatContext>
		static auto format(const Node& node, FormatContext& ctx)
		{
			return std::format_to(ctx.out(), "vertex: {}", node::vertex(node));
		}
	};

	template <concepts::ranked_node Node, class Char>
		requires sl::concepts::formattable<node::vertex_t<Node>, Char>
				&& sl::concepts::formattable<node::rank_t<Node>, Char>
	class NodeFormatter<Node, Char>
	{
	public:
		static constexpr auto parse(std::basic_format_parse_context<Char>& ctx) noexcept
		{
			return ctx.begin();
		}

		template <class FormatContext>
		static auto format(const Node& node, FormatContext& ctx)
		{
			return std::format_to(ctx.out(), "vertex: {}, rank: {}", node::vertex(node), node::rank(node));
		}
	};

	template <concepts::basic_node Node, class Char>
	class NodeFormatter<decorator::PredecessorNode<Node>, Char>
	{
	public:
		constexpr auto parse(std::basic_format_parse_context<Char>& ctx) noexcept
		{
			return m_Formatter.parse(ctx);
		}

		template <class FormatContext>
		auto format(const decorator::PredecessorNode<Node>& node, FormatContext& ctx) const
		{
			return std::format_to(
				m_Formatter.format(node, ctx),
				", predecessor: {}",
				node.predecessor ? std::format("{}", *node.predecessor) : "null");
		}

	private:
		SL_UTILITY_NO_UNIQUE_ADDRESS NodeFormatter<Node, Char> m_Formatter{};
	};

	template <concepts::basic_node Node, class Char>
	class NodeFormatter<decorator::DepthNode<Node>, Char>
	{
	public:
		constexpr auto parse(std::basic_format_parse_context<Char>& ctx) noexcept
		{
			return m_Formatter.parse(ctx);
		}

		template <class FormatContext>
		auto format(const decorator::DepthNode<Node>& node, FormatContext& ctx) const
		{
			return std::format_to(
				m_Formatter.format(node, ctx),
				", depth: {}",
				node.depth);
		}

	private:
		SL_UTILITY_NO_UNIQUE_ADDRESS NodeFormatter<Node, Char> m_Formatter{};
	};
}

template <sl::graph::concepts::basic_node Node, class Char>
struct std::formatter<Node, Char> // NOLINT(cert-dcl58-cpp)
{
public:
	constexpr auto parse(std::basic_format_parse_context<Char>& ctx)
	{
		return m_Formatter.parse(ctx);
	}

	template <class FormatContext>
	auto format(const Node& node, FormatContext& ctx) const
	{
		auto out = std::format_to(ctx.out(), "{}", "{");
		out = m_Formatter.format(node, ctx);
		return std::format_to(out, "{}", "}");
	}

private:
	SL_UTILITY_NO_UNIQUE_ADDRESS sl::graph::NodeFormatter<Node, Char> m_Formatter{};
};

template <sl::graph::concepts::edge Edge, class Char>
	requires sl::concepts::formattable<sl::graph::edge::vertex_t<Edge>, Char>
struct std::formatter<Edge, Char> // NOLINT(cert-dcl58-cpp)
{
	static constexpr auto parse(std::basic_format_parse_context<Char>& ctx) noexcept
	{
		return ctx.begin();
	}

	template <class FormatContext>
	auto format(const Edge& edge, FormatContext& ctx) const
	{
		return std::format_to(ctx.out(), "{}destination: {}{}", "{", sl::graph::edge::destination(edge), "}");
	}
};

template <sl::graph::concepts::weighted_edge Edge, class Char>
	requires sl::concepts::formattable<sl::graph::edge::vertex_t<Edge>, Char>
			&& sl::concepts::formattable<sl::graph::edge::weight_t<Edge>, Char>
struct std::formatter<Edge, Char> // NOLINT(cert-dcl58-cpp)
{
	static constexpr auto parse(std::basic_format_parse_context<Char>& ctx) noexcept
	{
		return ctx.begin();
	}

	template <class FormatContext>
	auto format(const Edge& edge, FormatContext& ctx) const
	{
		return std::format_to(
			ctx.out(),
			"{}destination: {}, weight: {}{}",
			"{",
			sl::graph::edge::destination(edge),
			sl::graph::edge::weight(edge),
			"}");
	}
};

#endif

#endif
