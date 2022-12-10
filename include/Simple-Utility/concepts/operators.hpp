//          Copyright Dominic Koepke 2019 - 2022.
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
	* \defgroup GROUP_CONCEPTS concepts
	*
	* \brief Contains concepts for different purposes.
	**/

	/**
	* \defgroup GROUP_SHIFT_OPERATOR_CONCEPTS shift operators
	*
	* \brief Contains concepts which determines whether a type is usable in operator <<, <<=, >> and >>= expressions.
	* \ingroup GROUP_CONCEPTS
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
	* \defgroup GROUP_LOGICAL_ARITHMETIC_OPERATOR_CONCEPTS logical arithmetic operators
	*
	* \brief Contains concepts which determines whether a type is usable in operator &, &=, |, |=, ^, ^= and ~  expressions.
	* \ingroup GROUP_CONCEPTS
	* @{
	*/

	/**
	 * \brief Determines whether a type can be used in operator ~ expression.
	 * \tparam T The type to check
	 */
	template <class T>
	concept complemented = requires(T t)
	{
		{ ~t };
	};

	/**
	 * \brief Determines whether a type can be used in operator ~ expression and if the return type is convertible
	 * to the expected one.
	 * \tparam T The type to check
	 * \tparam TResult The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>>
	concept complemented_r = requires(T t)
	{
		{ ~t } -> std::convertible_to<TResult>;
	};

	/**
	 * \brief Determines whether two types can be used in operator & expressions.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 */
	template <class TLhs, class TRhs>
	concept conjunctive_with = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs & rhs };
	};

	/**
	 * \brief Determines whether two types can be used in operator & expressions and if the return type is convertible
	 * to the expected one.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 * \tparam TResult The expected return type
	 */
	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>>
	concept conjunctive_with_r = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs & rhs } -> std::convertible_to<TResult>;
	};

	/**
	 * \brief Determines whether a type can be used on both sides of operator & expressions.
	 * \tparam T The type to check
	 */
	template <class T>
	concept conjunctive = conjunctive_with<T, T>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator & expressions and if the
	 * return type is convertible to the expected one.
	 * \tparam T The type to check
	 * \tparam TResult The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>>
	concept conjunctive_r = conjunctive_with_r<T, T, TResult>;

	/**
	 * \brief Determines whether two types can be used in operator &= expressions.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 */
	template <class TLhs, class TRhs>
	concept conjunctive_assign_with = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs &= rhs };
	};

	/**
	 * \brief Determines whether two types can be used in operator &= expressions and if the return type is convertible
	 * to the expected one.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 * \tparam TResult The expected return type
	 */
	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>&>
	concept conjunctive_assign_with_r = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs &= rhs } -> std::convertible_to<TResult>;
	};

	/**
	 * \brief Determines whether a type can be used on both sides of operator &= expressions.
	 * \tparam T The type to check
	 */
	template <class T>
	concept conjunctive_assign = conjunctive_assign_with<T, T>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator &= expressions and if the
	 * return type is convertible to the expected one.
	 * \tparam T The type to check
	 * \tparam TResult The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>&>
	concept conjunctive_assign_r = conjunctive_assign_with_r<T, T, TResult>;

	/**
	 * \brief Determines whether two types can be used in operator | expressions.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 */
	template <class TLhs, class TRhs>
	concept disjunctive_with = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs | rhs };
	};

	/**
	 * \brief Determines whether two types can be used in operator | expressions and if the return type is convertible
	 * to the expected one.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 * \tparam TResult The expected return type
	 */
	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>>
	concept disjunctive_with_r = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs | rhs } -> std::convertible_to<TResult>;
	};

	/**
	 * \brief Determines whether a type can be used on both sides of operator | expressions.
	 * \tparam T The type to check
	 */
	template <class T>
	concept disjunctive = disjunctive_with<T, T>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator | expressions and if the
	 * return type is convertible to the expected one.
	 * \tparam T The type to check
	 * \tparam TResult The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>>
	concept disjunctive_r = disjunctive_with_r<T, T, TResult>;

	/**
	 * \brief Determines whether two types can be used in operator |= expressions.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 */
	template <class TLhs, class TRhs>
	concept disjunctive_assign_with = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs |= rhs };
	};

	/**
	 * \brief Determines whether two types can be used in operator |= expressions and if the return type is convertible
	 * to the expected one.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 * \tparam TResult The expected return type
	 */
	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>&>
	concept disjunctive_assign_with_r = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs |= rhs } -> std::convertible_to<TResult>;
	};

	/**
	 * \brief Determines whether a type can be used on both sides of operator |= expressions.
	 * \tparam T The type to check
	 */
	template <class T>
	concept disjunctive_assign = disjunctive_assign_with<T, T>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator |= expressions and if the
	 * return type is convertible to the expected one.
	 * \tparam T The type to check
	 * \tparam TResult The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>&>
	concept disjunctive_assign_r = disjunctive_assign_with_r<T, T, TResult>;

	/**
	 * \brief Determines whether two types can be used in operator ^ expressions.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 */
	template <class TLhs, class TRhs>
	concept exclusive_disjunctive_with = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs ^ rhs };
	};

	/**
	 * \brief Determines whether two types can be used in operator ^ expressions and if the return type is convertible
	 * to the expected one.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 * \tparam TResult The expected return type
	 */
	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>>
	concept exclusive_disjunctive_with_r = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs ^ rhs } -> std::convertible_to<TResult>;
	};

	/**
	 * \brief Determines whether a type can be used on both sides of operator ^ expressions.
	 * \tparam T The type to check
	 */
	template <class T>
	concept exclusive_disjunctive = exclusive_disjunctive_with<T, T>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator ^ expressions and if the
	 * return type is convertible to the expected one.
	 * \tparam T The type to check
	 * \tparam TResult The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>>
	concept exclusive_disjunctive_r = exclusive_disjunctive_with_r<T, T, TResult>;

	/**
	 * \brief Determines whether two types can be used in operator ^= expressions.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 */
	template <class TLhs, class TRhs>
	concept exclusive_disjunctive_assign_with = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs ^= rhs };
	};

	/**
	 * \brief Determines whether two types can be used in operator ^= expressions and if the return type is convertible
	 * to the expected one.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 * \tparam TResult The expected return type
	 */
	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>&>
	concept exclusive_disjunctive_assign_with_r = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs ^= rhs } -> std::convertible_to<TResult>;
	};

	/**
	 * \brief Determines whether a type can be used on both sides of operator ^= expressions.
	 * \tparam T The type to check
	 */
	template <class T>
	concept exclusive_disjunctive_assign = exclusive_disjunctive_assign_with<T, T>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator ^= expressions and if the
	 * return type is convertible to the expected one.
	 * \tparam T The type to check
	 * \tparam TResult The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>&>
	concept exclusive_disjunctive_assign_r = exclusive_disjunctive_assign_with_r<T, T, TResult>;

	/**
	 * \brief Determines whether two types can be used in operator &, | amd ^ expressions.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 */
	template <class TLhs, class TRhs>
	concept logically_combinable_with = conjunctive_with<TLhs, TRhs>
										&& disjunctive_with<TLhs, TRhs>
										&& exclusive_disjunctive_with<TLhs, TRhs>;

	/**
	 * \brief Determines whether two types can be used in operator &, | amd ^ expressions and if the return type is convertible
	 * to the expected one.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 * \tparam TResult The expected return type
	 */
	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>>
	concept logically_combinable_with_r = conjunctive_with_r<TLhs, TRhs, TResult>
										&& disjunctive_with_r<TLhs, TRhs, TResult>
										&& exclusive_disjunctive_with_r<TLhs, TRhs, TResult>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator &, | and ^ expressions.
	 * \tparam T The type to check
	 */
	template <class T>
	concept logically_combinable = conjunctive<T>
									&& disjunctive<T>
									&& exclusive_disjunctive<T>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator &, | and ^ expressions and if the
	 * return type is convertible to the expected one.
	 * \tparam T The type to check
	 * \tparam TResult The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>>
	concept logically_combinable_r = conjunctive_r<T, TResult>
									&& disjunctive_r<T, TResult>
									&& exclusive_disjunctive_r<T, TResult>;

	/**
	 * \brief Determines whether two types can be used in operator &=, |= and ^= expressions.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 */
	template <class TLhs, class TRhs>
	concept logically_assignable_with = conjunctive_assign_with<TLhs, TRhs>
										&& disjunctive_assign_with<TLhs, TRhs>
										&& exclusive_disjunctive_assign_with<TLhs, TRhs>;

	/**
	 * \brief Determines whether two types can be used in operator &=, |= and ^= expressions and if the return type is convertible
	 * to the expected one.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 * \tparam TResult The expected return type
	 */
	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>&>
	concept logically_assignable_with_r = conjunctive_assign_with_r<TLhs, TRhs, TResult>
										&& disjunctive_assign_with_r<TLhs, TRhs, TResult>
										&& exclusive_disjunctive_assign_with_r<TLhs, TRhs, TResult>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator &=, |= and ^= expressions.
	 * \tparam T The type to check
	 */
	template <class T>
	concept logically_assignable = conjunctive_assign<T>
									&& disjunctive_assign<T>
									&& exclusive_disjunctive_assign<T>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator &=, |= and ^= expressions and if the
	 * return type is convertible to the expected one.
	 * \tparam T The type to check
	 * \tparam TResult The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>&>
	concept logically_assignable_r = conjunctive_assign_r<T, TResult>
									&& disjunctive_assign_r<T, TResult>
									&& exclusive_disjunctive_assign_r<T, TResult>;

	/**
	 * \brief Determines whether two types can be used in operator &, |, ^, &=, |=, ^= and ~ expressions.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 */
	template <class TLhs, class TRhs>
	concept fully_logical_with = complemented<TLhs>
								&& logically_combinable_with<TLhs, TRhs>
								&& logically_assignable_with<TLhs, TRhs>;

	/**
	 * \brief Determines whether two types can be used in operator &, |, ^, &=, |=, ^= and ~ expressions and if the return types
	 * are convertible to the expected ones.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 * \tparam TCombineResult The expected return type of combining operations
	 * \tparam TAssignResult The expected return type of assigning operations
	 */
	template <class TLhs,
			class TRhs,
			class TCombineResult = std::remove_cvref_t<TLhs>,
			class TAssignResult = std::remove_cvref_t<TLhs>&>
	concept fully_logical_with_r = complemented_r<TLhs, TCombineResult>
									&& logically_combinable_with_r<TLhs, TRhs, TCombineResult>
									&& logically_assignable_with_r<TLhs, TRhs, TAssignResult>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator &, |, ^, &=, |=, ^= and ~ expressions.
	 * \tparam T The type to check
	 */
	template <class T>
	concept fully_logical = complemented<T>
							&& logically_combinable<T>
							&& logically_assignable<T>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator &, |, ^, &=, |=, ^= and ~ expressions and if the
	 * return types are convertible to the expected ones.
	 * \tparam T The type to check
	 * \tparam TCombineResult The expected return type of combining operations
	 * \tparam TAssignResult The expected return type of assigning operations
	 */
	template <class T, class TCombineResult = std::remove_cvref_t<T>, class TAssignResult = std::remove_cvref_t<T>&>
	concept fully_logical_r = complemented_r<T, TCombineResult>
							&& logically_combinable_r<T, TCombineResult>
							&& logically_assignable_r<T, TAssignResult>;

	/** @} */

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

	/**
	* \defgroup GROUP_ACCESS_OPERATOR_CONCEPTS access operators
	*
	* \ingroup GROUP_CONCEPTS
	* @{
	*/

	/**
	 * \brief Determines whether a type can be used in unary operator * expressions.
	 * \tparam T The type to check
	 */
	template <class T>
	concept dereferencable = requires(T t) { *t; };

	/**
	 * \brief Determines whether a type can be used in unary operator * expressions and if the return type is convertible
	 * to the expected one.
	 * \tparam T The type to check
	 * \tparam TReturn The expected return type
	 */
	template <class T, class TReturn>
	concept dereferencable_r = requires(T t)
	{
		{ *t } -> std::convertible_to<TReturn>;
	};

	/**
	 * \brief Determines whether a type can be used in operator -> expressions.
	 * \tparam T The type to check
	 */
	template <class T>
	concept arrow_dereferencable = requires(T t) { t.operator ->(); };

	/**
	 * \brief Determines whether a type can be used in operator -> expressions and if the return type is convertible
	 * to the expected one.
	 * \tparam T The type to check
	 * \tparam TReturn The expected return type
	 */
	template <class T, class TReturn>
	concept arrow_dereferencable_r = requires(T t)
	{
		{ t.operator ->() } -> std::convertible_to<TReturn>;
	};

	/**
	 * \brief Determines whether a type can be used in operator [] expressions.
	 * \tparam T The type to check
	 * \tparam TIndex The index type
	 */
	template <class T, class TIndex>
	concept index_dereferencable = requires(T t, TIndex i) { t[i]; };

	/**
	 * \brief Determines whether a type can be used in operator -> expressions and if the return type is convertible
	 * to the expected one.
	 * \tparam T The type to check
	 * \tparam TIndex The index type
	 * \tparam TReturn The expected return type
	 */
	template <class T, class TIndex, class TReturn>
	concept index_dereferencable_r = requires(T t, TIndex i)
	{
		{ t[i] } -> std::convertible_to<TReturn>;
	};

	/** @} */











	/**
	* \defgroup GROUP_ARITHMETIC_OPERATOR_CONCEPTS arithmetic operators
	*
	* \brief Contains concepts which determines whether a type is usable in operator +, +=, -, -=, *, *=, /, /=, %, %= and unary - expressions.
	* \ingroup GROUP_CONCEPTS
	* @{
	*/

	/**
	 * \brief Determines whether a type can be used in operator - expression.
	 * \tparam T The type to check
	 */
	template <class T>
	concept negate = requires(T t)
	{
		{ -t };
	};

	/**
	 * \brief Determines whether a type can be used in operator - expression and if the return type is convertible
	 * to the expected one.
	 * \tparam T The type to check
	 * \tparam TResult The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>>
	concept negate_r = requires(T t)
	{
		{ -t } -> std::convertible_to<TResult>;
	};

	/**
	 * \brief Determines whether two types can be used in operator + expressions.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 */
	template <class TLhs, class TRhs>
	concept plus_with = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs + rhs };
	};

	/**
	 * \brief Determines whether two types can be used in operator + expressions and if the return type is convertible
	 * to the expected one.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 * \tparam TResult The expected return type
	 */
	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>>
	concept plus_with_r = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs + rhs } -> std::convertible_to<TResult>;
	};

	/**
	 * \brief Determines whether a type can be used on both sides of operator + expressions.
	 * \tparam T The type to check
	 */
	template <class T>
	concept plus = plus_with<T, T>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator + expressions and if the
	 * return type is convertible to the expected one.
	 * \tparam T The type to check
	 * \tparam TResult The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>>
	concept plus_r = plus_with_r<T, T, TResult>;

	/**
	 * \brief Determines whether two types can be used in operator += expressions.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 */
	template <class TLhs, class TRhs>
	concept plus_assign_with = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs += rhs };
	};

	/**
	 * \brief Determines whether two types can be used in operator += expressions and if the return type is convertible
	 * to the expected one.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 * \tparam TResult The expected return type
	 */
	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>&>
	concept plus_assign_with_r = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs += rhs } -> std::convertible_to<TResult>;
	};

	/**
	 * \brief Determines whether a type can be used on both sides of operator += expressions.
	 * \tparam T The type to check
	 */
	template <class T>
	concept plus_assign = plus_assign_with<T, T>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator += expressions and if the
	 * return type is convertible to the expected one.
	 * \tparam T The type to check
	 * \tparam TResult The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>&>
	concept plus_assign_r = plus_assign_with_r<T, T, TResult>;

	/**
	 * \brief Determines whether two types can be used in operator - expressions.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 */
	template <class TLhs, class TRhs>
	concept minus_with = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs - rhs };
	};

	/**
	 * \brief Determines whether two types can be used in operator - expressions and if the return type is convertible
	 * to the expected one.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 * \tparam TResult The expected return type
	 */
	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>>
	concept minus_with_r = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs - rhs } -> std::convertible_to<TResult>;
	};

	/**
	 * \brief Determines whether a type can be used on both sides of operator - expressions.
	 * \tparam T The type to check
	 */
	template <class T>
	concept minus = minus_with<T, T>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator - expressions and if the
	 * return type is convertible to the expected one.
	 * \tparam T The type to check
	 * \tparam TResult The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>>
	concept minus_r = minus_with_r<T, T, TResult>;

	/**
	 * \brief Determines whether two types can be used in operator -= expressions.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 */
	template <class TLhs, class TRhs>
	concept minus_assign_with = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs -= rhs };
	};

	/**
	 * \brief Determines whether two types can be used in operator -= expressions and if the return type is convertible
	 * to the expected one.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 * \tparam TResult The expected return type
	 */
	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>&>
	concept minus_assign_with_r = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs -= rhs } -> std::convertible_to<TResult>;
	};

	/**
	 * \brief Determines whether a type can be used on both sides of operator -= expressions.
	 * \tparam T The type to check
	 */
	template <class T>
	concept minus_assign = minus_assign_with<T, T>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator -= expressions and if the
	 * return type is convertible to the expected one.
	 * \tparam T The type to check
	 * \tparam TResult The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>&>
	concept minus_assign_r = minus_assign_with_r<T, T, TResult>;

	/**
	 * \brief Determines whether two types can be used in operator * expressions.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 */
	template <class TLhs, class TRhs>
	concept multiplies_with = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs * rhs };
	};

	/**
	 * \brief Determines whether two types can be used in operator * expressions and if the return type is convertible
	 * to the expected one.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 * \tparam TResult The expected return type
	 */
	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>>
	concept multiplies_with_r = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs * rhs } -> std::convertible_to<TResult>;
	};

	/**
	 * \brief Determines whether a type can be used on both sides of operator * expressions.
	 * \tparam T The type to check
	 */
	template <class T>
	concept multiplies = multiplies_with<T, T>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator * expressions and if the
	 * return type is convertible to the expected one.
	 * \tparam T The type to check
	 * \tparam TResult The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>>
	concept multiplies_r = multiplies_with_r<T, T, TResult>;

	/**
	 * \brief Determines whether two types can be used in operator *= expressions.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 */
	template <class TLhs, class TRhs>
	concept multiplies_assign_with = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs *= rhs };
	};

	/**
	 * \brief Determines whether two types can be used in operator *= expressions and if the return type is convertible
	 * to the expected one.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 * \tparam TResult The expected return type
	 */
	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>&>
	concept multiplies_assign_with_r = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs *= rhs } -> std::convertible_to<TResult>;
	};

	/**
	 * \brief Determines whether a type can be used on both sides of operator *= expressions.
	 * \tparam T The type to check
	 */
	template <class T>
	concept multiplies_assign = multiplies_assign_with<T, T>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator *= expressions and if the
	 * return type is convertible to the expected one.
	 * \tparam T The type to check
	 * \tparam TResult The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>&>
	concept multiplies_assign_r = multiplies_assign_with_r<T, T, TResult>;

	/**
	 * \brief Determines whether two types can be used in operator / expressions.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 */
	template <class TLhs, class TRhs>
	concept divides_with = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs / rhs };
	};

	/**
	 * \brief Determines whether two types can be used in operator / expressions and if the return type is convertible
	 * to the expected one.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 * \tparam TResult The expected return type
	 */
	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>>
	concept divides_with_r = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs / rhs } -> std::convertible_to<TResult>;
	};

	/**
	 * \brief Determines whether a type can be used on both sides of operator / expressions.
	 * \tparam T The type to check
	 */
	template <class T>
	concept divides = divides_with<T, T>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator / expressions and if the
	 * return type is convertible to the expected one.
	 * \tparam T The type to check
	 * \tparam TResult The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>>
	concept divides_r = divides_with_r<T, T, TResult>;

	/**
	 * \brief Determines whether two types can be used in operator /= expressions.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 */
	template <class TLhs, class TRhs>
	concept divides_assign_with = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs /= rhs };
	};

	/**
	 * \brief Determines whether two types can be used in operator /= expressions and if the return type is convertible
	 * to the expected one.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 * \tparam TResult The expected return type
	 */
	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>&>
	concept divides_assign_with_r = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs /= rhs } -> std::convertible_to<TResult>;
	};

	/**
	 * \brief Determines whether a type can be used on both sides of operator /= expressions.
	 * \tparam T The type to check
	 */
	template <class T>
	concept divides_assign = divides_assign_with<T, T>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator /= expressions and if the
	 * return type is convertible to the expected one.
	 * \tparam T The type to check
	 * \tparam TResult The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>&>
	concept divides_assign_r = divides_assign_with_r<T, T, TResult>;

	/**
	 * \brief Determines whether two types can be used in operator % expressions.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 */
	template <class TLhs, class TRhs>
	concept modulus_with = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs % rhs };
	};

	/**
	 * \brief Determines whether two types can be used in operator % expressions and if the return type is convertible
	 * to the expected one.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 * \tparam TResult The expected return type
	 */
	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>>
	concept modulus_with_r = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs % rhs } -> std::convertible_to<TResult>;
	};

	/**
	 * \brief Determines whether a type can be used on both sides of operator % expressions.
	 * \tparam T The type to check
	 */
	template <class T>
	concept modulus = modulus_with<T, T>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator % expressions and if the
	 * return type is convertible to the expected one.
	 * \tparam T The type to check
	 * \tparam TResult The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>>
	concept modulus_r = modulus_with_r<T, T, TResult>;

	/**
	 * \brief Determines whether two types can be used in operator %= expressions.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 */
	template <class TLhs, class TRhs>
	concept modulus_assign_with = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs %= rhs };
	};

	/**
	 * \brief Determines whether two types can be used in operator %= expressions and if the return type is convertible
	 * to the expected one.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 * \tparam TResult The expected return type
	 */
	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>&>
	concept modulus_assign_with_r = requires(TLhs lhs, TRhs rhs)
	{
		{ lhs %= rhs } -> std::convertible_to<TResult>;
	};

	/**
	 * \brief Determines whether a type can be used on both sides of operator %= expressions.
	 * \tparam T The type to check
	 */
	template <class T>
	concept modulus_assign = modulus_assign_with<T, T>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator %= expressions and if the
	 * return type is convertible to the expected one.
	 * \tparam T The type to check
	 * \tparam TResult The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>&>
	concept modulus_assign_r = modulus_assign_with_r<T, T, TResult>;

	/**
	 * \brief Determines whether two types can be used in operator +, -, * and / expressions.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 */
	template <class TLhs, class TRhs>
	concept arithmetically_combinable_with = plus_with<TLhs, TRhs>
										&& minus_with<TLhs, TRhs>
										&& multiplies_with<TLhs, TRhs>
										&& divides_with<TLhs, TRhs>;

	/**
	 * \brief Determines whether two types can be used in operator +, -, * and / expressions and if the return type is convertible
	 * to the expected one.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 * \tparam TResult The expected return type
	 */
	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>>
	concept arithmetically_combinable_with_r = plus_with_r<TLhs, TRhs, TResult>
										&& minus_with_r<TLhs, TRhs, TResult>
										&& multiplies_with_r<TLhs, TRhs, TResult>
										&& divides_with_r<TLhs, TRhs, TResult>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator +, -, * and / expressions.
	 * \tparam T The type to check
	 */
	template <class T>
	concept arithmetically_combinable = plus<T>
									&& minus<T>
									&& multiplies<T>
									&& divides<T>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator +, -, * and / expressions and if the
	 * return type is convertible to the expected one.
	 * \tparam T The type to check
	 * \tparam TResult The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>>
	concept arithmetically_combinable_r = plus_r<T, TResult>
									&& minus_r<T, TResult>
									&& multiplies_r<T, TResult>
									&& divides_r<T, TResult>;

	/**
	 * \brief Determines whether two types can be used in operator +=, -=, *= and /= expressions.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 */
	template <class TLhs, class TRhs>
	concept arithmetically_assignable_with = plus_assign_with<TLhs, TRhs>
										&& minus_assign_with<TLhs, TRhs>
										&& multiplies_assign_with<TLhs, TRhs>
										&& divides_assign_with<TLhs, TRhs>;

	/**
	 * \brief Determines whether two types can be used in operator +=, -=, *= and /= expressions and if the return type is convertible
	 * to the expected one.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 * \tparam TResult The expected return type
	 */
	template <class TLhs, class TRhs, class TResult = std::remove_cvref_t<TLhs>&>
	concept arithmetically_assignable_with_r = plus_assign_with_r<TLhs, TRhs, TResult>
										&& minus_assign_with_r<TLhs, TRhs, TResult>
										&& multiplies_assign_with_r<TLhs, TRhs, TResult>
										&& divides_assign_with_r<TLhs, TRhs, TResult>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator +=, -=, *= and /= expressions.
	 * \tparam T The type to check
	 */
	template <class T>
	concept arithmetically_assignable = plus_assign<T>
									&& minus_assign<T>
									&& multiplies_assign<T>
									&& divides_assign<T>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator +=, -=, *= and /= expressions and if the
	 * return type is convertible to the expected one.
	 * \tparam T The type to check
	 * \tparam TResult The expected return type
	 */
	template <class T, class TResult = std::remove_cvref_t<T>&>
	concept arithmetically_assignable_r = plus_assign_r<T, TResult>
									&& minus_assign_r<T, TResult>
									&& multiplies_assign_r<T, TResult>
									&& divides_assign_r<T, TResult>;

	/**
	 * \brief Determines whether two types can be used in operator +, -, *, /, +=, -=, *=, /= and unary - expressions.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 */
	template <class TLhs, class TRhs>
	concept fully_arithmetical_with = negate<TLhs>
								&& arithmetically_combinable_with<TLhs, TRhs>
								&& arithmetically_assignable_with<TLhs, TRhs>;

	/**
	 * \brief Determines whether two types can be used in operator +, -, *, /, +=, -=, *=, /= and unary - expressions and if the return types
	 * are convertible to the expected ones.
	 * \tparam TLhs The left-hand-side type
	 * \tparam TRhs the right-hand-side-type
	 * \tparam TCombineResult The expected return type of combining operations
	 * \tparam TAssignResult The expected return type of assigning operations
	 */
	template <class TLhs,
			class TRhs,
			class TCombineResult = std::remove_cvref_t<TLhs>,
			class TAssignResult = std::remove_cvref_t<TLhs>&>
	concept fully_arithmetical_with_r = negate_r<TLhs, TCombineResult>
									&& arithmetically_combinable_with_r<TLhs, TRhs, TCombineResult>
									&& arithmetically_assignable_with_r<TLhs, TRhs, TAssignResult>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator +, -, *, /, +=, -=, *=, /= and unary - expressions.
	 * \tparam T The type to check
	 */
	template <class T>
	concept fully_arithmetical = negate<T>
							&& arithmetically_combinable<T>
							&& arithmetically_assignable<T>;

	/**
	 * \brief Determines whether a type can be used on both sides of operator +, -, *, /, +=, -=, *=, /= and unary - expressions and if the
	 * return types are convertible to the expected ones.
	 * \tparam T The type to check
	 * \tparam TCombineResult The expected return type of combining operations
	 * \tparam TAssignResult The expected return type of assigning operations
	 */
	template <class T, class TCombineResult = std::remove_cvref_t<T>, class TAssignResult = std::remove_cvref_t<T>&>
	concept fully_arithmetical_r = negate_r<T, TCombineResult>
							&& arithmetically_combinable_r<T, TCombineResult>
							&& arithmetically_assignable_r<T, TAssignResult>;

	/** @} */
}

#endif
