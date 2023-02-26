//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_CONCEPTS_UTILITY_HPP
#define SL_UTILITY_CONCEPTS_UTILITY_HPP

#pragma once

#include "Simple-Utility/tuple_utility.hpp"

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

	/** @} */
}

namespace sl::concepts::detail
{
	template <class TTuple>
	consteval bool has_tuple_element()
	{
		return []<std::size_t... VIndices>([[maybe_unused]] std::index_sequence<VIndices...>)
		{
			return (requires { typename std::tuple_element_t<VIndices, TTuple>; } && ...);
		}(std::make_index_sequence<std::tuple_size_v<TTuple>>{});
	}
}

namespace sl::concepts
{
	/**
	* \addtogroup GROUP_UTILITY_CONCEPTS
	* @{
	*/

	/**
	 * \brief Determines whether a type can be used as a tuple-like.
	 * \details Requires the type to have a specialization of ```std::tuple_size`` and for each index
	 * in the interval ``[0, N)``, the ``std::tuple_element`` trait must yield a valid type.
	 * \concept tuple_like
	 * \ingroup GROUP_TUPLE_UTILITY
	 */
	template <class TTuple>
	concept tuple_like = requires
						{
							typename std::tuple_size<TTuple>::type;
							{ std::tuple_size_v<TTuple> } -> std::convertible_to<std::size_t>;
						}
						&& detail::has_tuple_element<TTuple>();

	/**
	 * \brief Determines whether the function is invocable with the elements of the given tuple.
	 * \ingroup GROUP_TUPLE_UTILITY
	 */
	template <class TFunc, class TTuple>
	concept apply_invocable = is_apply_invocable_v<TFunc, std::remove_reference_t<TTuple>>;

	/**
	 * \brief Determines whether the function is invocable with the elements of the given tuple without throwing.
	 * \ingroup GROUP_TUPLE_UTILITY
	 */
	template <class TFunc, class TTuple>
	concept nothrow_apply_invocable = apply_invocable<TFunc, TTuple>
									&& is_nothrow_apply_invocable_v<TFunc, std::remove_reference_t<TTuple>>;

	/** @} */
}

#endif
