//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_MIXINS_QUEUE_STD_PRIORITY_QUEUE_HPP
#define SIMPLE_UTILITY_GRAPH_MIXINS_QUEUE_STD_PRIORITY_QUEUE_HPP

#pragma once

#include <concepts>
#include <queue>
#include <ranges>
#include <utility>

#include "Simple-Utility/graph/Node.hpp"
#include "Simple-Utility/graph/Queue.hpp"

template <class T, class Container, class AfterRelation>
struct sl::graph::customize::insert_fn<std::priority_queue<T, Container, AfterRelation>>
{
	template <std::ranges::input_range Range>
		requires std::convertible_to<std::ranges::range_value_t<Range>, T>
	constexpr void operator ()(std::priority_queue<T, Container, AfterRelation>& container, Range&& elements) const
	{
		for (auto&& element : std::forward<Range>(elements))
		{
			container.push(std::forward<decltype(element)>(element));
		}
	}
};

template <class T, class Container, class AfterRelation>
struct sl::graph::customize::next_fn<std::priority_queue<T, Container, AfterRelation>>
{
	constexpr T operator ()(std::priority_queue<T, Container, AfterRelation>& container) const
	{
		auto element = container.top();
		container.pop();
		return element;
	}
};

namespace sl::graph::queue
{
	struct PriorityAfterRelation
	{
		template <concepts::ranked_node Node>
		[[nodiscard]]
		constexpr bool operator ()(const Node& lhs, const Node& rhs) const
		{
			return node::rank(lhs) > node::rank(rhs);
		}
	};

	template <concepts::ranked_node Node, class Container = std::vector<Node>>
	using CommonPriorityQueue = std::priority_queue<Node, Container, PriorityAfterRelation>;
}

#endif
