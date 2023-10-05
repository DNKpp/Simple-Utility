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

	template <concepts::ranked_node Node, concepts::heuristic_for<Node> Heuristic, typename NodeFactory>
	class BaseKernel
	{
	public:
		[[nodiscard]]
		explicit BaseKernel() = default;

		[[nodiscard]]
		explicit constexpr BaseKernel(
			Heuristic heuristic,
			NodeFactory nodeFactory
		) noexcept(std::is_nothrow_move_constructible_v<Heuristic>
					&& std::is_nothrow_move_constructible_v<NodeFactory>)
			: m_Heuristic{std::move(heuristic)},
			m_NodeFactory{std::move(nodeFactory)}
		{
		}

		constexpr Node operator ()(const node::vertex_t<Node>& vertex) const
		{
			return std::invoke(
				m_NodeFactory,
				vertex,
				std::invoke(m_Heuristic, vertex));
		}

		[[nodiscard]]
		constexpr const Heuristic& heuristic() const & noexcept
		{
			return m_Heuristic;
		}

		[[nodiscard]]
		constexpr const NodeFactory& node_factory() const & noexcept
		{
			return m_NodeFactory;
		}

	protected:
		SL_UTILITY_NO_UNIQUE_ADDRESS Heuristic m_Heuristic{};
		SL_UTILITY_NO_UNIQUE_ADDRESS NodeFactory m_NodeFactory{};
	};

	template <concepts::ranked_node Node, concepts::heuristic_for<Node> Heuristic, typename NodeFactory>
	class BufferedKernel
		: public BaseKernel<Node, Heuristic, NodeFactory>
	{
	private:
		using Super = BaseKernel<Node, Heuristic, NodeFactory>;

	public:
		using Super::Super;
		using Super::operator();

		template <std::ranges::input_range Edges>
			requires std::convertible_to<
				std::invoke_result_t<
					NodeFactory,
					const Node&,
					std::ranges::range_reference_t<Edges>,
					std::invoke_result_t<Heuristic, const node::vertex_t<Node>>>,
				Node>
		[[nodiscard]]
		constexpr auto operator ()(const Node& current, Edges&& edges) const
		{
			std::vector<Node> results{};
			if constexpr (std::ranges::sized_range<decltype(edges)>)
			{
				results.reserve(std::ranges::size(edges));
			}

			std::ranges::transform(
				std::forward<Edges>(edges),
				std::back_inserter(results),
				[&](const auto& edge)
				{
					return std::invoke(
						Super::m_NodeFactory,
						current,
						edge,
						std::invoke(Super::m_Heuristic, edge::destination(edge)));
				});

			return results;
		}
	};

#ifdef SL_UTILITY_HAS_RANGES_VIEWS
	template <concepts::ranked_node Node, concepts::heuristic_for<Node> Heuristic, typename NodeFactory>
	class LazyKernel
		: public BaseKernel<Node, Heuristic, NodeFactory>
	{
	private:
		using Super = BaseKernel<Node, Heuristic, NodeFactory>;

	public:
		using Super::Super;
		using Super::operator();

		template <std::ranges::input_range Edges>
			requires std::convertible_to<
				std::invoke_result_t<
					NodeFactory,
					const Node&,
					std::ranges::range_reference_t<Edges>,
					std::invoke_result_t<Heuristic, const node::vertex_t<Node>>>,
				Node>
		[[nodiscard]]
		constexpr auto operator ()(const Node& current, Edges&& edges) const
		{
			return std::forward<Edges>(edges)
					| std::views::transform(
						[&](const auto& edge)
						{
							return std::invoke(
								Super::m_NodeFactory,
								current,
								edge,
								std::invoke(Super::m_Heuristic, edge::destination(edge)));
						});
		}
	};

	template <typename Node, typename Heuristic, typename NodeFactory>
	using default_kernel_t = LazyKernel<Node, Heuristic, NodeFactory>;
#else
	template <typename Node, typename Heuristic, typename NodeFactory>
	using default_kernel_t = BufferedKernel<Node, Heuristic, NodeFactory>;
#endif
}

namespace sl::graph::astar
{
	template <concepts::vertex Vertex, concepts::rank Rank>
	struct Node
	{
		using vertex_type = Vertex;
		using rank_type = Rank;

		vertex_type vertex{};
		rank_type cost{};
		rank_type estimatedPendingCost{};

		[[nodiscard]]
		friend constexpr rank_type rank(const Node& node) noexcept(noexcept(node.cost + node.cost))
		{
			detail::check_bounds(node.cost, node.estimatedPendingCost);

			return node.cost + node.estimatedPendingCost;
		}

		[[nodiscard]]
		friend bool operator==(const Node&, const Node&) = default;
	};
}

template <sl::graph::concepts::vertex Vertex, sl::graph::concepts::rank Rank>
struct sl::graph::detail::NodeFactory<sl::graph::astar::Node<Vertex, Rank>>
{
public:
	using node_type = astar::Node<Vertex, Rank>;
	using vertex_type = Vertex;
	using rank_type = Rank;

	[[nodiscard]]
	constexpr node_type operator ()(vertex_type origin, rank_type estimatedPendingCost) const
	{
		node_type node{
			.vertex = std::move(origin),
			.cost = {},
			.estimatedPendingCost = std::move(estimatedPendingCost)
		};
		astar::detail::check_bounds(node.cost, node.estimatedPendingCost);

		return node;
	}

	template <concepts::edge_for<node_type> Edge>
	[[nodiscard]]
	constexpr node_type operator ()(const node_type& current, const Edge& edge, rank_type estimatedPendingCost) const
	{
		astar::detail::check_bounds(current.cost, edge::weight(edge));

		node_type node{
			.vertex = edge::destination(edge),
			.cost = current.cost + edge::weight(edge),
			.estimatedPendingCost = std::move(estimatedPendingCost)
		};
		astar::detail::check_bounds(node.cost, node.estimatedPendingCost);

		return node;
	}
};

namespace sl::graph::astar
{
	template <typename Node>
	struct NodeFactory
		: public graph::detail::NodeFactory<Node>
	{
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
		typename View,
		typename Heuristic,
		concepts::ranked_node Node = Node<edge::vertex_t<view::edge_t<View>>, edge::weight_t<view::edge_t<View>>>,
		concepts::tracker_for<node::vertex_t<Node>> Tracker = tracker::CommonHashMap<node::vertex_t<Node>>>
		requires concepts::view_for<View, Node>
				&& concepts::heuristic_for<Heuristic, Node>
	using Range = IterableTraverser<
		graph::detail::BasicTraverser<
			Node,
			View,
			queue::CommonPriorityQueue<Node>,
			Tracker,
			detail::default_kernel_t<
				Node,
				Heuristic,
				NodeFactory<Node>>>>;
}

#endif
