//          Copyright Dominic Koepke 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_STL_SYMMETRICAL_HPP
#define SL_STL_SYMMETRICAL_HPP

#pragma once

#include <concepts>

// ReSharper disable CppClangTidyClangDiagnosticDocumentation

namespace sl::concepts
{
	/**
	* @addtogroup concepts
	*
	* @{
	*/

	/**
	* @defgroup stl_counterpart_concepts stl counterparts
	*
	* \brief Contains several counterpart concepts for existing stl concepts.
	* @{
	*/

	/**
	 * \brief Checks whether the left-hand-side type is unequal to the right-hand-side type.
	 * \details This is the inverted counterpart of ``std::same_as`` concept.
	 * \tparam TLhs The source type handed over to the target
	 * \tparam TRhs The target type to check
	 */
	template <class TLhs, class TRhs>
	concept not_same_as = !std::same_as<TLhs, TRhs>;

	/**
	 * \brief Checks whether the target type is constructible from the source type.
	 * \details This is the symmetrical counterpart of ``std::constructible_from`` concept with a single constructor argument.
	 * \tparam TSource The source type handed over to the constructor of the target
	 * \tparam TTarget The target type to check
	 */
	template <class TSource, class TTarget>
	concept constructs = std::constructible_from<TTarget, TSource>;

	/**
	 * \brief Checks whether the target type is assignable from the source type.
	 * \details This is the symmetrical counterpart of ``std::assignable_from`` concept.
	 * \tparam TSource The source type handed over to the target
	 * \tparam TTarget The target type to check
	 */
	template <class TSource, class TTarget>
	concept assignable_to = std::assignable_from<TTarget, TSource>;

	/** @} */
	/** @} */
}

#endif
