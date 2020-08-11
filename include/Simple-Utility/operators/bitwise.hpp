
//          Copyright Dominic Koepke 2019 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_OPERATORS_BITWISE_HPP
#define SL_OPERATORS_BITWISE_HPP

#pragma once

#include <compare>

#include "Simple-Utility/concepts/bitwise.hpp"

namespace sl::operators
{
	struct BitwiseAnd
	{
		constexpr auto operator <=>(const BitwiseAnd&) const noexcept = default;
		
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
		constexpr auto operator <=>(const BitwiseOr&) const noexcept = default;
		
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
		constexpr auto operator <=>(const BitwiseXor&) const noexcept = default;
		
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
		constexpr auto operator <=>(const BitwiseShiftLeft&) const noexcept = default;
		
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
		constexpr auto operator <=>(const BitwiseShiftRight&) const noexcept = default;
		
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
		constexpr auto operator <=>(const BitwiseLogical&) const noexcept = default;
	};

	struct BitwiseShift :
		BitwiseShiftRight,
		BitwiseShiftLeft
	{
		constexpr auto operator <=>(const BitwiseShift&) const noexcept = default;
	};
}

#endif
