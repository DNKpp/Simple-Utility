
//          Copyright Dominic Koepke 2019 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_OPERATORS_BITWISE_HPP
#define SL_OPERATORS_BITWISE_HPP

#pragma once

#include "Simple-Utility/concepts/bitwise.hpp"

namespace sl::operators
{
	struct BitwiseAnd
	{
		template <class T1, class T2>
		requires concepts::BitwiseAndAssignable<T1, T2>
		friend constexpr T1 operator &(T1 lhs, const T2& rhs) noexcept(noexcept(lhs &= rhs))
		{
			lhs &= rhs;
			return lhs;
		}
	};

	struct BitwiseOr
	{
		template <class T1, class T2>
		requires concepts::BitwiseOrAssignable<T1, T2>
		friend constexpr T1 operator |(T1 lhs, const T2& rhs) noexcept(noexcept(lhs |= rhs))
		{
			lhs |= rhs;
			return lhs;
		}
	};

	struct BitwiseXor
	{
		template <class T1, class T2>
		requires concepts::BitwiseXorAssignable<T1, T2>
		friend constexpr T1 operator ^(T1 lhs, const T2& rhs) noexcept(noexcept(lhs ^= rhs))
		{
			lhs ^= rhs;
			return lhs;
		}
	};

	struct BitwiseShiftLeft
	{
		template <class T1, class T2>
		requires concepts::BitwiseLeftShiftable<T1, T2>
		friend constexpr T1 operator <<(T1 lhs, const T2& rhs) noexcept(noexcept(lhs <<= rhs))
		{
			lhs <<= rhs;
			return lhs;
		}
	};

	struct BitwiseShiftRight
	{
		template <class T1, class T2>
		requires concepts::BitwiseRightShiftable<T1, T2>
		friend constexpr T1 operator >>(T1 lhs, const T2& rhs) noexcept(noexcept(lhs >>= rhs))
		{
			lhs >>= rhs;
			return lhs;
		}
	};

	struct BitwiseLogical :
		BitwiseAnd,
		BitwiseOr,
		BitwiseXor
	{
	};

	struct BitwiseShift :
		BitwiseShiftRight,
		BitwiseShiftLeft
	{
	};
}

#endif
