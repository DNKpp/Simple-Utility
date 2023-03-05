//          Copyright Dominic Koepke 2019 - 2023.
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
	 * \tparam T Used floating point type.
	 */
	template <std::floating_point T>
	struct remquo_result
	{
		T remainder;
		int quo;
	};

	/**
	 * \brief Manual deduction guide for the ``remquo_result`` type.
	 * \tparam T Used floating point type.
	 *
	 * \note Several clang versions require this to be present.
	 */
	template <class T>
	remquo_result(T, int) -> remquo_result<T>;

	/**
	 * \brief Computes the floating-point remainder of the division operation x/y. Additionally, the sign and at least the three of the
	 * last bits of x/y will be determined, sufficient to determine the octant of the result within a period.
	 * \param x First floating point value.
	 * \param y Second floating point value.
	 * \attention y must not be equal to 0.
	 * \return Returns the result as ``remquo_result`` type.
	 *
	 * \details This function is a more convenient version of the ``std::remquo`` function and simply forwards the call to that.
	 */
	constexpr auto remquo = [](const auto x, const auto y) SL_CONSTEXPR_MATH
		requires requires { std::remquo(x, x, nullptr); }
	{
		int quo{};
		const auto remainder{ std::remquo(x, y, &quo) };
		return remquo_result{ remainder, quo };
	};

	/**
	 * \brief Result type for the frexp operation.
	 * \tparam T Used floating point type
	 */
	template <std::floating_point T>
	struct frexp_result
	{
		T fraction;
		int exp;
	};

	/**
	 * \brief Manual deduction guide for the ``frexp_result`` type.
	 * \tparam T Used floating point type.
	 *
	 * \note Several clang versions require this to be present.
	 */
	template <class T>
	frexp_result(T, int) -> frexp_result<T>;

	/**
	 * \brief Decomposes given floating point value into a normalized fraction and an integral power of two.
	 * \tparam T  Used floating point type
	 * \param value The value to decompose.
	 * \return Returns the result as ``frexp_result`` type.
	 *
	 * \details This function is a more convenient version of the ``std::frexp`` function and simply forwards the call to that.
	 */
	constexpr auto frexp = [](const auto value) SL_CONSTEXPR_MATH
		requires requires { std::frexp(value, nullptr); }
	{
		int exp{};
		const auto fraction{ std::frexp(value, &exp) };
		return frexp_result{ fraction, exp };
	};

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
	 * \details This function is a more convenient version of the ``std::modf`` function and simply forwards the call to that.
	 */
	constexpr auto modf = []<std::floating_point T>(const T value) SL_CONSTEXPR_MATH
	{
		T integral{};
		T fraction{ std::modf(value, &integral) };
		return modf_result<T>{ integral, fraction };
	};

	/** \} */
}

#endif
