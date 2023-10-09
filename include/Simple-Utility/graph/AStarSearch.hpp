//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_A_STAR_SEARCH_HPP
#define SIMPLE_UTILITY_GRAPH_A_STAR_SEARCH_HPP

#pragma once

#include "Simple-Utility/graph/Traverse.hpp"
#include "Simple-Utility/graph/mixins/queue/std_priority_queue.hpp"
#include "Simple-Utility/graph/mixins/tracker/std_unordered_map.hpp"

namespace sl::graph::concepts
{
	template <typename T, typename Node>
	concept heuristic_for = ranked_node<Node>
							&& sl::concepts::unqualified<T>
							&& std::movable<T>
							&& std::invocable<const T&, node::vertex_t<Node>>
							&& std::convertible_to<std::invoke_result_t<T, node::vertex_t<Node>>, node::rank_t<Node>>;
}

namespace sl::graph::astar::detail
{
	template <concepts::rank Rank>
	constexpr void check_bounds(const Rank& base, const Rank& increase) noexcept
	{
		assert(0 <= base && "base must be non-negative.");
		assert(0 <= increase && "increase must be non-negative.");
		assert(increase <= std::numeric_limits<Rank>::max() - base && "Rank is about to overflow.");
	}
}

namespace sl::graph::astar
{
	template <concepts::vertex Vertex, concepts::rank Rank>
	struct CommonNode
	{
		using vertex_type = Vertex;
		using rank_type = Rank;

		vertex_type vertex{};
		rank_type cost{};
		rank_type estimatedPendingCost{};

		[[nodiscard]]
		friend constexpr rank_type rank(const CommonNode& node) noexcept(noexcept(node.cost + node.cost))
		{
			detail::check_bounds(node.cost, node.estimatedPendingCost);

			return node.cost + node.estimatedPendingCost;
		}

		[[nodiscard]]
		friend bool operator==(const CommonNode&, const CommonNode&) = default;
	};

	template <concepts::ranked_node Node, concepts::heuristic_for<Node> Heuristic>
	struct NodeFactory;

	template <concepts::vertex Vertex, concepts::rank Rank, concepts::heuristic_for<CommonNode<Vertex, Rank>> Heuristic>
	struct NodeFactory<CommonNode<Vertex, Rank>, Heuristic>
	{
	public:
		using node_type = CommonNode<Vertex, Rank>;
		using vertex_type = Vertex;
		using rank_type = Rank;

		[[nodiscard]]
		explicit NodeFactory(Heuristic heuristic) noexcept(std::is_nothrow_move_constructible_v<Heuristic>)
			: m_Heuristic{std::move(heuristic)}
		{
		}

		[[nodiscard]]
		constexpr node_type operator ()(const vertex_type& origin) const
		{
			node_type node{
				.vertex = origin,
				.cost = {0},
				.estimatedPendingCost = std::invoke(m_Heuristic, origin)
			};
			detail::check_bounds(node.cost, node.estimatedPendingCost);

			return node;
		}

		template <concepts::edge_for<node_type> Edge>
		[[nodiscard]]
		constexpr node_type operator ()(const node_type& current, const Edge& edge) const
		{
			detail::check_bounds(current.cost, edge::weight(edge));

			node_type node{
				.vertex = edge::destination(edge),
				.cost = current.cost + edge::weight(edge),
				.estimatedPendingCost = std::invoke(m_Heuristic, edge::destination(edge))
			};
			detail::check_bounds(node.cost, node.estimatedPendingCost);

			return node;
		}

	private:
		SL_UTILITY_NO_UNIQUE_ADDRESS Heuristic m_Heuristic;
	};

	template <typename Heuristic>
	struct NodeFactoryTemplate
	{
		template <concepts::ranked_node Node>
		using type = NodeFactory<Node, Heuristic>;
	};

	template <concepts::ranked_node Node, concepts::heuristic_for<Node> Heuristic>
		requires requires { typename decorator::NodeFactory<Node, NodeFactoryTemplate<Heuristic>::template type>::node_type; }
	struct NodeFactory<Node, Heuristic>
		: public decorator::NodeFactory<Node, NodeFactoryTemplate<Heuristic>::template type>
	{
	private:
		using Super = decorator::NodeFactory<Node, NodeFactoryTemplate<Heuristic>::template type>;

	public:
		using Super::Super;
		using Super::operator ();
	};

	template <concepts::vertex Vertex, std::invocable<Vertex, Vertex> Strategy>
	class SingleDestinationHeuristic
	{
	public:
		using vertex_type = Vertex;

		[[nodiscard]]
		explicit constexpr SingleDestinationHeuristic(
			Vertex destination
		) noexcept(std::is_nothrow_move_constructible_v<Vertex>
					&& std::is_nothrow_default_constructible_v<Strategy>)
			: m_Destination{std::move(destination)}
		{
		}

		[[nodiscard]]
		explicit constexpr SingleDestinationHeuristic(
			Vertex destination,
			Strategy strategy
		) noexcept(std::is_nothrow_move_constructible_v<Vertex>
					&& std::is_nothrow_move_constructible_v<Strategy>)
			: m_Destination{std::move(destination)},
			m_Strategy{std::move(strategy)}
		{
		}

		[[nodiscard]]
		constexpr auto operator ()(const Vertex& current) const noexcept(std::is_nothrow_invocable_v<Strategy, Vertex, Vertex>)
		{
			return std::invoke(m_Strategy, m_Destination, current);
		}

	private:
		Vertex m_Destination{};
		Strategy m_Strategy{};
	};

	template <
		concepts::basic_graph View,
		typename Heuristic,
		concepts::ranked_node Node = CommonNode<edge::vertex_t<view::edge_t<View>>, edge::weight_t<view::edge_t<View>>>,
		concepts::tracker_for<node::vertex_t<Node>> Tracker = tracker::CommonHashMap<node::vertex_t<Node>>>
		requires concepts::heuristic_for<Heuristic, Node>
	using Range = IterableTraverser<
		graph::detail::BasicTraverser<
			Node,
			View,
			queue::CommonPriorityQueue<Node>,
			Tracker,
			graph::detail::default_explorer_t<
				Node,
				NodeFactory<Node, Heuristic>>>>;
}

#endif
