//          Copyright Dominic Koepke 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_CONCEPTS_OPERATORS_HPP
#define SL_CONCEPTS_OPERATORS_HPP

#pragma once

#include <concepts>

namespace sl::concepts
{
	template <class T1, class T2>
	concept right_shiftable_with = requires(T1 lhs, T2 rhs)
	{
		{ lhs >> rhs };
	};

	template <class T1, class T2, class TResult = std::remove_cvref_t<T1>>
	concept right_shiftable_with_r = requires(T1 lhs, T2 rhs)
	{
		{ lhs >> rhs } -> std::convertible_to<TResult>;
	};

	template <class T>
	concept right_shiftable = right_shiftable_with<T, T>;

	template <class T, class TResult = std::remove_cvref_t<T>>
	concept right_shiftable_r = right_shiftable_with_r<T, T, TResult>;

	template <class T1, class T2>
	concept left_shiftable_with = requires(T1 lhs, T2 rhs)
	{
		{ lhs << rhs };
	};

	template <class T1, class T2, class TResult = std::remove_cvref_t<T1>>
	concept left_shiftable_with_r = requires(T1 lhs, T2 rhs)
	{
		{ lhs << rhs } -> std::convertible_to<TResult>;
	};

	template <class T>
	concept left_shiftable = left_shiftable_with<T, T>;

	template <class T, class TResult = std::remove_cvref_t<T>>
	concept left_shiftable_r = left_shiftable_with_r<T, T, TResult>;

	template <class T1, class T2>
	concept bidirectional_shiftable_with = left_shiftable_with<T1, T2> && right_shiftable_with<T1, T2>;

	template <class T1, class T2, class TResult = std::remove_cvref_t<T1>>
	concept bidirectional_shiftable_with_r = left_shiftable_with_r<T1, T2, TResult> && right_shiftable_with_r<T1, T2, TResult>;

	template <class T>
	concept bidirectional_shiftable = bidirectional_shiftable_with<T, T>;

	template <class T, class TResult = std::remove_cvref_t<T>>
	concept bidirectional_shiftable_r = bidirectional_shiftable_with_r<T, T, TResult>;

	template <class T1, class T2>
	concept right_shift_assignable_with = requires(T1 lhs, T2 rhs)
	{
		{ lhs >>= rhs };
	};

	template <class T1, class T2, class TResult = std::remove_cvref_t<T1>&>
	concept right_shift_assignable_with_r = requires(T1 lhs, T2 rhs)
	{
		{ lhs >>= rhs } -> std::convertible_to<TResult>;
	};

	template <class T>
	concept right_shift_assignable = right_shift_assignable_with<T, T>;

	template <class T, class TResult = std::remove_cvref_t<T>&>
	concept right_shift_assignable_r = right_shift_assignable_with_r<T, T, TResult>;

	template <class T1, class T2>
	concept left_shift_assignable_with = requires(T1 lhs, T2 rhs)
	{
		{ lhs <<= rhs };
	};

	template <class T1, class T2, class TResult = std::remove_cvref_t<T1>&>
	concept left_shift_assignable_with_r = requires(T1 lhs, T2 rhs)
	{
		{ lhs <<= rhs } -> std::convertible_to<TResult>;
	};

	template <class T>
	concept left_shift_assignable = left_shift_assignable_with<T, T>;

	template <class T, class TResult = std::remove_cvref_t<T>&>
	concept left_shift_assignable_r = left_shift_assignable_with_r<T, T, TResult>;

	template <class T1, class T2>
	concept bidirectional_shift_assignable_with = left_shift_assignable_with<T1, T2> && right_shift_assignable_with<T1, T2>;

	template <class T1, class T2, class TResult = std::remove_cvref_t<T1>&>
	concept bidirectional_shift_assignable_with_r = left_shift_assignable_with_r<T1, T2, TResult>
													&& right_shift_assignable_with_r<T1, T2, TResult>;

	template <class T>
	concept bidirectional_shift_assignable = bidirectional_shift_assignable_with<T, T>;

	template <class T, class TResult = std::remove_cvref_t<T>&>
	concept bidirectional_shift_assignable_r = bidirectional_shift_assignable_with_r<T, T, TResult>;

	template <class T1, class T2>
	concept fully_shiftable_with = bidirectional_shift_assignable_with<T1, T2> && bidirectional_shiftable_with<T1, T2>;

	template <class T1, class T2, class TResult = std::remove_cvref_t<T1>, class TAssignResult = std::remove_cvref_t<T1>&>
	concept fully_shiftable_with_r = bidirectional_shiftable_with_r<T1, T2, TResult>
									&& bidirectional_shift_assignable_with_r<T1, T2, TAssignResult>;

	template <class T>
	concept fully_shiftable = fully_shiftable_with<T, T>;

	template <class T, class TResult = std::remove_cvref_t<T>, class TAssignResult = std::remove_cvref_t<T>&>
	concept fully_shiftable_r = fully_shiftable_with_r<T, T, TResult, TAssignResult>;
}

#endif
