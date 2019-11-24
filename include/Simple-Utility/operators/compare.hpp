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

#ifndef SL_OPERATORS_COMPARE_HPP
#define SL_OPERATORS_COMPARE_HPP

#pragma once

#include <type_traits>

namespace SL {
namespace operators {

	/*!
	* \struct	Equal
	*
	* \brief	Single template helper struct for compare equal. Provides the inheritor with an implementation of operator !=(const T&, const T&) as friend function.
	* 			The inheritors must implement the operator ==(const T&, const T&) themselves.
	*
	* \tparam	T	Generic type parameter.
	*/
	template <class T>
	struct Equal
	{
		/*!
		 * \brief	Inequality operator
		 *
		 * \param	_lhs	The first instance to compare.
		 * \param	_rhs	The second instance to compare.
		 *
		 * \return	True if the parameters are not considered equivalent.
		 */
		friend constexpr bool operator !=(const T& _lhs, const T& _rhs)
		{
			return !(_lhs == _rhs);
		}
	};

	template <class T>
	struct Order
	{
		friend constexpr bool operator >=(const T& _lhs, const T& _rhs)
		{
			return !(_lhs < _rhs);
		}
		
		friend constexpr bool operator >(const T& _lhs, const T& _rhs)
		{
			return _rhs < _lhs;
		}
		
		friend constexpr bool operator <=(const T& _lhs, const T& _rhs)
		{
			return !(_lhs > _rhs);
		}
	};
	
	template <class T>
	struct TotalCompare :
		Equal<T>,
		Order<T>
	{
	};
	
	/*#####
	# compare 2
	#####*/
	template <class T1, class T2>
	struct Equal2
	{
		friend constexpr bool operator ==(const T2& _lhs, const T1& _rhs)
		{
			return _rhs == _lhs;
		}

		friend constexpr bool operator !=(const T1& _lhs, const T2& _rhs)
		{
			return !(_lhs == _rhs);
		}

		friend constexpr bool operator !=(const T2& _lhs, const T1& _rhs)
		{
			return !(_lhs == _rhs);
		}
	};
	
	template <class T1, class T2>
	struct Order2
	{
		friend constexpr bool operator >=(const T1& _lhs, const T2& _rhs)
		{
			return !(_lhs < _rhs);
		}
		
		friend constexpr bool operator >(const T1& _lhs, const T2& _rhs)
		{
			return _rhs < _lhs;
		}
		
		friend constexpr bool operator <=(const T1& _lhs, const T2& _rhs)
		{
			return !(_lhs > _rhs);
		}
		
		// ---------------------------------------------------
		friend constexpr bool operator >=(const T2& _lhs, const T1& _rhs)
		{
			return !(_lhs < _rhs);
		}

		friend constexpr bool operator >(const T2& _lhs, const T1& _rhs)
		{
			return _rhs < _lhs;
		}

		friend constexpr bool operator <=(const T2& _lhs, const T1& _rhs)
		{
			return !(_lhs > _rhs);
		}
	};
	
	template <class T1, class T2>
	struct TotalCompare2 :
		Equal2<T1, T2>,
		Order2<T1, T2>
	{
	};
}
}

#endif // !SL_OPERATORS_COMPARE_HPP
