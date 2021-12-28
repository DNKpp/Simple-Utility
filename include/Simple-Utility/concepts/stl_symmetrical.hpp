//          Copyright Dominic Koepke 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UNIQUE_HANDLE_HPP
#define SL_UNIQUE_HANDLE_HPP

#pragma once

#include <concepts>

namespace sl::concepts
{
	template <class TSource, class TTarget>
	concept constructible_by = std::constructible_from<TTarget, TSource>;
}

#endif
