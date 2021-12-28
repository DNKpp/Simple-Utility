//          Copyright Dominic Koepke 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UNIQUE_HANDLE_HPP
#define SL_UNIQUE_HANDLE_HPP

#pragma once

#include <concepts>

// ReSharper disable CppClangTidyClangDiagnosticDocumentation

namespace sl::concepts
{
	/**
	* \addtogroup concepts
	* @{
	*/

	/**
	* \defgroup stl_symmetrical stl symmetrical
	* \brief This group offers symmetrical counterparts concepts to already existing stl concepts.
	* @{
	*/

	/**
	 * \brief Checks whether the target type is constructible from the source type.
	 * \detail This is the symmetrical counterpart of ``std::constructible_from`` concept with a single constructor argument.
	 * \tparam TSource The source type handed over to the constructor of the target
	 * \tparam TTarget The target type to check
	 */
	template <class TSource, class TTarget>
	concept constructs = std::constructible_from<TTarget, TSource>;

	/** @} */
	/** @} */
}

#endif
