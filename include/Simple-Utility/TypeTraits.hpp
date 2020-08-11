
//          Copyright Dominic Koepke 2019 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_TYPE_TRAITS_HPP
#define SL_TYPE_TRAITS_HPP

#pragma once

#include <type_traits>

namespace sl::detail
{
	template <class T1, class T2>
	struct IsNothrowComparable :
		std::bool_constant<noexcept(std::declval<T1>() == std::declval<T2>())>
	{
	};
}

namespace sl
{
	template <class T1, class T2>
	struct IsNothrowComparable :
		detail::IsNothrowComparable<std::remove_cvref_t<T1>, std::remove_cvref_t<T2>>
	{
	};

	template <class T1, class T2>
	inline constexpr bool IsNothrowComparable_v = IsNothrowComparable<T1, T2>::value;
}

#endif
