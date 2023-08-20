//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_QUEUE_HPP
#define SIMPLE_UTILITY_GRAPH_QUEUE_HPP

#include "Simple-Utility/graph/Common.hpp"
#include "Simple-Utility/graph/Node.hpp"

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
}

namespace sl::graph::queue
{
	inline constexpr detail::empty_fn empty{};
}

#endif
