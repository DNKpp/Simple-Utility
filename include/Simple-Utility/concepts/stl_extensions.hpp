//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_CONCEPTS_STL_EXTENSIONSL_HPP
#define SL_UTILITY_CONCEPTS_STL_EXTENSIONSL_HPP

#pragma once

#include <concepts>

// ReSharper disable CppClangTidyClangDiagnosticDocumentation

namespace sl::concepts
{
	/**
	* \defgroup GROUP_STL_EXTENSION_CONCEPTS stl extensions
	*
	* \brief Contains several concept extensions for existing stl concepts.
	* \ingroup GROUP_CONCEPTS
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
	concept initializes = std::constructible_from<TTarget, TSource>;

	/**
	 * \brief Checks whether the target type is assignable from the source type.
	 * \details This is the symmetrical counterpart of ``std::assignable_from`` concept.
	 * \tparam TSource The source type handed over to the target
	 * \tparam TTarget The target type to check
	 */
	template <class TSource, class TTarget>
	concept assignable_to = std::assignable_from<TTarget, TSource>;

	/**
	 * \brief Checks whether a symmetrical set of operators == and != to compare both types with each other exists. 
	 * \details This is a less restrictive version of the ``std::equality_comparable_with`` function.
	 * \see https://en.cppreference.com/w/cpp/concepts/equality_comparable
	 * \tparam T1 The first type to check
	 * \tparam T2 The second type to check
	 */
	template <class T1, class T2>
	concept weakly_equality_comparable_with = requires(const std::remove_cvref_t<T1>& t1, const std::remove_cvref_t<T2>& t2)
	{
		{ t1 == t2 } -> std::convertible_to<bool>;
		{ t1 != t2 } -> std::convertible_to<bool>;
		{ t2 == t1 } -> std::convertible_to<bool>;
		{ t2 != t1 } -> std::convertible_to<bool>;
	};

	/** @} */
}

#endif
