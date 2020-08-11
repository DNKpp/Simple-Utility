
//          Copyright Dominic Koepke 2019 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_CONCEPTS_ARITHMETIC_HPP
#define SL_CONCEPTS_ARITHMETIC_HPP

#pragma once

#include <concepts>

namespace sl::concepts
{
	template <class T1, class T2>
	concept AddAssignable = requires(T1 lhs, T2 rhs)
	{
		{ lhs += rhs };
	};

	template <class T1, class T2>
	concept SubtractAssignable = requires(T1 lhs, T2 rhs)
	{
		{ lhs -= rhs };
	};

	template <class T1, class T2>
	concept MultiplyAssignable = requires(T1 lhs, T2 rhs)
	{
		{ lhs *= rhs };
	};

	template <class T1, class T2>
	concept DivideAssignable = requires(T1 lhs, T2 rhs)
	{
		{ lhs /= rhs };
	};

	template <class T1, class T2>
	concept ModuloAssignable = requires(T1 lhs, T2 rhs)
	{
		{ lhs %= rhs };
	};

	template <class T1, class T2>
	concept Addable = AddAssignable<T1, T2> && requires(T1 lhs, T2 rhs)
	{
		{ lhs + rhs }->std::convertible_to<T1>;
	};

	template <class T1, class T2>
	concept Subtractable = SubtractAssignable<T1, T2> && requires(T1 lhs, T2 rhs)
	{
		{ lhs - rhs } -> std::convertible_to<T1>;
	};

	template <class T1, class T2>
	concept Multiplicable = MultiplyAssignable<T1, T2> && requires(T1 lhs, T2 rhs)
	{
		{ lhs * rhs } -> std::convertible_to<T1>;
	};

	template <class T1, class T2>
	concept Divisable = DivideAssignable<T1, T2> && requires(T1 lhs, T2 rhs)
	{
		{ lhs / rhs } -> std::convertible_to<T1>;
	};

	template <class T1, class T2>
	concept Moduloable = ModuloAssignable<T1, T2> && requires(T1 lhs, T2 rhs)
	{
		{ lhs % rhs }->std::convertible_to<T1>;
	};
	
}

#endif
