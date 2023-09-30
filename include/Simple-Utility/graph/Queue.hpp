//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_QUEUE_HPP
#define SIMPLE_UTILITY_GRAPH_QUEUE_HPP

#pragma once

#include "Simple-Utility/Utility.hpp"
#include "Simple-Utility/graph/Common.hpp"
#include "Simple-Utility/graph/Node.hpp"

#include <ranges>
#include <utility>

namespace sl::graph::customize
{
	template <class>
	struct empty_fn;

	template <class>
	struct insert_fn;

	template <class>
	struct next_fn;
}

namespace sl::graph::queue::detail
{
	template <class T>
		requires requires(const T& t) { { customize::empty_fn<T>{}(t) } -> std::convertible_to<bool>; }
	constexpr bool empty(const T& queue, const priority_tag<2>) noexcept(noexcept(customize::empty_fn<T>{}(queue)))
	{
		return customize::empty_fn<T>{}(queue);
	}

	template <class T>
		requires requires(const T& t) { { t.empty() } -> std::convertible_to<bool>; }
	constexpr bool empty(const T& queue, const priority_tag<1>) noexcept(noexcept(queue.empty()))
	{
		return queue.empty();
	}

	template <class T>
		requires requires(const T& t) { { empty(t) } -> std::convertible_to<bool>; }
	constexpr decltype(auto) empty(const T& queue, const priority_tag<0>) noexcept(noexcept(empty(queue)))
	{
		return empty(queue);
	}

	struct empty_fn
	{
		template <class T>
			requires requires(const T& t) { { detail::empty(t, priority_tag<2>{}) } -> std::convertible_to<bool>; }
		constexpr bool operator ()(const T& queue) const noexcept(noexcept(detail::empty(queue, priority_tag<2>{})))
		{
			return detail::empty(queue, priority_tag<2>{});
		}
	};

	template <class Queue, std::ranges::input_range Range>
		requires requires(Queue& queue) { customize::insert_fn<Queue>{}(queue, std::declval<Range&&>()); }
	constexpr void insert(
		Queue& queue,
		Range&& elements,
		const priority_tag<2>
	) noexcept(noexcept(customize::insert_fn<Queue>{}(queue, std::forward<Range>(elements))))
	{
		customize::insert_fn<Queue>{}(queue, std::forward<Range>(elements));
	}

	template <class Queue, std::ranges::input_range Range>
		requires requires(Queue& queue) { queue.insert(std::declval<Range&&>()); }
	constexpr void insert(
		Queue& container,
		Range&& elements,
		const priority_tag<1>
	) noexcept(noexcept(container.insert(std::forward<Range>(elements))))
	{
		container.insert(std::forward<Range>(elements));
	}

	template <class Queue, std::ranges::input_range Range>
		requires requires(Queue& queue) { insert(queue, std::declval<Range&&>()); }
	constexpr void insert(
		Queue& container,
		Range&& elements,
		const priority_tag<0>
	) noexcept(noexcept(insert(container, std::forward<Range>(elements))))
	{
		insert(container, std::forward<Range>(elements));
	}

	struct insert_fn
	{
		template <class Queue, std::ranges::input_range Range>
			requires requires(Queue& queue) { detail::insert(queue, std::declval<Range&&>(), priority_tag<2>{}); }
		constexpr void operator ()(
			Queue& queue,
			Range&& elements
		) const noexcept(noexcept(detail::insert(queue, std::forward<Range>(elements), priority_tag<2>{})))
		{
			detail::insert(queue, std::forward<Range>(elements), priority_tag<2>{});
		}
	};

	template <class Queue>
		requires requires(Queue& queue)
		{
			requires concepts::basic_node<std::remove_cvref_t<decltype(customize::next_fn<Queue>{}(queue))>>;
		}
	constexpr decltype(auto) next(Queue& queue, const priority_tag<2>) noexcept(noexcept(customize::next_fn<Queue>{}(queue)))
	{
		return customize::next_fn<Queue>{}(queue);
	}

	template <class Queue>
		requires requires(Queue& queue)
		{
			requires concepts::basic_node<std::remove_cvref_t<decltype(queue.next())>>;
		}
	constexpr decltype(auto) next(Queue& queue, const priority_tag<1>) noexcept(noexcept(queue.next()))
	{
		return queue.next();
	}

	template <class Queue>
		requires requires(Queue& queue)
		{
			requires concepts::basic_node<std::remove_cvref_t<decltype(next(queue))>>;
		}
	constexpr decltype(auto) next(Queue& queue, const priority_tag<0>) noexcept(noexcept(next(queue)))
	{
		return next(queue);
	}

	struct next_fn
	{
		template <class Queue>
			requires requires(Queue& queue)
			{
				requires concepts::basic_node<std::remove_cvref_t<decltype(detail::next(queue, priority_tag<2>{}))>>;
			}
		constexpr decltype(auto) operator ()(Queue& queue) const noexcept(noexcept(detail::next(queue, priority_tag<2>{})))
		{
			return detail::next(queue, priority_tag<2>{});
		}
	};
}

namespace sl::graph::queue
{
	inline constexpr detail::empty_fn empty{};
	inline constexpr detail::insert_fn insert{};
	inline constexpr detail::next_fn next{};
}

namespace sl::graph::queue::detail
{
	// LCOV_EXCL_START

	template <class T>
	struct dummy_input_range
	{
		struct iterator
		{
			using iterator_concept = std::input_iterator_tag;
			using element_type = T;
			using difference_type = std::ptrdiff_t;

			T operator *() const { throw std::runtime_error{"Dummy"}; }

			iterator& operator ++() { return *this; }

			// ReSharper disable once CppDiscardedPostfixOperatorResult
			void operator ++(int) { (*this)++; }

			bool operator==(const iterator&) const = default;
		};

		static iterator begin() { return {}; }

		static iterator end() { return {}; }
	};

	static_assert(std::ranges::input_range<dummy_input_range<int>>);

	// LCOV_EXCL_STOP
}

namespace sl::graph::concepts
{
	template <class T, class Node>
	concept queue_for = sl::concepts::unqualified<T>
						&& basic_node<Node>
						// ReSharper disable once CppRedundantTemplateKeyword // pleases msvc v142
						&& requires(T& queue, queue::detail::template dummy_input_range<Node> inputRange)
						{
							{ queue::empty(std::as_const(queue)) } -> std::convertible_to<bool>;
							queue::insert(queue, inputRange);
							{ queue::next(queue) } -> std::convertible_to<Node>;
						};
}

#endif
