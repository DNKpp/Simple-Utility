//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_NODE_FACTORY_HPP
#define SIMPLE_UTILITY_GRAPH_NODE_FACTORY_HPP

#pragma once

#include "Simple-Utility/graph/Edge.hpp"
#include "Simple-Utility/graph/Node.hpp"

namespace sl::graph::concepts
{
	template <class T, class Node, class Edge>
	concept node_factory_for = sl::concepts::unqualified<T>
								&& node<Node>
								&& edge_for<Edge, Node>
								&& std::destructible<T>
								&& requires(T& factory, const Node& node, const Edge& edge)
								{
									{ factory.make_init_node(node::vertex(node)) } -> std::convertible_to<Node>;
									{ factory.make_successor_node(node, edge) } -> std::convertible_to<Node>;
								};
}

namespace sl::graph
{
	template <concepts::node Node>
	class CommonNodeFactory
	{
	public:
		using node_type = Node;
		using vertex_type = node::vertex_t<node_type>;

		[[nodiscard]]
		static constexpr node_type make_init_node(vertex_type origin)
		{
			return node_type{.vertex = std::move(origin)};
		}

		template <concepts::edge_for<node_type> Edge>
		[[nodiscard]]
		static constexpr node_type make_successor_node([[maybe_unused]] const node_type& current, const Edge& edge)
		{
			return node_type{.vertex = edge::destination(edge)};
		}
	};

	template <concepts::ranked_node Node>
	class CommonNodeFactory<Node>
	{
	public:
		using node_type = Node;
		using vertex_type = node::vertex_t<node_type>;

		[[nodiscard]]
		static constexpr node_type make_init_node(vertex_type origin)
		{
			return node_type{.vertex = std::move(origin), .rank = 0};
		}

		template <concepts::edge_for<node_type> Edge>
		[[nodiscard]]
		static constexpr node_type make_successor_node(const node_type& current, const Edge& edge)
		{
			return node_type{
				.vertex = edge::destination(edge),
				.rank = node::rank(current) + edge::weight(edge)
			};
		}
	};

	template <concepts::node Node>
	class CommonNodeFactory<PredecessorNodeDecorator<Node>>
		: public CommonNodeFactory<Node>
	{
	private:
		using Super = CommonNodeFactory<Node>;

	public:
		using node_type = PredecessorNodeDecorator<Node>;
		using vertex_type = node::vertex_t<node_type>;

		using Super::Super;

		[[nodiscard]]
		constexpr node_type make_init_node(vertex_type origin)
		{
			return {
				{static_cast<Super&>(*this).make_init_node(std::move(origin))},
				std::nullopt
			};
		}

		template <concepts::edge_for<node_type> Edge>
		[[nodiscard]]
		constexpr node_type make_successor_node(const node_type& current, const Edge& edge)
		{
			return {
				{static_cast<Super&>(*this).make_successor_node(current, edge)},
				node::vertex(current)
			};
		}
	};
}

#endif
