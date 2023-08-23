//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_TRACKER_HPP
#define SIMPLE_UTILITY_GRAPH_TRACKER_HPP

#pragma once

#include "Simple-Utility/Utility.hpp"
#include "Simple-Utility/concepts/stl_extensions.hpp"
#include "Simple-Utility/graph/Common.hpp"

// ReSharper disable once CppUnusedIncludeDirective
#include <concepts>

namespace sl::graph::customize
{
	template <class>
	struct set_discovered_fn;

	template <class>
	struct set_visited_fn;
}

namespace sl::graph::tracker::detail
{
	template <class T, concepts::vertex Vertex>
		requires requires(T& t, const Vertex& v) { { customize::set_discovered_fn<T>{}(t, v) } -> std::convertible_to<bool>; }
	constexpr bool set_discovered(
		T& tracker,
		const Vertex& v,
		const priority_tag<2>
	) noexcept(noexcept(customize::set_discovered_fn<T>{}(tracker, v)))
	{
		return customize::set_discovered_fn<T>{}(tracker, v);
	}

	template <class T, concepts::vertex Vertex>
		requires requires(T& t, const Vertex& v) { { t.set_discovered(v) } -> std::convertible_to<bool>; }
	constexpr bool set_discovered(T& tracker, const Vertex& v, const priority_tag<1>) noexcept(noexcept(tracker.set_discovered(v)))
	{
		return tracker.set_discovered(v);
	}

	template <class T, concepts::vertex Vertex>
		requires requires(T& t, const Vertex& v) { { set_discovered(t, v) } -> std::convertible_to<bool>; }
	constexpr bool set_discovered(T& tracker, const Vertex& v, const priority_tag<0>) noexcept(noexcept(set_discovered(tracker, v)))
	{
		return set_discovered(tracker, v);
	}

	struct set_discovered_fn
	{
		template <class T, concepts::vertex Vertex>
			requires requires(T& t, const Vertex& v)
			{
				{ detail::set_discovered(t, v, priority_tag<2>{}) } -> std::convertible_to<bool>;
			}
		constexpr bool operator ()(
			T& tracker,
			const Vertex& v
		) const noexcept(noexcept(detail::set_discovered(tracker, v, priority_tag<2>{})))
		{
			return detail::set_discovered(tracker, v, priority_tag<2>{});
		}
	};

	template <class T, concepts::vertex Vertex>
		requires requires(T& t, const Vertex& v) { customize::set_visited_fn<T>{}(t, v); }
	constexpr void set_visited(
		T& tracker,
		const Vertex& v,
		const priority_tag<2>
	) noexcept(noexcept(customize::set_visited_fn<T>{}(tracker, v)))
	{
		customize::set_visited_fn<T>{}(tracker, v);
	}

	template <class T, concepts::vertex Vertex>
		requires requires(T& t, const Vertex& v) { t.set_visited(v); }
	constexpr void set_visited(T& tracker, const Vertex& v, const priority_tag<1>) noexcept(noexcept(tracker.set_visited(v)))
	{
		tracker.set_visited(v);
	}

	template <class T, concepts::vertex Vertex>
		requires requires(T& t, const Vertex& v) { set_visited(t, v); }
	constexpr void set_visited(T& tracker, const Vertex& v, const priority_tag<0>) noexcept(noexcept(set_visited(tracker, v)))
	{
		set_visited(tracker, v);
	}

	struct set_visited_fn
	{
		template <class T, concepts::vertex Vertex>
			requires requires(T& t, const Vertex& v) { detail::set_visited(t, v, priority_tag<2>{}); }
		constexpr void operator ()(
			T& tracker,
			const Vertex& v
		) const noexcept(noexcept(detail::set_visited(tracker, v, priority_tag<2>{})))
		{
			detail::set_visited(tracker, v, priority_tag<2>{});
		}
	};
}

namespace sl::graph::tracker
{
	inline constexpr detail::set_discovered_fn set_discovered{};
	inline constexpr detail::set_visited_fn set_visited{};
}

namespace sl::graph::concepts
{
	template <class T, class Vertex>
	concept tracker_for = sl::concepts::unqualified<T>
						&& vertex<Vertex>
						&& requires(T& tracker, const Vertex& v)
						{
							{ tracker::set_discovered(tracker, v) } -> std::convertible_to<bool>;
							tracker::set_visited(tracker, v);
						};
}

#endif
