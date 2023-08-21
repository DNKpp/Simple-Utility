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
#include <concepts>
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
		constexpr explicit BasicState(node_type origin)
			: BasicState{std::move(origin), {}}
		{
		}

		[[nodiscard]]
		constexpr explicit BasicState(node_type origin, queue_type queue)
			: m_Queue{std::move(queue)}
		{
			queue::insert(m_Queue, std::array{std::move(origin)});
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

	template <
		concepts::node Node,
		class StateStrategy,
		concepts::tracker_for<Node> TrackingStrategy,
		concepts::node_factory_for<Node> NodeFactoryStrategy
	>
	class BasicTraverseDriver
	{
	public:
		using node_type = Node;
		using vertex_type = feature_vertex_t<Node>;

		template <class... OtherArgs>
		[[nodiscard]]
		explicit BasicTraverseDriver(vertex_type origin, OtherArgs&&... otherArgs)
			: m_Current{m_NodeFactory.make_init_node(std::move(origin), std::forward<OtherArgs>(otherArgs)...)},
			m_State{m_Current}
		{
			tracker::set_discovered(m_Tracker, node::vertex(m_Current));
		}

		template <class Graph>
		[[nodiscard]]
		constexpr std::optional<node_type> next(const Graph& graph)
		{
			if (auto result = m_State.next(
				graph.neighbor_infos(m_Current)
				| std::views::filter([&](const auto& info) { return !tracker::set_discovered(m_Tracker, info.vertex); })
				| std::views::transform(
					functional::envelop<functional::Apply>(
						[&]<class... Ts>(Ts&&... infos)
						{
							return m_NodeFactory.make_successor_node(m_Current, std::forward<Ts>(infos)...);
						}))))
			{
				m_Current = *result;
				tracker::set_visited(m_Tracker, node::vertex(m_Current));
				return result;
			}

			return std::nullopt;
		}

		[[nodiscard]]
		constexpr const Node& current_node() const noexcept
		{
			return m_Current;
		}

	private:
		Node m_Current{};
		StateStrategy m_State{};
		SL_UTILITY_NO_UNIQUE_ADDRESS TrackingStrategy m_Tracker{};
		SL_UTILITY_NO_UNIQUE_ADDRESS NodeFactoryStrategy m_NodeFactory{};
	};
}

#endif
