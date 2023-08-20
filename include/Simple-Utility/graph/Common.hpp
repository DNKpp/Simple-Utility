//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_COMMON_HPP
#define SIMPLE_UTILITY_GRAPH_COMMON_HPP

#pragma once

#include <concepts>
#include <ranges>

namespace sl::graph::concepts
{
	template <class T>
	concept vertex = std::same_as<T, std::remove_cvref_t<T>>
					&& std::equality_comparable<T>
					&& std::copyable<T>;
}
	
#endif
