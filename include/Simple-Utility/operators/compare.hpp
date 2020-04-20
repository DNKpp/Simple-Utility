
//          Copyright Dominic Koepke 2019 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_OPERATORS_COMPARE_HPP
#define SL_OPERATORS_COMPARE_HPP

#pragma once

#include <type_traits>

namespace sl {
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
