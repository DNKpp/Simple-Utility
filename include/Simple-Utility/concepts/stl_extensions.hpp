//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_CONCEPTS_STL_EXTENSIONS_HPP
#define SL_UTILITY_CONCEPTS_STL_EXTENSIONS_HPP

#pragma once

#include "Simple-Utility/Config.hpp"

#include <compare>
#include <concepts>

// ReSharper disable CppClangTidyClangDiagnosticDocumentation
// ReSharper disable CppIdenticalOperandsInBinaryExpression
// ReSharper disable CppClangTidyMiscRedundantExpression

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
	 * \brief Checks whether T is not ``void``.
	 * \details This is the inverted counterpart of ``std::is_void_v`` trait.
	 * \see https://en.cppreference.com/w/cpp/types/is_void
	 * \tparam T Type to check.
	 */
	template <class T>
	concept not_void = !std::is_void_v<T>;

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
	 * \brief Checks whether the given type is comparable via operator == and !=.
	 * \details This is a less restrictive version of the ``std::equality_comparable`` function.
	 * \see https://en.cppreference.com/w/cpp/concepts/equality_comparable
	 * \tparam T The type to check.
	 */
	template <class T>
	concept weakly_equality_comparable = requires(const std::remove_cvref_t<T>& t)
	{
		{ t == t } -> std::convertible_to<bool>;
		{ t != t } -> std::convertible_to<bool>;
	};

	/**
	 * \brief Checks whether the given type is comparable via operator == and != and has noexcept specifier.
	 * \details This is a less restrictive version of the ``std::equality_comparable`` function, but with additional noexcept check.
	 * \see https://en.cppreference.com/w/cpp/concepts/equality_comparable
	 * \tparam T The type to check.
	 */
	template <class T>
	concept nothrow_weakly_equality_comparable = weakly_equality_comparable<T>
												&& requires(const std::remove_cvref_t<T>& t)
												{
													{ t == t } noexcept -> std::convertible_to<bool>;
													{ t != t } noexcept -> std::convertible_to<bool>;
												};

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
	 * \brief Checks whether the given type denotes a common stl comparison category.
	 * \tparam T Type to check.
	 * \see https://en.cppreference.com/w/cpp/utility/compare/common_comparison_category
	 */
	template <class T>
	concept comparison_category = !std::is_void_v<std::common_comparison_category_t<T>>;

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

	/**
	* \}
	*/
}

namespace sl::concepts::detail
{
	template <class Category, class MinimumCategory>
	concept compares_as = comparison_category<Category>
						&& comparison_category<MinimumCategory>
						&& std::same_as<std::common_comparison_category_t<Category, MinimumCategory>, MinimumCategory>;
}

namespace sl::concepts
{
	/**
	* \addtogroup GROUP_STL_EXTENSION_CONCEPTS
	* \{
	*/

	/**
	 * \brief Checks whether the given type is comparable <=> with the minimal comparison category. This also implies equality comparable.
	 * \details This is a less restrictive version of the ``std::three_way_comparable`` function.
	 * \see https://en.cppreference.com/w/cpp/utility/compare/three_way_comparable
	 * \tparam T The type to check.
	 * \tparam MinimumCategory The minimum category the comparison has to yield.
	 */
	template <class T, class MinimumCategory = std::partial_ordering>
	concept weakly_three_way_comparable = weakly_equality_comparable<T>
										&& comparison_category<MinimumCategory>
										&& requires(const std::remove_reference_t<T>& t)
										{
											{ t <=> t } -> detail::compares_as<MinimumCategory>;
										};

	/**
	 * \brief Checks whether the given type is comparable <=> with the minimal comparison category and noexcept specifier.
	 * This also implies equality comparable.
	 * \details This is a less restrictive version of the ``std::three_way_comparable`` function, but with additional noexcept check.
	 * \see https://en.cppreference.com/w/cpp/utility/compare/three_way_comparable
	 * \tparam T The type to check.
	 * \tparam MinimumCategory The minimum category the comparison has to yield.
	 */
	template <class T, class MinimumCategory = std::partial_ordering>
	concept nothrow_weakly_three_way_comparable = nothrow_weakly_equality_comparable<T>
												&& comparison_category<MinimumCategory>
												&& requires(const std::remove_reference_t<T>& t)
												{
													{ t <=> t } noexcept -> detail::compares_as<MinimumCategory>;
												};

