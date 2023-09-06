//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_COMMON_HPP
#define SIMPLE_UTILITY_GRAPH_COMMON_HPP

#pragma once

#include "Simple-Utility/concepts/operators.hpp"
#include "Simple-Utility/concepts/stl_extensions.hpp"

#include <concepts>

namespace sl::graph::concepts
{
	template <class T>
	concept vertex = sl::concepts::unqualified<T>
					&& std::equality_comparable<T>
					&& std::copyable<T>;

	template <class T>
	concept weight = sl::concepts::unqualified<T>
					&& std::copyable<T>
					&& sl::concepts::plus<T>
					&& sl::concepts::minus<T>
					&& sl::concepts::plus_assign<T>
					&& sl::concepts::minus_assign<T>;

	template <class T>
	concept rank = sl::concepts::unqualified<T>
					&& std::regular<T>
					&& std::totally_ordered<T>;

	template <class T>
	concept readable_vertex_type = requires { typename T::vertex_type; }
									&& vertex<typename T::vertex_type>;

	template <class T>
	concept readable_weight_type = requires { typename T::weight_type; }
									&& weight<typename T::weight_type>;

	template <class T>
	concept readable_rank_type = requires { typename T::rank_type; }
								&& rank<typename T::rank_type>;
}

#endif
