//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_UNIFIED_BASE_HPP
#define SL_UTILITY_UNIFIED_BASE_HPP

#pragma once

#include <type_traits>

namespace sl
{
	template <class TTag>
	struct unified_base
	{};

	template <class T, class TTag>
	concept derived_from_unified_base = std::is_base_of_v<unified_base<TTag>, std::remove_cvref_t<T>>; 
}

#endif