	/**
	 * \brief Checks whether a symmetrical set of operator <=> to compare both types with each other exists.
	 * \details This is a less restrictive version of the ``std::three_way_comparable_with`` function.
	 * \see https://en.cppreference.com/w/cpp/utility/compare/three_way_comparable
	 * \tparam T1 The first type to check.
	 * \tparam T2 The second type to check.
	 * \tparam MinimumCategory The minimum category the comparison has to yield.
	 */
	template <class T1, class T2, class MinimumCategory = std::partial_ordering>
	concept weakly_three_way_comparable_with =
		weakly_equality_comparable_with<T1, T2>
		&& comparison_category<MinimumCategory>
		&& requires(const std::remove_reference_t<T1>& t1, const std::remove_reference_t<T2>& t2)
		{
			{ t1 <=> t2 } -> detail::compares_as<MinimumCategory>;
			{ t2 <=> t1 } -> detail::compares_as<MinimumCategory>;
		};

	/**
	 * \brief Checks whether a symmetrical set of operator <=> to compare both types with each other with noexcept specifier exists.
	 * \details This is a less restrictive version of the ``std::three_way_comparable_with`` function, but with additional noexcept check.
	 * \see https://en.cppreference.com/w/cpp/utility/compare/three_way_comparable
	 * \tparam T1 The first type to check.
	 * \tparam T2 The second type to check.
	 * \tparam MinimumCategory The minimum category the comparison has to yield.
	 */
	template <class T1, class T2, class MinimumCategory = std::partial_ordering>
	concept nothrow_weakly_three_way_comparable_with =
		nothrow_weakly_equality_comparable_with<T1, T2>
		&& comparison_category<MinimumCategory>
		&& requires(const std::remove_reference_t<T1>& t1, const std::remove_reference_t<T2>& t2)
		{
			{ t1 <=> t2 } noexcept -> detail::compares_as<MinimumCategory>;
			{ t2 <=> t1 } noexcept -> detail::compares_as<MinimumCategory>;
		};

	/**
	* \}
	*/
}

#ifdef SL_UTLITY_HAS_STD_FORMAT

#include <format>

namespace sl::concepts
{
	/**
	* \addtogroup GROUP_STL_EXTENSION_CONCEPTS
	* \{
	*/

	/**
	 * \brief Determines, whether a complete specialization of ``std::formatter`` for the given (possibly cv-ref qualified) type exists.
	 * \tparam T Type to check.
	 * \tparam Char Used character type.
	 * \details This is an adapted implementation of the ``std::formattable`` concept, which is added c++23.
	 * \note This implementation takes a simple but reasonable shortcut in assuming, that ```Char`` is either ``char`` or ``wchar_t``,
	 * which must not necessarily true.
	 * \see Adapted from here: https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2286r8.html#concept-formattable
	 * \see https://en.cppreference.com/w/cpp/utility/format/formattable
	 */
	template <class T, class Char>
	concept formattable =
		std::semiregular<std::formatter<std::remove_cvref_t<T>, Char>>
		&& requires(
		std::formatter<std::remove_cvref_t<T>, Char> formatter,
		T t,
		std::conditional_t<std::same_as<Char, char>, std::format_context, std::wformat_context> formatContext,
		std::basic_format_parse_context<Char> parseContext
	)
		{
			{ formatter.parse(parseContext) } -> std::same_as<typename std::basic_format_parse_context<Char>::iterator>;
			{
				std::as_const(formatter).format(t, formatContext)
			} -> std::same_as<typename std::remove_reference_t<decltype(formatContext)>::iterator>;
		};

	/**
	* \}
	*/
}

#endif

#endif
