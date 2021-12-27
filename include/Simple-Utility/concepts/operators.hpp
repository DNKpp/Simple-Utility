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

	/**
	* \defgroup logical_operators logical operators
	* @{
	*/

	template <class T>
	concept complemented = requires(T t)
	{
		{ ~t };
	};

	template <class T, class TResult = std::remove_cvref_t<T>>
	concept complemented_r = requires(T t)
	{
		{ ~t } -> std::convertible_to<TResult>;
	};

	template <class T>
	concept invertible = requires(T t)
	{
		{ !t };
	};

	template <class T, class TResult = std::remove_cvref_t<T>>
	concept invertible_r = requires(T t)
	{
		{ !t } -> std::convertible_to<TResult>;
	};

	template <class TLhs, class TRhs>
	concept conjunctive_with = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs & rhs };
	};

	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>>
	concept conjunctive_with_r = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs & rhs } -> std::convertible_to<TResult>;
	};

	template <class T>
	concept conjunctive = conjunctive_with<T, T>;

	template <class T, class TResult = std::remove_cvref_t<T>>
	concept conjunctive_r = conjunctive_with_r<T, T, TResult>;

	template <class TLhs, class TRhs>
	concept conjunctive_assign_with = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs &= rhs };
	};

	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>&>
	concept conjunctive_assign_with_r = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs &= rhs } -> std::convertible_to<TResult>;
	};

	template <class T>
	concept conjunctive_assign = conjunctive_assign_with<T, T>;

	template <class T, class TResult = std::remove_cvref_t<T>&>
	concept conjunctive_assign_r = conjunctive_assign_with_r<T, T, TResult>;

	template <class TLhs, class TRhs>
	concept disjunctive_with = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs | rhs };
	};

	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>>
	concept disjunctive_with_r = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs | rhs } -> std::convertible_to<TResult>;
	};

	template <class T>
	concept disjunctive = disjunctive_with<T, T>;

	template <class T, class TResult = std::remove_cvref_t<T>>
	concept disjunctive_r = disjunctive_with_r<T, T, TResult>;

	template <class TLhs, class TRhs>
	concept disjunctive_assign_with = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs |= rhs };
	};

	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>&>
	concept disjunctive_assign_with_r = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs |= rhs } -> std::convertible_to<TResult>;
	};

	template <class T>
	concept disjunctive_assign = disjunctive_assign_with<T, T>;

	template <class T, class TResult = std::remove_cvref_t<T>&>
	concept disjunctive_assign_r = disjunctive_assign_with_r<T, T, TResult>;

	template <class TLhs, class TRhs>
	concept exclusive_disjunctive_with = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs ^ rhs };
	};

	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>>
	concept exclusive_disjunctive_with_r = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs ^ rhs } -> std::convertible_to<TResult>;
	};

	template <class T>
	concept exclusive_disjunctive = exclusive_disjunctive_with<T, T>;

	template <class T, class TResult = std::remove_cvref_t<T>>
	concept exclusive_disjunctive_r = exclusive_disjunctive_with_r<T, T, TResult>;

	template <class TLhs, class TRhs>
	concept exclusive_disjunctive_assign_with = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs ^= rhs };
	};

	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>&>
	concept exclusive_disjunctive_assign_with_r = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs ^= rhs } -> std::convertible_to<TResult>;
	};

	template <class T>
	concept exclusive_disjunctive_assign = exclusive_disjunctive_assign_with<T, T>;

	template <class T, class TResult = std::remove_cvref_t<T>&>
	concept exclusive_disjunctive_assign_r = exclusive_disjunctive_assign_with_r<T, T, TResult>;

	template <class TLhs, class TRhs>
	concept logically_combinable_with = conjunctive_with<TLhs, TRhs>
										&& disjunctive_with<TLhs, TRhs>
										&& exclusive_disjunctive_with<TLhs, TRhs>;

	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>>
	concept logically_combinable_with_r = conjunctive_with_r<TLhs, TRhs, TResult>
										&& disjunctive_with_r<TLhs, TRhs, TResult>
										&& exclusive_disjunctive_with_r<TLhs, TRhs, TResult>;

	template <class T>
	concept logically_combinable = conjunctive<T>
									&& disjunctive<T>
									&& exclusive_disjunctive<T>;

	template <class T, class TResult = std::remove_cvref_t<T>>
	concept logically_combinable_r = conjunctive_r<T, TResult>
									&& disjunctive_r<T, TResult>
									&& exclusive_disjunctive_r<T, TResult>;

	template <class TLhs, class TRhs>
	concept logically_assignable_with = conjunctive_assign_with<TLhs, TRhs>
										&& disjunctive_assign_with<TLhs, TRhs>
										&& exclusive_disjunctive_assign_with<TLhs, TRhs>;

	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>&>
	concept logically_assignable_with_r = conjunctive_assign_with_r<TLhs, TRhs, TResult>
										&& disjunctive_assign_with_r<TLhs, TRhs, TResult>
										&& exclusive_disjunctive_assign_with_r<TLhs, TRhs, TResult>;

	template <class T>
	concept logically_assignable = conjunctive_assign<T>
									&& disjunctive_assign<T>
									&& exclusive_disjunctive_assign<T>;

	template <class T, class TResult = std::remove_cvref_t<T>&>
	concept logically_assignable_r = conjunctive_assign_r<T, TResult>
									&& disjunctive_assign_r<T, TResult>
									&& exclusive_disjunctive_assign_r<T, TResult>;
	/** @} */
	/** @} */
}

#endif
