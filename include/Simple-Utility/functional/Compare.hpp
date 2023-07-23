//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_COMPARE_HPP
#define SL_UTILITY_FUNCTIONAL_COMPARE_HPP

#pragma once

#include "Simple-Utility/concepts/stl_extensions.hpp"
#include "Simple-Utility/functional/Predicate.hpp"
#include "Simple-Utility/functional/Transform.hpp"

namespace sl::functional::compare
{
	/**
	 * \defgroup GROUP_FUNCTIONAL_COMPARE compare
	 * \ingroup GROUP_FUNCTIONAL
	 * @{
	 */

	/**
	 * \brief Functional object, which compares its two operands less.
	 * \note If both operands satisfy the ``std::integral`` concept, ``std::cmp_less`` is used instead.
	 */
	inline constexpr auto less = envelop<Predicate>(
		[]<class Lhs, class Rhs>(Lhs&& lhs, Rhs&& rhs) noexcept(noexcept(std::declval<Lhs>() < std::declval<Rhs>()))
		{
			if constexpr (std::integral<std::remove_cvref_t<Lhs>> && std::integral<std::remove_cvref_t<Rhs>>)
			{
				return std::cmp_less(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
			}
			else
			{
				static_assert(
					requires { { lhs < rhs } -> std::convertible_to<bool>; },
					"Operands are not usable in operator < expressions.");

				return std::forward<Lhs>(lhs) < std::forward<Rhs>(rhs);
			}
		});

	/**
	 * \brief Functional object, which compares its two operands less-equal.
	 * \note If both operands satisfy the ``std::integral`` concept, ``std::cmp_less_equal`` is used instead.
	 */
	inline constexpr auto less_equal = envelop<Predicate>(
		[]<class Lhs, class Rhs>(Lhs&& lhs, Rhs&& rhs) noexcept(noexcept(std::declval<Lhs>() <= std::declval<Rhs>()))
		{
			if constexpr (std::integral<std::remove_cvref_t<Lhs>> && std::integral<std::remove_cvref_t<Rhs>>)
			{
				return std::cmp_less_equal(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
			}
			else
			{
				static_assert(
					requires { { lhs <= rhs } -> std::convertible_to<bool>; },
					"Operands are not usable in operator <= expressions.");

				return std::forward<Lhs>(lhs) <= std::forward<Rhs>(rhs);
			}
		});

	/**
	 * \brief Functional object, which compares its two operands greater.
	 * \note If both operands satisfy the ``std::integral`` concept, ``std::cmp_greater`` is used instead.
	 */
	inline constexpr auto greater = envelop<Predicate>(
		[]<class Lhs, class Rhs>(Lhs&& lhs, Rhs&& rhs) noexcept(noexcept(std::declval<Lhs>() > std::declval<Rhs>()))
		{
			if constexpr (std::integral<std::remove_cvref_t<Lhs>> && std::integral<std::remove_cvref_t<Rhs>>)
			{
				return std::cmp_greater(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
			}
			else
			{
				static_assert(
					requires { { lhs > rhs } -> std::convertible_to<bool>; },
					"Operands are not usable in operator > expressions.");

				return std::forward<Lhs>(lhs) > std::forward<Rhs>(rhs);
			}
		});

	/**
	 * \brief Functional object, which compares its two operands greater-equal.
	 * \note If both operands satisfy the ``std::integral`` concept, ``std::cmp_greater_equal`` is used instead.
	 */
	inline constexpr auto greater_equal = envelop<Predicate>(
		[]<class Lhs, class Rhs>(Lhs&& lhs, Rhs&& rhs) noexcept(noexcept(std::declval<Lhs>() >= std::declval<Rhs>()))
		{
			if constexpr (std::integral<std::remove_cvref_t<Lhs>> && std::integral<std::remove_cvref_t<Rhs>>)
			{
				return std::cmp_greater_equal(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
			}
			else
			{
				static_assert(
					requires { { lhs >= rhs } -> std::convertible_to<bool>; },
					"Operands are not usable in operator >= expressions.");

				return std::forward<Lhs>(lhs) >= std::forward<Rhs>(rhs);
			}
		});

	/**
	 * \brief Functional object, which compares its two operands equal.
	 * \note If both operands satisfy the ``std::integral`` concept, ``std::cmp_equal`` is used instead.
	 */
	inline constexpr auto equal = envelop<Predicate>(
		[]<class Lhs, class Rhs>(Lhs&& lhs, Rhs&& rhs) noexcept(noexcept(std::declval<Lhs>() == std::declval<Rhs>()))
		{
			if constexpr (std::integral<std::remove_cvref_t<Lhs>> && std::integral<std::remove_cvref_t<Rhs>>)
			{
				return std::cmp_equal(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
			}
			else
			{
				static_assert(
					requires { { lhs == rhs } -> std::convertible_to<bool>; },
					"Operands are not usable in operator == expressions.");

				return std::forward<Lhs>(lhs) == std::forward<Rhs>(rhs);
			}
		});

	/**
	 * \brief Functional object, which compares its two operands not-equal.
	 * \note If both operands satisfy the ``std::integral`` concept, ``std::cmp_not_equal`` is used instead.
	 */
	inline constexpr auto not_equal = envelop<Predicate>(
		[]<class Lhs, class Rhs>(Lhs&& lhs, Rhs&& rhs) noexcept(noexcept(std::declval<Lhs>() != std::declval<Rhs>()))
		{
			if constexpr (std::integral<std::remove_cvref_t<Lhs>> && std::integral<std::remove_cvref_t<Rhs>>)
			{
				return std::cmp_not_equal(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
			}
			else
			{
				static_assert(
					requires { { lhs != rhs } -> std::convertible_to<bool>; },
					"Operands are not usable in operator != expressions.");

				return std::forward<Lhs>(lhs) != std::forward<Rhs>(rhs);
			}
		});

	/**
	 * \brief Functional object, which performs a three-way comparison on its two operands..
	 */
	inline constexpr auto three_way = envelop<Transform>(
		[]<class Lhs, class Rhs>(Lhs&& lhs, Rhs&& rhs) noexcept(concepts::nothrow_weakly_three_way_comparable_with<Lhs, Rhs>)
		{
			static_assert(concepts::weakly_three_way_comparable_with<Lhs, Rhs>,
				"Operands are not usable in operator <=> expressions.");

			return std::forward<Lhs>(lhs) <=> std::forward<Rhs>(rhs);
		});

	/** @} */
}

#endif
