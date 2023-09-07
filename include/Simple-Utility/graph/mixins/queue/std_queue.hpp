//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_MIXINS_QUEUE_STD_QUEUE_HPP
#define SIMPLE_UTILITY_GRAPH_MIXINS_QUEUE_STD_QUEUE_HPP

#pragma once

#include <concepts>
#include <queue>
#include <ranges>
#include <utility>

#include "Simple-Utility/graph/Node.hpp"
#include "Simple-Utility/graph/Queue.hpp"

template <class T, class Container>
struct sl::graph::customize::insert_fn<std::queue<T, Container>>
{
	template <std::ranges::input_range Range>
		requires std::convertible_to<std::ranges::range_value_t<Range>, T>
	constexpr void operator ()(std::queue<T, Container>& container, Range&& elements) const
	{
		for (auto&& element : std::forward<Range>(elements))
		{
			container.push(std::forward<decltype(element)>(element));
		}
	}
};

template <class T, class Container>
struct sl::graph::customize::next_fn<std::queue<T, Container>>
{
	constexpr T operator ()(std::queue<T, Container>& container) const
	{
		auto element = std::move(container.front());
		container.pop();
		return element;
	}
};

namespace sl::graph::queue
{
	template <concepts::node Node, class Container = std::deque<Node>>
	using CommonQueue = std::queue<Node, Container>;
}

#endif
