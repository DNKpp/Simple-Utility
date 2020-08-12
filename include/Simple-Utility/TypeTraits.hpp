//		  Copyright Dominic Koepke 2019 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//	(See accompanying file LICENSE_1_0.txt or copy at
//		  https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_TYPE_TRAITS_HPP
#define SL_TYPE_TRAITS_HPP

#pragma once

#include <type_traits>

namespace sl::detail
{
	template <class T1, class T2, class TReturn = void>
	struct IsComparableImpl :
		std::false_type
	{
	};

	template <class T1, class T2>
	struct IsComparableImpl<T1, T2, bool> :
		std::true_type
	{
	};

	template <class T1, class T2>
	struct IsComparable :
		IsComparableImpl<T1, T2, decltype(std::declval<T1>() == std::declval<T2>())>
	{
	};

	template <class T1, class T2>
	struct IsNothrowComparable :
		std::conjunction<IsComparable<T1, T2>, std::bool_constant<noexcept(std::declval<T1>() == std::declval<T2>())>>
	{
	};
}

namespace sl
{
	template <class T1, class T2 = T1>
	struct IsComparable :
		detail::IsComparable<std::remove_cvref_t<T1>, std::remove_cvref_t<T2>>
	{
	};

	template <class T1, class T2 = T1>
	inline constexpr bool IsComparable_v = IsComparable<T1, T2>::value;

	template <class T1, class T2 = T1>
	struct IsNothrowComparable :
		detail::IsNothrowComparable<std::remove_cvref_t<T1>, std::remove_cvref_t<T2>>
	{
	};

	template <class T1, class T2 = T1>
	inline constexpr bool IsNothrowComparable_v = IsNothrowComparable<T1, T2>::value;
}

#endif
