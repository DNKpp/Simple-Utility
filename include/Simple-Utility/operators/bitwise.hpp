
//          Copyright Dominic Koepke 2019 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_OPERATORS_BINARY_HPP
#define SL_OPERATORS_BINARY_HPP

#pragma once

namespace sl {
namespace operators {

	template <class T>
	struct BitwiseAND
	{
		friend constexpr T operator &(const T& _lhs, const T& _rhs)
		{
			T tmp(_lhs);
			tmp &= _rhs;
			return tmp;
		}
	};

	template <class T>
	struct BitwiseOR
	{
		friend constexpr T operator |(const T& _lhs, const T& _rhs)
		{
			T tmp(_lhs);
			tmp |= _rhs;
			return tmp;
		}
	};

	template <class T>
	struct BitwiseXOR
	{
		friend constexpr T operator ^(const T& _lhs, const T& _rhs)
		{
			T tmp(_lhs);
			tmp ^= _rhs;
			return tmp;
		}
	};

	template <class T>
	struct BitwiseShiftLeft
	{
		friend constexpr T operator <<(const T& _lhs, const T& _rhs)
		{
			T tmp(_lhs);
			tmp <<= _rhs;
			return tmp;
		}
	};

	template <class T>
	struct BitwiseShiftRight
	{
		friend constexpr T operator >>(const T& _lhs, const T& _rhs)
		{
			T tmp(_lhs);
			tmp >>= _rhs;
			return tmp;
		}
	};

	template <class T>
	struct BitwiseLogical : BitwiseAND<T>,
		BitwiseOR<T>,
		BitwiseXOR<T>
	{
	};

	template <class T>
	struct BitwiseShift : BitwiseShiftRight<T>,
		BitwiseShiftLeft<T>
	{
	};

	template <class T>
	struct Bitwise : BitwiseLogical<T>,
		BitwiseShift<T>
	{
	};

	template <class T1, class T2>
	struct BitwiseAND2
	{
		friend constexpr T1 operator &(const T1& _lhs, const T2& _rhs)
		{
			T1 tmp(_lhs);
			tmp &= _rhs;
			return tmp;
		}
	};

	template <class T1, class T2>
	struct BitwiseOR2
	{
		friend constexpr T1 operator |(const T1& _lhs, const T2& _rhs)
		{
			T1 tmp(_lhs);
			tmp |= _rhs;
			return tmp;
		}
	};

	template <class T1, class T2>
	struct BitwiseXOR2
	{
		friend constexpr T1 operator ^(const T1& _lhs, const T2& _rhs)
		{
			T1 tmp(_lhs);
			tmp ^= _rhs;
			return tmp;
		}
	};

	template <class T1, class T2>
	struct BitwiseShiftLeft2
	{
		friend constexpr T1 operator <<(const T1& _lhs, const T2& _rhs)
		{
			T1 tmp(_lhs);
			tmp <<= _rhs;
			return tmp;
		}
	};

	template <class T1, class T2>
	struct BitwiseShiftRight2
	{
		friend constexpr T1 operator >>(const T1& _lhs, const T2& _rhs)
		{
			T1 tmp(_lhs);
			tmp >>= _rhs;
			return tmp;
		}
	};

	template <class T1, class T2>
	struct BitwiseLogical2 : BitwiseAND2<T1, T2>,
		BitwiseOR2<T1, T2>,
		BitwiseXOR2<T1, T2>
	{
	};

	template <class T1, class T2>
	struct BitwiseShift2 : BitwiseShiftRight2<T1, T2>,
		BitwiseShiftLeft2<T1, T2>
	{
	};

	template <class T1, class T2>
	struct Bitwise2 : BitwiseLogical2<T1, T2>,
		BitwiseShift2<T1, T2>
	{
	};
}
}

#endif // !SL_OPERATORS_BINARY_HPP
