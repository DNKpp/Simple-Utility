
//          Copyright Dominic Koepke 2019 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_OPERATORS_ARITHMETIC_HPP
#define SL_OPERATORS_ARITHMETIC_HPP

#pragma once

#include "Simple-Utility/concepts/arithmetic.hpp"

namespace sl::operators
{
	struct Plus
	{
		template <class T1, class T2>
		requires concepts::AddAssignable<T1, T2>
		friend constexpr T1 operator +(T1 lhs, const T2& rhs) noexcept(noexcept(lhs += rhs))
		{
			lhs += rhs;
			return lhs;
		}
	};

	struct Minus
	{
		template <class T1, class T2>
		requires concepts::SubtractAssignable<T1, T2>
		friend constexpr T1 operator -(T1 lhs, const T2& rhs) noexcept(noexcept(lhs -= rhs))
		{
			lhs -= rhs;
			return lhs;
		}
	};

	struct Multiply
	{
		template <class T1, class T2>
		requires concepts::MultiplyAssignable<T1, T2>
		friend constexpr T1 operator *(T1 lhs, const T2& rhs) noexcept(noexcept(lhs *= rhs))
		{
			lhs *= rhs;
			return lhs;
		}
	};

	struct Divide
	{
		template <class T1, class T2>
		requires concepts::DivideAssignable<T1, T2>
		friend constexpr T1 operator /(T1 lhs, const T2& rhs) noexcept(noexcept(lhs /= rhs))
		{
			lhs /= rhs;
			return lhs;
		}
	};

	struct Modulo
	{
		template <class T1, class T2>
		requires concepts::ModuloAssignable<T1, T2>
		friend constexpr T1 operator %(T1 lhs, const T2& rhs) noexcept(noexcept(lhs %= rhs))
		{
			lhs %= rhs;
			return lhs;
		}
	};

	struct Arithmetic :
		Plus,
		Minus,
		Multiply,
		Divide,
		Modulo
	{
	};
}

#endif
