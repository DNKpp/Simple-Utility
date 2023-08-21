//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_TRACKER_HPP
#define SIMPLE_UTILITY_GRAPH_TRACKER_HPP

#pragma once

#include "Simple-Utility/graph/Common.hpp"

#include <concepts>

namespace sl::graph::tracker::detail
{
	struct set_discovered_fn
	{
		template <class T, concepts::vertex Vertex>
			requires requires(T& t, const Vertex& v) { { t.set_discovered(v) } -> std::convertible_to<bool>; }
		constexpr bool operator ()(T& tracker, const Vertex& v) const noexcept(noexcept(tracker.set_discovered(v)))
		{
			return tracker.set_discovered(v);
		}

		template <class T, concepts::vertex Vertex>
			requires requires(T& t, const Vertex& v) { { set_discovered(t, v) } -> std::convertible_to<bool>; }
		constexpr bool operator ()(T& tracker, const Vertex& v) const noexcept(noexcept(set_discovered(tracker, v)))
		{
			return set_discovered(tracker, v);
		}
	};

	struct set_visited_fn
	{
		template <class T, concepts::vertex Vertex>
			requires requires(T& t, const Vertex& v) { t.set_visited(v); }
		constexpr void operator ()(T& tracker, const Vertex& v) const noexcept(noexcept(tracker.set_visited(v)))
		{
			tracker.set_visited(v);
		}

		template <class T, concepts::vertex Vertex>
			requires requires(T& t, const Vertex& v) { set_visited(t, v); }
		constexpr void operator ()(T& tracker, const Vertex& v) const noexcept(noexcept(set_visited(tracker, v)))
		{
			set_visited(tracker, v);
		}
	};
}

namespace sl::graph::tracker
{
	inline constexpr detail::set_discovered_fn set_discovered{};
	inline constexpr detail::set_visited_fn set_visited{};
}

#endif
