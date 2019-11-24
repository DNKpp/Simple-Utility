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

#ifndef SL_OPERATORS_ARITHMETIC_HPP
#define SL_OPERATORS_ARITHMETIC_HPP

#pragma once

namespace sl {
namespace operators {

	/*!
	 * \struct	Plus
	 *
	 * \brief	Single template helper struct for additions. Provides the inheritor with an implementation of operator +(const T&, const T&) as friend function.
	 * 			The inheritors must implement the operator +=(const T&, const T&) themselves.
	 *
	 * \tparam	T	Generic type parameter.
	 */
	template <class T>
	struct Plus
	{
		/*!
		 * \brief	Addition operator.
		 *
		 * \param	_lhs	The first value.
		 * \param	_rhs	A value to add to it.
		 *
		 * \return	The result of the operation.
		 */
		friend constexpr T operator +(const T& _lhs, const T& _rhs)
		{
			T tmp(_lhs);
			tmp += _rhs;
			return tmp;
		}
	};

	/*!
	* \struct	Minus
	*
	* \brief	Single template helper struct for subtractions. Provides the inheritor with an implementation of operator -(const T&, const T&) as friend function.
	* 			The inheritors must implement the operator -=(const T&, const T&) themselves.
	*
	* \tparam	T	Generic type parameter.
	*/
	template <class T>
	struct Minus
	{
		/*!
		 * \brief	Subtraction operator.
		 *
		 * \param	_lhs	The first value.
		 * \param	_rhs	A value to subtract from it.
		 *
		 * \return	The result of the operation.
		 */
		friend constexpr T operator -(const T& _lhs, const T& _rhs)
		{
			T tmp(_lhs);
			tmp -= _rhs;
			return tmp;
		}
	};

	/*!
	* \struct	Multiply
	*
	* \brief	Single template helper struct for multiplications. Provides the inheritor with an implementation of operator *(const T&, const T&) as friend function.
	* 			The inheritors must implement the operator *=(const T&, const T&) themselves.
	*
	* \tparam	T	Generic type parameter.
	*/
	template <class T>
	struct Multiply
	{
		/*!
		 * \brief	Multiplication operator.
		 *
		 * \param	_lhs	The first value to multiply.
		 * \param	_rhs	The second value to multiply.
		 *
		 * \return	The result of the operation.
		 */
		friend constexpr T operator *(const T& _lhs, const T& _rhs)
		{
			T tmp(_lhs);
			tmp *= _rhs;
			return tmp;
		}
	};

	/*!
	* \struct	Divide
	*
	* \brief	Single template helper struct for divisions. Provides the inheritor with an implementation of operator /(const T&, const T&) as friend function.
	* 			The inheritors must implement the operator /=(const T&, const T&) themselves.
	*
	* \tparam	T	Generic type parameter.
	*/
	template <class T>
	struct Divide
	{
		/*!
		 * \brief	Division operator.
		 *
		 * \param	_lhs	The numerator.
		 * \param	_rhs	The denominator.
		 *
		 * \return	The result of the operation.
		 */
		friend constexpr T operator /(const T& _lhs, const T& _rhs)
		{
			T tmp(_lhs);
			tmp /= _rhs;
			return tmp;
		}
	};

	/*!
	* \struct	Modulo
	*
	* \brief	Single template helper struct for modulo. Provides the inheritor with an implementation of operator %(const T&, const T&) as friend function.
	* 			The inheritors must implement the operator %=(const T&, const T&) themselves.
	*
	* \tparam	T	Generic type parameter.
	*/
	template <class T>
	struct Modulo
	{
		/*!
		 * \brief	Modulus operator.
		 *
		 * \param	_lhs	The numerator.
		 * \param	_rhs	The denominator.
		 *
		 * \return	The result of the operation.
		 */
		friend constexpr T operator %(const T& _lhs, const T& _rhs)
		{
			T tmp(_lhs);
			tmp %= _rhs;
			return tmp;
		}
	};

	/*!
	* \struct	Arithmetic
	*
	* \brief	Single template helper struct for all arithmetic operations. Look at the inherited classes for more details.
	*
	* \tparam	T	Generic type parameter.
	*/
	template <class T>
	struct Arithmetic :
		Plus<T>,
		Minus<T>,
		Multiply<T>,
		Divide<T>,
		Modulo<T>
	{
	};

	/*!
	* \struct	Plus2
	*
	* \brief	Multi template helper struct for additions. Provides the inheritor with an implementation of operator +(const T1&, const T2&) as friend function.
	* 			The inheritors must implement the operator +=(const T1&, const T2&) themselves.
	* \remark	Because lack of consistency, the reversed operator +=(const T2&, const T1&) isn't implemented by this struct. If you need it, you either have to provide it by yourself
	* 			or inherit Plus2<T2, T1>.
	*
	* \tparam	T1	Generic type parameter.
	* \tparam	T2	Generic type parameter.
	*/
	template <class T1, class T2>
	struct Plus2
	{
		/*!
		* \brief	Addition operator.
		*
		* \param	_lhs	The first value.
		* \param	_rhs	A value to add to it.
		*
		* \return	The result of the operation.
		*/
		friend constexpr T1 operator +(const T1& _lhs, const T2& _rhs)
		{
			T1 tmp(_lhs);
			tmp += _rhs;
			return tmp;
		}
	};

