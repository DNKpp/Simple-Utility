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
#include "Simple-Utility/graph/NodeFactory.hpp"
#include "Simple-Utility/graph/Queue.hpp"
#include "Simple-Utility/graph/Tracker.hpp"
#include "Simple-Utility/graph/View.hpp"

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

		~BasicState() = default;
		BasicState(const BasicState&) = delete;
		BasicState& operator =(const BasicState&) = delete;
		BasicState(BasicState&&) = default;
		BasicState& operator =(BasicState&&) = default;

		template <class... QueueArgs>
			requires std::constructible_from<queue_type, QueueArgs...>
		[[nodiscard]]
		constexpr explicit BasicState(QueueArgs&&... queueArgs) noexcept(std::is_nothrow_constructible_v<queue_type, QueueArgs...>)
			: m_Queue{std::forward<QueueArgs>(queueArgs)...}
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
		QueuingStrategy m_Queue;
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

	template <class Edges, class Node>
	[[nodiscard]]
	constexpr std::vector<Node> filter_transform(const Edges& edges, auto& tracker, auto& nodeFactory, const Node& currentNode)
	{
		std::vector<Node> nodes{};
		if constexpr (std::ranges::sized_range<Edges>)
		{
			nodes.reserve(std::ranges::size(edges));
		}

		for (const auto& edge : edges)
		{
			if (tracker::set_discovered(tracker, edge::destination(edge)))
			{
				nodes.emplace_back(nodeFactory.make_successor_node(currentNode, edge));
			}
		}

		return nodes;
	};

#else

	template <class Edges, class Node>
	[[nodiscard]]
	constexpr auto filter_transform(Edges&& edges, auto& tracker, auto& nodeFactory, const Node& currentNode)
	{
		return std::forward<Edges>(edges)
				| std::views::filter([&](const auto& edge) { return tracker::set_discovered(tracker, edge::destination(edge)); })
				| std::views::transform([&](const auto& edge) { return nodeFactory.make_successor_node(currentNode, edge); });
	};

#endif

	template <
		concepts::node Node,
		state_for<Node> StateStrategy,
		concepts::tracker_for<node::vertex_t<Node>> TrackingStrategy,
		class NodeFactoryStrategy>
	class BasicTraverseDriver
	{
	public:
		using node_type = Node;
		using vertex_type = node::vertex_t<Node>;
		using state_type = StateStrategy;
		using tracker_type = TrackingStrategy;
		using node_factory_type = NodeFactoryStrategy;

		template <class Origin, class... TrackerArgs, class... NodeFactoryArgs, class... StateArgs>
			requires std::constructible_from<vertex_type, Origin>
					&& std::constructible_from<tracker_type, TrackerArgs...>
					&& std::constructible_from<node_factory_type, NodeFactoryArgs...>
					&& std::constructible_from<state_type, StateArgs...>
		[[nodiscard]]
		explicit BasicTraverseDriver(
			Origin&& origin,
			std::tuple<TrackerArgs...> trackerArgs,
			std::tuple<NodeFactoryArgs...> nodeFactoryArgs,
			std::tuple<StateArgs...> stateArgs
		)
			: m_Tracker{std::make_from_tuple<tracker_type>(std::move(trackerArgs))},
			m_NodeFactory{std::make_from_tuple<node_factory_type>(std::move(nodeFactoryArgs))},
			m_State{std::make_from_tuple<state_type>(std::move(stateArgs))},
			m_Current{m_NodeFactory.make_init_node(std::forward<Origin>(origin))}
		{
			const bool result = tracker::set_discovered(m_Tracker, node::vertex(m_Current))
								&& tracker::set_visited(m_Tracker, node::vertex(m_Current));
			assert(result && "Tracker returned false (already visited) for the origin node.");
		}

		template <concepts::view_for<node_type> View> // let the concept here, because otherwise it results in an ICE on msvc v142
			requires concepts::node_factory_for<NodeFactoryStrategy, Node, view::edge_t<View>>
		[[nodiscard]]
		constexpr std::optional<node_type> next(const View& graph)
		{
			std::optional result = m_State.next(
				filter_transform(graph.edges(m_Current), m_Tracker, m_NodeFactory, m_Current));
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
		// do not reorder. Seems to somehow cause segment faults, when ordered differently
		SL_UTILITY_NO_UNIQUE_ADDRESS TrackingStrategy m_Tracker;
		SL_UTILITY_NO_UNIQUE_ADDRESS NodeFactoryStrategy m_NodeFactory;
		StateStrategy m_State;
		Node m_Current;
	};
}

namespace sl::graph
{
	template <class View, class Driver>
	class Traverser final
	{
	public:
		using node_type = typename Driver::node_type;
		using vertex_type = node::vertex_t<node_type>;

		[[nodiscard]]
		constexpr explicit Traverser(View view, vertex_type origin)
			: m_View{std::move(view)},
			m_Driver{std::move(origin), std::tuple{}, std::tuple{}, std::tuple{}}
		{
		}

		[[nodiscard]]
		std::optional<node_type> next()
		{
			return m_Driver.next(m_View);
		}

		struct Sentinel final
		{
		};

		struct Iterator final
		{
			friend Traverser;

		public:
			using iterator_concept = std::input_iterator_tag;
			using element_type = node_type;
			using difference_type = std::ptrdiff_t;

			[[nodiscard]]
			constexpr const node_type& operator *() const noexcept
			{
				return *m_Value;
			}

			constexpr Iterator& operator ++()
			{
				m_Value = m_Source->next();
				return *this;
			}

			constexpr void operator ++(int)
			{
				operator++();
			}

			[[nodiscard]]
			constexpr bool operator==([[maybe_unused]] const Sentinel) const noexcept
			{
				return !m_Value;
			}

		private:
			Traverser* m_Source{};
			std::optional<node_type> m_Value{};

			[[nodiscard]]
			constexpr explicit Iterator(Traverser& source)
				: m_Source{std::addressof(source)},
				m_Value{source.next()}
			{
			}
		};

		[[nodiscard]]
		constexpr Iterator begin() &
		{
			return Iterator{*this};
		}

		[[nodiscard]]
		constexpr Sentinel end() const noexcept
		{
			return Sentinel{};
		}

	private:
		View m_View{};
		Driver m_Driver{};
	};
}

#endif
