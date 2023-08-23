//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_MIXINS_TRACKER_STD_UNORDERED_MAP_HPP
#define SIMPLE_UTILITY_GRAPH_MIXINS_TRACKER_STD_UNORDERED_MAP_HPP

#pragma once

#include <cassert>
#include <unordered_map>

#include "Simple-Utility/graph/Tracker.hpp"

template <sl::graph::concepts::vertex Key, class Hash, class KeyEqual, class Allocator>
struct sl::graph::customize::set_discovered_fn<std::unordered_map<Key, bool, Hash, KeyEqual, Allocator>>
{
	template <std::convertible_to<Key> K>
	bool operator ()(std::unordered_map<Key, bool, Hash, KeyEqual, Allocator>& container, K&& vertex)
	{
		const auto [_, inserted] = container.try_emplace(std::forward<K>(vertex), false);
		return !inserted;
	}
};

template <sl::graph::concepts::vertex Key, class Hash, class KeyEqual, class Allocator>
struct sl::graph::customize::set_visited_fn<std::unordered_map<Key, bool, Hash, KeyEqual, Allocator>>
{
	template <std::convertible_to<Key> K>
	constexpr void operator ()(std::unordered_map<Key, bool, Hash, KeyEqual, Allocator>& container, K&& vertex) const
	{
		const auto [iter, inserted] = container.insert_or_assign(std::forward<K>(vertex), true);
		assert(!inserted && "Visited a vertex which hasn't been discovered yet.");
	}
};

#endif
