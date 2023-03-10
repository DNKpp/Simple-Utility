//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_COMPARE_HPP
#define SL_UTILITY_FUNCTIONAL_COMPARE_HPP

#pragma once

#include "Simple-Utility/functional/predicate.hpp"
#include "Simple-Utility/functional/transform.hpp"

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
	inline constexpr predicate_fn less{
		[]<class TLhs, class TRhs>(TLhs&& lhs, TRhs&& rhs) noexcept(noexcept(std::declval<TLhs>() < std::declval<TRhs>()))
		{
			if constexpr (std::integral<std::remove_cvref_t<TLhs>> && std::integral<std::remove_cvref_t<TRhs>>)
			{
				return std::cmp_less(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs));
			}
			else
			{
				static_assert(requires { { lhs < rhs } -> std::convertible_to<bool>; },
					"Operands are not usable in operator < expressions.");

				return std::forward<TLhs>(lhs) < std::forward<TRhs>(rhs);
			}
		}
	};

	/**
	 * \brief Functional object, which compares its two operands less-equal.
	 * \note If both operands satisfy the ``std::integral`` concept, ``std::cmp_less_equal`` is used instead.
	 */
	inline constexpr predicate_fn less_equal{
		[]<class TLhs, class TRhs>(TLhs&& lhs, TRhs&& rhs) noexcept(noexcept(std::declval<TLhs>() <= std::declval<TRhs>()))
		{
			if constexpr (std::integral<std::remove_cvref_t<TLhs>> && std::integral<std::remove_cvref_t<TRhs>>)
			{
				return std::cmp_less_equal(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs));
			}
			else
			{
				static_assert(requires { { lhs <= rhs } -> std::convertible_to<bool>; },
					"Operands are not usable in operator <= expressions.");

				return std::forward<TLhs>(lhs) <= std::forward<TRhs>(rhs);
			}
		}
	};

	/**
	 * \brief Functional object, which compares its two operands greater.
	 * \note If both operands satisfy the ``std::integral`` concept, ``std::cmp_greater`` is used instead.
	 */
	inline constexpr predicate_fn greater{
		[]<class TLhs, class TRhs>(TLhs&& lhs, TRhs&& rhs) noexcept(noexcept(std::declval<TLhs>() > std::declval<TRhs>()))
		{
			if constexpr (std::integral<std::remove_cvref_t<TLhs>> && std::integral<std::remove_cvref_t<TRhs>>)
			{
				return std::cmp_greater(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs));
			}
			else
			{
				static_assert(requires { { lhs > rhs } -> std::convertible_to<bool>; },
					"Operands are not usable in operator > expressions.");

				return std::forward<TLhs>(lhs) > std::forward<TRhs>(rhs);
			}
		}
	};

	/**
	 * \brief Functional object, which compares its two operands greater-equal.
	 * \note If both operands satisfy the ``std::integral`` concept, ``std::cmp_greater_equal`` is used instead.
	 */
	inline constexpr predicate_fn greater_equal{
		[]<class TLhs, class TRhs>(TLhs&& lhs, TRhs&& rhs) noexcept(noexcept(std::declval<TLhs>() >= std::declval<TRhs>()))
		{
			if constexpr (std::integral<std::remove_cvref_t<TLhs>> && std::integral<std::remove_cvref_t<TRhs>>)
			{
				return std::cmp_greater_equal(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs));
			}
			else
			{
				static_assert(requires { { lhs >= rhs } -> std::convertible_to<bool>; },
					"Operands are not usable in operator >= expressions.");

				return std::forward<TLhs>(lhs) >= std::forward<TRhs>(rhs);
			}
		}
	};

	/**
	 * \brief Functional object, which compares its two operands equal.
	 * \note If both operands satisfy the ``std::integral`` concept, ``std::cmp_equal`` is used instead.
	 */
	inline constexpr predicate_fn equal{
		[]<class TLhs, class TRhs>(TLhs&& lhs, TRhs&& rhs) noexcept(noexcept(std::declval<TLhs>() == std::declval<TRhs>()))  // NOLINT(clang-diagnostic-float-equal)
		{
			if constexpr (std::integral<std::remove_cvref_t<TLhs>> && std::integral<std::remove_cvref_t<TRhs>>)
			{
				return std::cmp_equal(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs));
			}
			else
			{
				static_assert(requires { { lhs == rhs } -> std::convertible_to<bool>; },
					"Operands are not usable in operator == expressions.");

				return std::forward<TLhs>(lhs) == std::forward<TRhs>(rhs);
			}
		}
	};

	/**
	 * \brief Functional object, which compares its two operands not-equal.
	 * \note If both operands satisfy the ``std::integral`` concept, ``std::cmp_not_equal`` is used instead.
	 */
	inline constexpr predicate_fn not_equal{
		[]<class TLhs, class TRhs>(TLhs&& lhs, TRhs&& rhs) noexcept(noexcept(std::declval<TLhs>() != std::declval<TRhs>()))  // NOLINT(clang-diagnostic-float-equal)
		{
			if constexpr (std::integral<std::remove_cvref_t<TLhs>> && std::integral<std::remove_cvref_t<TRhs>>)
			{
				return std::cmp_not_equal(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs));
			}
			else
			{
				static_assert(requires { { lhs != rhs } -> std::convertible_to<bool>; },
					"Operands are not usable in operator != expressions.");

				return std::forward<TLhs>(lhs) != std::forward<TRhs>(rhs);
			}
		}
	};

	/**
	 * \brief Functional object, which performs a three-way comparison on its two operands..
	 */
	inline constexpr transform_fn three_way{
		[]<class TLhs, class TRhs>(TLhs&& lhs, TRhs&& rhs) noexcept(noexcept(std::declval<TLhs>() <=> std::declval<TRhs>()))
		{
			static_assert(std::three_way_comparable_with<TLhs, TRhs>, "Operands are not usable in operator <=> expressions.");

			return std::forward<TLhs>(lhs) <=> std::forward<TRhs>(rhs);
		}
	};

	/** @} */
}

#endif
