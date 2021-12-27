//          Copyright Dominic Koepke 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// ReSharper disable CppClangTidyClangDiagnosticDocumentation
#ifndef SL_CONCEPTS_OPERATORS_HPP
#define SL_CONCEPTS_OPERATORS_HPP

#pragma once

#include <concepts>

namespace sl::concepts
{
	/**
	* \defgroup concepts concepts
	* @{
	*/

	/**
	* \defgroup shift_operators shift operators
	* @{
	*/

	/**
	 * \brief Determines whether two types can be used in operator >> statements.
	 * \tparam T1 The left-hand-side type
	 * \tparam T2 the right-hand-side-type
	 */
	template <class T1, class T2>
	concept right_shiftable_with = requires(T1 lhs, T2 rhs)
	{
		{ lhs >> rhs };
	};

	/**
	 * \brief Determines whether two types can be used in operator >> statements and checks if the return type
	 * is convertible to the expected.
	 * \tparam T1 The left-hand-side type
	 * \tparam T2 the right-hand-side-type
	 * \tparam TResult The expected return type
	 */
	template <class T1, class T2, class TResult = std::remove_cvref_t<T1>>
	concept right_shiftable_with_r = requires(T1 lhs, T2 rhs)
	{
		{ lhs >> rhs } -> std::convertible_to<TResult>;
	};

	/**
	 * \brief Determines whether the given type can be used on both sides of operator >> statements.
	 * \tparam T The type to check
	 */
	template <class T>
	concept right_shiftable = right_shiftable_with<T, T>;

	/**
	 * \brief Determines whether the given type can be used on both sides of operator >> statements and checks if the
	 * return type is convertible to the expected.
	 * \tparam T The type to check
	 * \tparam TResult  The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>>
	concept right_shiftable_r = right_shiftable_with_r<T, T, TResult>;

	/**
	 * \brief Determines whether two types can be used in operator << statements.
	 * \tparam T1 The left-hand-side type
	 * \tparam T2 the right-hand-side-type
	 */
	template <class T1, class T2>
	concept left_shiftable_with = requires(T1 lhs, T2 rhs)
	{
		{ lhs << rhs };
	};

	/**
	 * \brief Determines whether two types can be used in operator << statements and checks if the return type
	 * is convertible to the expected.
	 * \tparam T1 The left-hand-side type
	 * \tparam T2 the right-hand-side-type
	 * \tparam TResult The expected return type
	 */
	template <class T1, class T2, class TResult = std::remove_cvref_t<T1>>
	concept left_shiftable_with_r = requires(T1 lhs, T2 rhs)
	{
		{ lhs << rhs } -> std::convertible_to<TResult>;
	};

	/**
	 * \brief Determines whether the given type can be used on both sides of operator << statements.
	 * \tparam T The type to check
	 */
	template <class T>
	concept left_shiftable = left_shiftable_with<T, T>;

	/**
	 * \brief Determines whether the given type can be used on both sides of operator << statements and checks if the
	 * return type is convertible to the expected.
	 * \tparam T The type to check
	 * \tparam TResult  The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>>
	concept left_shiftable_r = left_shiftable_with_r<T, T, TResult>;

	/**
	 * \brief Determines whether two types can be used in operator >> and operator << statements.
	 * \tparam T1 The left-hand-side type
	 * \tparam T2 the right-hand-side-type
	 */
	template <class T1, class T2>
	concept bidirectional_shiftable_with = left_shiftable_with<T1, T2> && right_shiftable_with<T1, T2>;

	/**
	 * \brief Determines whether two types can be used in operator >> and operator << statements and checks if the return type
	 * is convertible to the expected.
	 * \tparam T1 The left-hand-side type
	 * \tparam T2 the right-hand-side-type
	 * \tparam TResult The expected return type
	 */
	template <class T1, class T2, class TResult = std::remove_cvref_t<T1>>
	concept bidirectional_shiftable_with_r = left_shiftable_with_r<T1, T2, TResult> && right_shiftable_with_r<T1, T2, TResult>;

	/**
	 * \brief Determines whether the given type can be used on both sides of operator << and operator >> statements.
	 * \tparam T The type to check
	 */
	template <class T>
	concept bidirectional_shiftable = bidirectional_shiftable_with<T, T>;

	/**
	 * \brief Determines whether the given type can be used on both sides of operator >> and operator << statements and checks if the
	 * return type is convertible to the expected.
	 * \tparam T The type to check
	 * \tparam TResult  The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>>
	concept bidirectional_shiftable_r = bidirectional_shiftable_with_r<T, T, TResult>;

	/**
	 * \brief Determines whether two types can be used in operator >>= statements.
	 * \tparam T1 The left-hand-side type
	 * \tparam T2 the right-hand-side-type
	 */
	template <class T1, class T2>
	concept right_shift_assignable_with = requires(T1 lhs, T2 rhs)
	{
		{ lhs >>= rhs };
	};

	/**
	 * \brief Determines whether two types can be used in operator >>= statements and checks if the return type
	 * is convertible to the expected.
	 * \tparam T1 The left-hand-side type
	 * \tparam T2 the right-hand-side-type
	 * \tparam TResult The expected return type
	 */
	template <class T1, class T2, class TResult = std::remove_cvref_t<T1>&>
	concept right_shift_assignable_with_r = requires(T1 lhs, T2 rhs)
	{
		{ lhs >>= rhs } -> std::convertible_to<TResult>;
	};

