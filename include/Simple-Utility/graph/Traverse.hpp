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
}

#endif
