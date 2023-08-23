//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_TRAVERSE_HPP
#define SIMPLE_UTILITY_GRAPH_TRAVERSE_HPP

#pragma once

#include "Simple-Utility/Config.hpp"
#include "Simple-Utility/concepts/stl_extensions.hpp"
#include "Simple-Utility/functional/Tuple.hpp"
#include "Simple-Utility/graph/Common.hpp"
#include "Simple-Utility/graph/Node.hpp"
#include "Simple-Utility/graph/Queue.hpp"
#include "Simple-Utility/graph/Tracker.hpp"

#include <array>
#include <cassert>
#include <concepts>
#include <forward_list>
#include <optional>
#include <ranges>
#include <type_traits>

namespace sl::graph::detail
{
	template <concepts::node Node, concepts::queue_for<Node> QueuingStrategy>
	class BasicState
	{
	public:
		using node_type = Node;
		using queue_type = QueuingStrategy;

		[[nodiscard]]
		constexpr explicit BasicState() = default;

		[[nodiscard]]
		constexpr explicit BasicState(queue_type queue) noexcept(std::is_nothrow_move_constructible_v<queue_type>)
			: m_Queue{std::move(queue)}
		{
			assert(queue::empty(m_Queue) && "Queue already contains elements.");
		}

		[[nodiscard]]
		constexpr const queue_type& queue() const noexcept
		{
			return m_Queue;
		}

		template <class Neighbors>
			requires std::convertible_to<std::ranges::range_value_t<Neighbors>, node_type>
		[[nodiscard]]
		constexpr std::optional<node_type> next(Neighbors&& neighbors)
		{
			queue::insert(m_Queue, std::forward<Neighbors>(neighbors));
			if (!queue::empty(m_Queue))
			{
				return queue::next(m_Queue);
			}

			return std::nullopt;
		}

	private:
		QueuingStrategy m_Queue{};
	};

	template <class T, class Node>
	concept state_for = sl::concepts::unqualified<T>
						&& std::destructible<T>
						&& concepts::node<Node>
						&& requires(T& state, const std::forward_list<Node>& inputRange)
						{
							{ !state.next(inputRange) } -> std::convertible_to<bool>;
							{ *state.next(inputRange) } -> std::convertible_to<Node>;
						};

#if (defined(__clang__) && __clang_major__ < 16) \
	|| (defined(__GNUG__) && __GNUG__ < 12)

	template <class Source, class Node>
	[[nodiscard]]
	constexpr std::vector<Node> filter_transform(const Source& neighbors, auto& tracker, auto& nodeFactory, const Node& currentNode)
	{
		std::vector<Node> nodes{};
		if constexpr (std::ranges::sized_range<Source>)
		{
			nodes.reserve(std::ranges::size(neighbors));
		}

		for (const auto& info : neighbors)
		{
			if (tracker::set_discovered(tracker, node::vertex(info)))
			{
				nodes.emplace_back(nodeFactory.make_successor_node(currentNode, info));
			}
		}

		return nodes;
	};

#else

	template <class Source, class Node>
	[[nodiscard]]
	constexpr auto filter_transform(Source&& neighbors, auto& tracker, auto& nodeFactory, const Node& currentNode)
	{
		return std::forward<Source>(neighbors)
				| std::views::filter([&](const auto& info) { return tracker::set_discovered(tracker, node::vertex(info)); })
				| std::views::transform([&](const auto& info) { return nodeFactory.make_successor_node(currentNode, info); });
	};

#endif

	template <
		concepts::node Node,
		state_for<Node> StateStrategy,
		concepts::tracker_for<feature_vertex_t<Node>> TrackingStrategy,
		concepts::node_factory_for<Node> NodeFactoryStrategy>
	class BasicTraverseDriver
	{
	public:
		using node_type = Node;
		using vertex_type = feature_vertex_t<Node>;
		using state_type = StateStrategy;
		using tracker_type = TrackingStrategy;
		using node_factory_type = NodeFactoryStrategy;

		[[nodiscard]]
		explicit BasicTraverseDriver(
			const vertex_type& origin,
			tracker_type tracker = tracker_type{},
			node_factory_type nodeFactory = node_factory_type{},
			state_type state = state_type{}
		)
			: m_Tracker{std::move(tracker)},
			m_NodeFactory{std::move(nodeFactory)},
			m_Current{m_NodeFactory.make_init_node(std::move(origin))},
			m_State{std::move(state)}
		{
			const bool result = tracker::set_visited(m_Tracker, node::vertex(m_Current));
			assert(result && "Tracker returned false (already visited) the origin node.");
		}

		template <concepts::graph_for<node_type> Graph> // let the concept here, because otherwise it results in an ICE on msvc v142
		[[nodiscard]]
		constexpr std::optional<node_type> next(const Graph& graph)
		{
			std::optional result = m_State.next(
				filter_transform(graph.neighbor_infos(m_Current), m_Tracker, m_NodeFactory, m_Current));
			for (; result; result = m_State.next(std::array<node_type, 0>{}))
			{
				if (tracker::set_visited(m_Tracker, node::vertex(*result)))
				{
					m_Current = *result;
					return result;
				}
			}

			return std::nullopt;
		}

		[[nodiscard]]
		constexpr const Node& current_node() const noexcept
		{
			return m_Current;
		}

		[[nodiscard]]
		constexpr const TrackingStrategy& tracker() const noexcept
		{
			return m_Tracker;
		}

		[[nodiscard]]
		constexpr const NodeFactoryStrategy& node_factory() const noexcept
		{
			return m_NodeFactory;
		}

		[[nodiscard]]
		constexpr const StateStrategy& state() const noexcept
		{
			return m_State;
		}

	private:
		SL_UTILITY_NO_UNIQUE_ADDRESS TrackingStrategy m_Tracker{};
		SL_UTILITY_NO_UNIQUE_ADDRESS NodeFactoryStrategy m_NodeFactory{};
		Node m_Current{};
		StateStrategy m_State{};
	};
}

#endif
