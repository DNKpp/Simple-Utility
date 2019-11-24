//The MIT License(MIT)
//
//Copyright(c) 2017 Dominic Koepke
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files(the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions :
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.

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
