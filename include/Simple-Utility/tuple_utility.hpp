//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_TUPLE_UTILITY_HPP
#define SL_UTILITY_TUPLE_UTILITY_HPP

#pragma once

#include <concepts>
#include <tuple>
#include <type_traits>
#include <utility>

namespace sl::detail
{
	template <class...>
	struct is_apply_invocable
		: std::false_type
	{};

	template <class TFunc, class TTuple, std::size_t... VIndices>
	struct is_apply_invocable<TFunc, TTuple, std::index_sequence<VIndices...>>
		: std::bool_constant<std::invocable<TFunc, std::tuple_element_t<VIndices, TTuple>...>>
	{};

	template <class...>
	struct is_nothrow_apply_invocable
		: std::false_type
	{};

	template <class TFunc, class TTuple, std::size_t... VIndices>
	struct is_nothrow_apply_invocable<TFunc, TTuple, std::index_sequence<VIndices...>>
		: std::bool_constant<std::is_nothrow_invocable_v<TFunc, std::tuple_element_t<VIndices, TTuple>...>>
	{};
}

namespace sl
{
	/**
	 * \defgroup GROUP_TUPLE_UTILITY tuple utility
	 * @{
	 */

	/**
	 * \brief Trait type determining whether the function is invocable with elements of the given tuple.
	 */
	template <class TFunc, class TTuple>
	struct is_apply_invocable
		: detail::is_apply_invocable<TFunc, TTuple, std::make_index_sequence<std::tuple_size_v<TTuple>>>
	{};

	/**
	 * \brief Convenience constant, which determines whether the function is invocable with elements of the given tuple.
	 * \relates is_apply_invocable
	 */
	template <class TFunc, class TTuple>
	inline constexpr bool is_apply_invocable_v{ is_apply_invocable<TFunc, TTuple>::value };

	/**
	 * \brief Trait type determining whether the function is invocable with elements of the given tuple without throwing.
	 */
	template <class TFunc, class TTuple>
	struct is_nothrow_apply_invocable
		: detail::is_nothrow_apply_invocable<TFunc, TTuple, std::make_index_sequence<std::tuple_size_v<TTuple>>>
	{};

	/**
	 * \brief Convenience constant, which determines whether the function is invocable with elements of the given tuple
	 * without throwing.
	 * \relates is_nothrow_apply_invocable
	 */
	template <class TFunc, class TTuple>
	inline constexpr bool is_nothrow_apply_invocable_v{ is_nothrow_apply_invocable<TFunc, TTuple>::value };

	/**
	 * \brief Trait type determining the result of a ``std::tuple_cat`` call.
	 */
	template <class... TTuples>
	struct tuple_cat_result
	{
		using type = decltype(std::tuple_cat(std::declval<TTuples>()...));
	};

	/**
	 * \brief Alias type determining the result of a ``std::tuple_cat`` call.
	 */
	template <class... TTuples>
	using tuple_cat_result_t = typename tuple_cat_result<TTuples...>::type;

	/**
	 * \brief Trait type determining the result of a ``std::apply`` call.
	 */
	template <class TFunc, class TTuple>
	struct apply_invoke_result
	{
		using type = decltype(std::apply(std::declval<TFunc>(), std::declval<TTuple>()));
	};

	/**
	 * \brief Alias type determining the result of a ``std::apply`` call.
	 */
	template <class TFunc, class TTuple>
	using apply_invoke_result_t = typename apply_invoke_result<TFunc, TTuple>::type;

	/** @} */
}

#endif