	/**
	 * \brief Determines whether the given type can be used on both sides of operator >>= statements.
	 * \tparam T The type to check
	 */
	template <class T>
	concept right_shift_assignable = right_shift_assignable_with<T, T>;

	/**
	 * \brief Determines whether the given type can be used on both sides of operator >>= statements and checks if the
	 * return type is convertible to the expected.
	 * \tparam T The type to check
	 * \tparam TResult  The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>&>
	concept right_shift_assignable_r = right_shift_assignable_with_r<T, T, TResult>;

	/**
	 * \brief Determines whether two types can be used in operator <<= statements.
	 * \tparam T1 The left-hand-side type
	 * \tparam T2 the right-hand-side-type
	 */
	template <class T1, class T2>
	concept left_shift_assignable_with = requires(T1 lhs, T2 rhs)
	{
		{ lhs <<= rhs };
	};

	/**
	 * \brief Determines whether two types can be used in operator <<= statements and checks if the return type
	 * is convertible to the expected.
	 * \tparam T1 The left-hand-side type
	 * \tparam T2 the right-hand-side-type
	 * \tparam TResult The expected return type
	 */
	template <class T1, class T2, class TResult = std::remove_cvref_t<T1>&>
	concept left_shift_assignable_with_r = requires(T1 lhs, T2 rhs)
	{
		{ lhs <<= rhs } -> std::convertible_to<TResult>;
	};

	/**
	 * \brief Determines whether the given type can be used on both sides of operator <<= statements.
	 * \tparam T The type to check
	 */
	template <class T>
	concept left_shift_assignable = left_shift_assignable_with<T, T>;

	/**
	 * \brief Determines whether the given type can be used on both sides of operator <<= statements and checks if the
	 * return type is convertible to the expected.
	 * \tparam T The type to check
	 * \tparam TResult  The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>&>
	concept left_shift_assignable_r = left_shift_assignable_with_r<T, T, TResult>;

	/**
	 * \brief Determines whether two types can be used in operator >>= and operator <<= statements.
	 * \tparam T1 The left-hand-side type
	 * \tparam T2 the right-hand-side-type
	 */
	template <class T1, class T2>
	concept bidirectional_shift_assignable_with = left_shift_assignable_with<T1, T2> && right_shift_assignable_with<T1, T2>;

	/**
	 * \brief Determines whether two types can be used in operator >>= and operator <<= statements and checks if the return type
	 * is convertible to the expected.
	 * \tparam T1 The left-hand-side type
	 * \tparam T2 the right-hand-side-type
	 * \tparam TResult The expected return type
	 */
	template <class T1, class T2, class TResult = std::remove_cvref_t<T1>&>
	concept bidirectional_shift_assignable_with_r = left_shift_assignable_with_r<T1, T2, TResult>
													&& right_shift_assignable_with_r<T1, T2, TResult>;

	/**
	 * \brief Determines whether the given type can be used on both sides of operator <<= and operator >>= statements.
	 * \tparam T The type to check
	 */
	template <class T>
	concept bidirectional_shift_assignable = bidirectional_shift_assignable_with<T, T>;

	/**
	 * \brief Determines whether the given type can be used on both sides of operator <<= and operator >>= statements and checks if the
	 * return type is convertible to the expected.
	 * \tparam T The type to check
	 * \tparam TResult  The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>&>
	concept bidirectional_shift_assignable_r = bidirectional_shift_assignable_with_r<T, T, TResult>;

	/**
	 * \brief Determines whether two types can be used in operator <<, >>, <<= and >>= statements.
	 * \tparam T1 The left-hand-side type
	 * \tparam T2 the right-hand-side-type
	 */
	template <class T1, class T2>
	concept fully_shiftable_with = bidirectional_shift_assignable_with<T1, T2> && bidirectional_shiftable_with<T1, T2>;

	/**
	 * \brief Determines whether two types can be used in operator <<, >>, <<= and >>= statements and checks if the return types
	 * are convertible to the expected ones.
	 * \tparam T1 The left-hand-side type
	 * \tparam T2 the right-hand-side-type
	 * \tparam TResult The expected return type of the non-assign operations
	 * \tparam TAssignResult The expected return type of shift assign operations
	 */
	template <class T1, class T2, class TResult = std::remove_cvref_t<T1>, class TAssignResult = std::remove_cvref_t<T1>&>
	concept fully_shiftable_with_r = bidirectional_shiftable_with_r<T1, T2, TResult>
									&& bidirectional_shift_assignable_with_r<T1, T2, TAssignResult>;

	/**
	 * \brief Determines whether the given type can be used on both sides of operator <<, >>, <<= and >>= statements.
	 * \tparam T The type to check
	 */
	template <class T>
	concept fully_shiftable = fully_shiftable_with<T, T>;

	/**
	 * \brief Determines whether the given type can be used on both sides of operator <<, >>, <<= and >>= statements and checks
	 * if the return types are convertible to the expected ones.
	 * \tparam T The type to check
	 * \tparam TResult The expected return type of the non-assign operations
	 * \tparam TAssignResult The expected return type of shift assign operations
	 */
	template <class T, class TResult = std::remove_cvref_t<T>, class TAssignResult = std::remove_cvref_t<T>&>
	concept fully_shiftable_r = fully_shiftable_with_r<T, T, TResult, TAssignResult>;

	/** @} */
	/** @} */
}

#endif
