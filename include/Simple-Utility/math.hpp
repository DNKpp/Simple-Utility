//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_MATH_HPP
#define SL_UTILITY_MATH_HPP

#pragma once

#include <cmath>
#include <concepts>

#if __cpp_lib_constexpr_cmath >= 202202L
#define SL_CONSTEXPR_MATH constexpr
#else
#define SL_CONSTEXPR_MATH
#endif

namespace sl::math
{
	/*! \defgroup GROUP_MATH math
	* \{
	*/

	/**
	 * \brief Result type for the remquo operation.
	 * \tparam T Used floating point type
	 */
	template <std::floating_point T>
	struct remquo_result
	{
		T remainder;
		int quo;
	};

	/**
	 * \brief Helper alias, deducing the result type of a ``std::remquo`` call.
	 */
	template <class T>
	using remquo_result_value_type = decltype(std::remquo(std::declval<T>(), std::declval<T>(), nullptr));

	/**
	 * \brief Computes the floating-point remainder of the division operation x/y. Additionally, the sign and at least the three of the
	 * last bits of x/y will be determined, sufficient to determine the octant of the result within a period.
	 * \tparam T  Used floating point type
	 * \param x First floating point value.
	 * \param y Second floating point value.
	 * \attention y must not be equal to 0.
	 * \return Returns the result as ``remquo_result`` type.
	 *
	 * \detail This function is a more convenient version of the ``std::remquo`` function and simply forwards the call to that.
	 */
	template <class T>
		requires requires { std::remquo(std::declval<T>(), std::declval<T>(), nullptr); }
	[[nodiscard]]
	SL_CONSTEXPR_MATH remquo_result<remquo_result_value_type<T>> remquo(T x, T y)
	{
		int quo{};
		const auto remainder{ std::remquo(x, y, &quo) };
		return { remainder, quo };
	}

	/**
	 * \brief Result type for the modf operation.
	 * \tparam T Used floating point type
	 */
	template <std::floating_point T>
	struct modf_result
	{
		T integral;
		T fraction;
	};

	/**
	 * \brief Decomposes given floating point value into integral and fractional parts, each having the same type and sign as the original value.
	 * \tparam T  Used floating point type
	 * \param value The value to decompose.
	 * \return Returns the result as ``modf_result`` type.
	 *
	 * \detail This function is a more convenient version of the ``std::modf`` function and simply forwards the call to that.
	 */
	template <std::floating_point T>
	[[nodiscard]]
	SL_CONSTEXPR_MATH modf_result<T> modf(T value)
	{
		T integral{};
		T fraction{ std::modf(value, &integral) };
		return { integral, fraction };
	}

	/** \} */
}

#endif
