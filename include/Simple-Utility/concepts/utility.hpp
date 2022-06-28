//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_CONCEPTS_UTILITY_HPP
#define SL_UTILITY_CONCEPTS_UTILITY_HPP

#pragma once

#include <concepts>

namespace sl::concepts
{
	/**
	* \defgroup GROUP_UTILITY_CONCEPTS utility
	* \ingroup GROUP_CONCEPTS
	* @{
	*/

	/**
	 * \brief Trait for determining if all types of a parameter pack are unique. 
	 * \tparam T First type.
	 * \tparam TArgs Other additional types.
	 */
	template <class T, class... TArgs>
	inline constexpr bool are_types_unique_v = (!std::same_as<T, TArgs> && ...) && are_types_unique_v<TArgs...>;

	/**
	 * \brief Trait overload for single type parameter packs. Returns always true.
	 * \tparam T Single type.
	 */
	template <class T>
	inline constexpr bool are_types_unique_v<T> = true;

	/**
	 * \brief Checks whether each given type appears only once in the template argument pack.
	 * \tparam T The types to check.
	 */
	template <class... T>
	concept unique_types = are_types_unique_v<T...>;
}

namespace sl::concepts::detail
{
	template <class...>
	struct is_apply_invocable
		: std::false_type
	{};

	template <class TFunc, class TTuple, std::size_t... VIndices>
	struct is_apply_invocable<TFunc, TTuple, std::index_sequence<VIndices...>>
		: std::bool_constant<std::invocable<TFunc, std::tuple_element_t<VIndices, TTuple>...>>
	{};

	template <class TFunc, class TTuple>
	inline constexpr bool is_apply_invocable_v{
		is_apply_invocable<TFunc, TTuple, std::make_index_sequence<std::tuple_size_v<TTuple>>>::value
	};

	template <class...>
	struct is_nothrow_apply_invocable
		: std::false_type
	{};

	template <class TFunc, class TTuple, std::size_t... VIndices>
	struct is_nothrow_apply_invocable<TFunc, TTuple, std::index_sequence<VIndices...>>
		: std::bool_constant<std::is_nothrow_invocable_v<TFunc, std::tuple_element_t<VIndices, TTuple>...>>
	{};

	template <class TFunc, class TTuple>
	inline constexpr bool is_nothrow_apply_invocable_v{
		is_nothrow_apply_invocable<TFunc, TTuple, std::make_index_sequence<std::tuple_size_v<TTuple>>>::value
	};
}

namespace sl::concepts
{
	/**
	 * \brief Determines whether the function is invocable with the elements of the given tuple.
	 * \ingroup GROUP_TUPLE_UTILITY
	 */
	template <class TFunc, class TTuple>
	concept apply_invocable = detail::is_apply_invocable_v<TFunc, std::remove_reference_t<TTuple>>;

	/**
	 * \brief Determines whether the function is invocable with the elements of the given tuple without throwing.
	 * \ingroup GROUP_TUPLE_UTILITY
	 */
	template <class TFunc, class TTuple>
	concept nothrow_apply_invocable = apply_invocable<TFunc, TTuple>
									&& detail::is_nothrow_apply_invocable_v<TFunc, std::remove_reference_t<TTuple>>;

	/** @} */
}

#endif
