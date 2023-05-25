//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_CONCEPTS_STL_EXTENSIONS_HPP
#define SL_UTILITY_CONCEPTS_STL_EXTENSIONS_HPP

#pragma once

#include <concepts>

// ReSharper disable CppClangTidyClangDiagnosticDocumentation

namespace sl::concepts
{
	/**
	* \defgroup GROUP_STL_EXTENSION_CONCEPTS stl extensions
	*
	* \brief Contains several concept extensions for existing stl concepts and traits.
	* \ingroup GROUP_CONCEPTS
	* @{
	*/

	/**
	 * \brief Checks whether the ``T`` denotes a (possibly cv-qualified) pointer type.
	 * \details This is the concept version of ``std::is_pointer``.
	 * \see https://en.cppreference.com/w/cpp/types/is_pointer
	 * \tparam T The type to check.
	 */
	template <class T>
	concept pointer = std::is_pointer_v<T>;

	/**
	 * \brief Checks whether the ``T`` denotes a reference type.
	 * \details This is the concept version of ``std::is_reference``.
	 * \see https://en.cppreference.com/w/cpp/types/is_reference
	 * \tparam T The type to check.
	 */
	template <class T>
	concept reference = std::is_reference_v<T>;

	/**
	 * \brief Checks whether the ``T`` denotes a decayed type.
	 * \see https://en.cppreference.com/w/cpp/types/decay
	 * \tparam T The type to check.
	 */
	template <class T>
	concept decayed = std::same_as<std::decay_t<T>, T>;

	/**
	 * \brief Checks whether the ``T`` denotes an unqualified type.
	 * \see https://en.cppreference.com/w/cpp/types/remove_cvref
	 * \tparam T The type to check.
	 */
	template <class T>
	concept unqualified = std::same_as<std::remove_cvref_t<T>, T>;

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

	/**
	 * \brief Checks whether a symmetrical set of operators == and != to compare both types with each other exists and has noexcept specifier. 
	 * \details This is a less restrictive version of the ``std::equality_comparable_with`` function, but with additional noexcept check.
	 * \see https://en.cppreference.com/w/cpp/concepts/equality_comparable
	 * \tparam T1 The first type to check
	 * \tparam T2 The second type to check
	 */
	template <class T1, class T2>
	concept nothrow_weakly_equality_comparable_with = requires(const std::remove_cvref_t<T1>& t1, const std::remove_cvref_t<T2>& t2)
	{
		{ t1 == t2 } noexcept -> std::convertible_to<bool>;
		{ t1 != t2 } noexcept -> std::convertible_to<bool>;
		{ t2 == t1 } noexcept -> std::convertible_to<bool>;
		{ t2 != t1 } noexcept -> std::convertible_to<bool>;
	};

	/**
	 * \brief Checks whether a type is explicit convertible to another. 
	 * \details This is a less restrictive version of the ``std::convertible_to`` concept.
	 * \see https://en.cppreference.com/w/cpp/concepts/convertible_to
	 * \tparam TFrom The source type.
	 * \tparam TTo The target type.
	 */
	template <class TFrom, class TTo>
	concept explicitly_convertible_to = requires
	{
		static_cast<TTo>(std::declval<TFrom>());
	};

	/**
	 * \brief Checks whether a type is explicit convertible to another and does not throw. 
	 * \details This is a less restrictive version of the ``std::convertible_to`` concept, but with additional noexcept check.
	 * \see https://en.cppreference.com/w/cpp/concepts/convertible_to
	 * \tparam TFrom The source type.
	 * \tparam TTo The target type.
	 */
	template <class TFrom, class TTo>
	concept nothrow_explicitly_convertible_to = requires
	{
		{ static_cast<TTo>(std::declval<TFrom>()) } noexcept;
	};

	/** @} */
}

#endif
