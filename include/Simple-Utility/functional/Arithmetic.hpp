//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_ARITHMETIC_HPP
#define SL_UTILITY_FUNCTIONAL_ARITHMETIC_HPP

#include "Simple-Utility/concepts/operators.hpp"
#include "Simple-Utility/functional/transform.hpp"

namespace sl::functional::arithmetic
{
	/**
	 * \defgroup GROUP_FUNCTIONAL_ARITHMETIC arithmetic
	 * \brief Contains functional objects, implementing several arithmetically operations.
	 * \ingroup GROUP_FUNCTIONAL
	 * @{
	 */

	/**
	 * \brief Functional object which forwards the params to binary operator + and returns the result.
	 */
	inline constexpr auto plus = envelop<Transform>(
		[]<class TLhs, class TRhs>(TLhs&& lhs, TRhs&& rhs) -> decltype(auto)
		{
			static_assert(concepts::plus_with<TLhs, TRhs>, "Arguments are not usable as operands of binary operator +.");

			return std::forward<TLhs>(lhs) + std::forward<TRhs>(rhs);
		});

	/**
	 * \brief Functional object which forwards the params to binary operator - and returns the result.
	 */
	inline constexpr auto minus = envelop<Transform>(
		[]<class TLhs, class TRhs>(TLhs&& lhs, TRhs&& rhs) -> decltype(auto)
		{
			static_assert(concepts::minus_with<TLhs, TRhs>, "Arguments are not usable as operands of binary operator -.");

			return std::forward<TLhs>(lhs) - std::forward<TRhs>(rhs);
		});

	/**
	 * \brief Functional object which forwards the params to binary operator * and returns the result.
	 */
	inline constexpr auto multiplies = envelop<Transform>(
		[]<class TLhs, class TRhs>(TLhs&& lhs, TRhs&& rhs) -> decltype(auto)
		{
			static_assert(concepts::multiplies_with<TLhs, TRhs>, "Arguments are not usable as operands of binary operator *.");

			return std::forward<TLhs>(lhs) * std::forward<TRhs>(rhs);
		});

	/**
	 * \brief Functional object which forwards the params to binary operator / and returns the result.
	 */
	inline constexpr auto divides = envelop<Transform>(
		[]<class TLhs, class TRhs>(TLhs&& lhs, TRhs&& rhs) -> decltype(auto)
		{
			static_assert(concepts::divides_with<TLhs, TRhs>, "Arguments are not usable as operands of binary operator /.");

			return std::forward<TLhs>(lhs) / std::forward<TRhs>(rhs);
		});

	/**
	 * \brief Functional object which forwards the params to binary operator % and returns the result.
	 */
	inline constexpr auto modulus = envelop<Transform>(
		[]<class TLhs, class TRhs>(TLhs&& lhs, TRhs&& rhs) -> decltype(auto)
		{
			static_assert(concepts::modulus_with<TLhs, TRhs>, "Arguments are not usable as operands of binary operator %.");

			return std::forward<TLhs>(lhs) % std::forward<TRhs>(rhs);
		});

	/**
	 * \brief Functional object which forwards the param to unary operator - and returns the result.
	 */
	inline constexpr auto negate = envelop<Transform>(
		[]<class T>(T&& value) -> decltype(auto)
		{
			static_assert(concepts::negate<T>, "Argument is not usable as operand of unary operator -.");

			return -std::forward<T>(value);
		});;

	/** @} */
}

#endif
