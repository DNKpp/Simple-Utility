//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_QUEUE_HPP
#define SIMPLE_UTILITY_GRAPH_QUEUE_HPP

#include "Simple-Utility/graph/Common.hpp"
#include "Simple-Utility/graph/Node.hpp"

#include <ranges>

namespace sl::graph::queue::detail
{
	struct empty_fn
	{
		template <class T>
			requires requires(const T& t) { { t.empty() } -> std::convertible_to<bool>; }
		constexpr bool operator ()(const T& queue) const noexcept(noexcept(queue.empty()))
		{
			return queue.empty();
		}

		template <class T>
			requires requires(const T& t) { { empty(t) } -> std::convertible_to<bool>; }
		constexpr decltype(auto) operator ()(const T& queue) const noexcept(noexcept(empty(queue)))
		{
			return empty(queue);
		}
	};

	struct insert_fn
	{
		template <class Queue, std::ranges::input_range Range>
			requires concepts::node<std::ranges::range_value_t<Range>>
					&& requires(Queue& queue) { queue.insert(std::declval<Range&&>()); }
		constexpr void operator ()(
			Queue& container,
			Range&& elements
		) const noexcept(noexcept(container.insert(std::forward<Range>(elements))))
		{
			container.insert(std::forward<Range>(elements));
		}

		template <class Queue, std::ranges::input_range Range>
			requires concepts::node<std::ranges::range_value_t<Range>>
					&& requires(Queue& queue) { insert(queue, std::declval<Range&&>()); }
		constexpr void operator ()(
			Queue& container,
			Range&& elements
		) const noexcept(noexcept(insert(container, std::forward<Range>(elements))))
		{
			insert(container, std::forward<Range>(elements));
		}
	};

	struct next_fn
	{
		template <class Queue>
			requires concepts::node<std::remove_cvref_t<decltype(std::declval<Queue&>().next())>>
		constexpr decltype(auto) operator ()(Queue& queue) const noexcept(noexcept(queue.next()))
		{
			return queue.next();
		}

		template <class Queue>
			requires concepts::node<std::remove_cvref_t<decltype(next(std::declval<Queue&>()))>>
		constexpr decltype(auto) operator ()(Queue& queue) const noexcept(noexcept(next(queue)))
		{
			return next(queue);
		}
	};
}

namespace sl::graph::queue
{
	inline constexpr detail::empty_fn empty{};
	inline constexpr detail::insert_fn insert{};
	inline constexpr detail::next_fn next{};
}

#endif
