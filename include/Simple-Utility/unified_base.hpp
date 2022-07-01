//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_UNIFIED_BASE_HPP
#define SL_UTILITY_UNIFIED_BASE_HPP

#pragma once

#include <concepts>
#include <type_traits>

namespace sl
{
	/**
	 * \brief Helper type, which can be used to create a unified base class for templated derived classes and thus
	 * getting an straight-forward way of disambiguing binary operators.
	 * \tparam TTag The provided tag type.
	 */
	template <class TTag>
		requires std::is_class_v<TTag> && std::same_as<TTag, std::remove_cvref_t<TTag>>
	struct unified_base
	{};

	/**
	 * \brief Determines whether T is derived of ``unified_base`` with the provided tag type.
	 * \tparam T The type to check.
	 * \tparam TTag The tag type to check for.
	 * \note This concepts makes use of ``std::is_base_of_v`` instead of ``std::derived_from`` because the former detects
	 * private inheritance, which is in fact the intended inheritance type.
	 */
	template <class T, class TTag>
	concept derived_from_unified_base = std::is_base_of_v<unified_base<TTag>, std::remove_cvref_t<T>>; 
}

#endif
