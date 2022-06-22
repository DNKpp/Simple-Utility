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

	/** @} */
}

#endif
