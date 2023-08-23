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
			assert(queue::empty(m_Queue) && "Queue contains already elements.");
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

		[[nodiscard]]
		constexpr std::optional<node_type> next(const concepts::graph_for<Node> auto& graph)
		{
			auto result = m_State.next(
				graph.neighbor_infos(m_Current)
				| std::views::filter(
					[&](const auto& info) { return tracker::set_discovered(m_Tracker, node::vertex(info)); })
				| std::views::transform(
					[&](const auto& info)
					{
						return m_NodeFactory.make_successor_node(m_Current, info);
					}));

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