	/*!
	* \struct	Minus2
	*
	* \brief	Multi template helper struct for subtractions. Provides the inheritor with an implementation of operator -(const T1&, const T2&) as friend function.
	* 			The inheritors must implement the operator -=(const T1&, const T2&) themselves.
	* \remark	Because lack of consistency, the reversed operator -=(const T2&, const T1&) isn't implemented by this struct. If you need it, you either have to provide it by yourself
	* 			or inherit Minus2<T2, T1>.
	*
	* \tparam	T1	Generic type parameter.
	* \tparam	T2	Generic type parameter.
	*/
	template <class T1, class T2>
	struct Minus2
	{
		/*!
		* \brief	Subtraction operator.
		*
		* \param	_lhs	The first value.
		* \param	_rhs	A value to subtract from it.
		*
		* \return	The result of the operation.
		*/
		friend constexpr T1 operator -(const T1& _lhs, const T2& _rhs)
		{
			T1 tmp(_lhs);
			tmp -= _rhs;
			return tmp;
		}
	};

	/*!
	* \struct	Multiply2
	*
	* \brief	Multi template helper struct for multiplications. Provides the inheritor with an implementation of operator *(const T1&, const T2&) as friend function.
	* 			The inheritors must implement the operator *=(const T1&, const T2&) themselves.
	* \remark	Because lack of consistency, the reversed operator *=(const T2&, const T1&) isn't implemented by this struct. If you need it, you either have to provide it by yourself
	* 			or inherit Multiply2<T2, T1>.
	*
	* \tparam	T1	Generic type parameter.
	* \tparam	T2	Generic type parameter.
	*/
	template <class T1, class T2>
	struct Multiply2
	{
		/*!
		* \brief	Multiplication operator.
		*
		* \param	_lhs	The first value to multiply.
		* \param	_rhs	The second value to multiply.
		*
		* \return	The result of the operation.
		*/
		friend constexpr T1 operator *(const T1& _lhs, const T2& _rhs)
		{
			T1 tmp(_lhs);
			tmp *= _rhs;
			return tmp;
		}
	};

	/*!
	* \struct	Divide2
	*
	* \brief	Multi template helper struct for divisions. Provides the inheritor with an implementation of operator /(const T1&, const T2&) as friend function.
	* 			The inheritors must implement the operator /=(const T1&, const T2&) themselves.
	* \remark	Because lack of consistency, the reversed operator /=(const T2&, const T1&) isn't implemented by this struct. If you need it, you either have to provide it by yourself
	* 			or inherit Divide2<T2, T1>.
	*
	* \tparam	T1	Generic type parameter.
	* \tparam	T2	Generic type parameter.
	*/
	template <class T1, class T2>
	struct Divide2
	{
		/*!
		* \brief	Division operator.
		*
		* \param	_lhs	The numerator.
		* \param	_rhs	The denominator.
		*
		* \return	The result of the operation.
		*/
		friend constexpr T1 operator /(const T1& _lhs, const T2& _rhs)
		{
			T1 tmp(_lhs);
			tmp /= _rhs;
			return tmp;
		}
	};

	/*!
	* \struct	Modulo2
	*
	* \brief	Multi template helper struct for modulo. Provides the inheritor with an implementation of operator %(const T1&, const T2&) as friend function.
	* 			The inheritors must implement the operator %=(const T1&, const T2&) themselves.
	* \remark	Because lack of consistency, the reversed operator %=(const T2&, const T1&) isn't implemented by this struct. If you need it, you either have to provide it by yourself
	* 			or inherit Modulo2<T2, T1>.
	*
	* \tparam	T1	Generic type parameter.
	* \tparam	T2	Generic type parameter.
	*/
	template <class T1, class T2>
	struct Modulo2
	{
		/*!
		* \brief	Modulus operator.
		*
		* \param	_lhs	The numerator.
		* \param	_rhs	The denominator.
		*
		* \return	The result of the operation.
		*/
		friend constexpr T1 operator %(const T1& _lhs, const T2& _rhs)
		{
			T1 tmp(_lhs);
			tmp %= _rhs;
			return tmp;
		}
	};

	/*!
	* \struct	Arithmetic2
	*
	* \brief	Multi template helper struct for all arithmetic operations. Look at the inherited classes for more details.
	*
	* \tparam	T1	Generic type parameter.
	* \tparam	T2	Generic type parameter.
	*/
	template <class T1, class T2>
	struct Arithmetic2 :
		Plus2<T1, T2>,
		Minus2<T1, T2>,
		Multiply2<T1, T2>,
		Divide2<T1, T2>,
		Modulo2<T1, T2>
	{
	};
}
}

#endif // !SL_OPERATORS_ARITHMETIC_HPP
