
//          Copyright Dominic Koepke 2019 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_CONCEPTS_BITWISE_HPP
#define SL_CONCEPTS_BITWISE_HPP

#pragma once

#include <concepts>

namespace sl::concepts
{
	template <class T1, class T2>
	concept BitwiseRightShiftable = requires (T1 lhs, T2 rhs)
	{
		{ lhs >>= rhs } -> std::convertible_to<T1>;
	};

	template <class T1, class T2>
	concept BitwiseLeftShiftable = requires (T1 lhs, T2 rhs)
	{
		{ lhs <<= rhs } -> std::convertible_to<T1>;
	};

	template <class T1, class T2>
	concept BitwiseShiftable = BitwiseLeftShiftable<T1, T2> && BitwiseRightShiftable<T1, T2>;

	template <class T1, class T2>
	concept BitwiseAndAssignable = requires (T1 lhs, T2 rhs)
	{
		{ lhs &= rhs };
	};

	template <class T1, class T2>
	concept BitwiseOrAssignable = requires (T1 lhs, T2 rhs)
	{
		{ lhs |= rhs };
	};

	template <class T1, class T2>
	concept BitwiseXorAssignable = requires (T1 lhs, T2 rhs)
	{
		{ lhs ^= rhs };
	};

	template <class T1, class T2>
	concept BitwiseLogicallyAssignable = BitwiseAndAssignable<T1, T2> && BitwiseOrAssignable<T1, T2> && BitwiseXorAssignable<T1, T2>;
	
}

#endif
