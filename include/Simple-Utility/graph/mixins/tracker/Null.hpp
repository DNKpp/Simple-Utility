//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_MIXINS_TRACKER_NULL_HPP
#define SIMPLE_UTILITY_GRAPH_MIXINS_TRACKER_NULL_HPP

#pragma once

#include "Simple-Utility/graph/Common.hpp"
#include "Simple-Utility/graph/Tracker.hpp"

namespace sl::graph::tracker
{
	class Null
	{
	public:
		template <concepts::vertex Vertex>
		[[nodiscard]]
		static constexpr bool set_discovered([[maybe_unused]] const Vertex& vertex) noexcept
		{
			return true;
		}

		template <concepts::vertex Vertex>
		[[nodiscard]]
		static constexpr bool set_visited([[maybe_unused]] const Vertex& vertex) noexcept
		{
			return true;
		}
	};
}

#endif
