//          Copyright Dominic Koepke 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch.hpp>

#include "Simple-Utility/concepts/operators.hpp"

using namespace sl::concepts;

namespace
{
	enum shift
	{
		none = 0,
		left = 1 << 1,
		right = 1 << 2,
		bidirectional = left | right,

		left_assign = 1 << 3,
		right_assign = 1 << 4,
		bidirectional_assign = left_assign | right_assign,

		fully = bidirectional | bidirectional_assign
	};

	template <int VShifts = none>
	struct shift_target
	{
		shift_target operator <<(int) const
			requires ((VShifts & left) != 0)
		{
			return shift_target{};
		}

		shift_target operator >>(int) const
			requires ((VShifts & right) != 0)
		{
			return shift_target{};
		}

		shift_target& operator <<=(int)
			requires ((VShifts & left_assign) != 0)
		{
			return *this;
		}

		shift_target& operator >>=(int)
			requires ((VShifts & right_assign) != 0)
		{
			return *this;
		}
	};

	struct no_shift
	{
	};
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"right_shiftable_with should determine if two types can be used within an operator >> expression.",
	"[concepts][operators]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(int, int, true),
	(float, int, false),
	(int, float, false),
	(shift_target<right>, int, true),
	(const shift_target<right>, int, true),
	(shift_target<right>, no_shift, false),
	(shift_target<>, int, false)
)
#pragma warning(default: 26444)
{
	REQUIRE(right_shiftable_with<TLhs, TRhs> == VExpected);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"right_shiftable should determine if a type can be used within an operator >> expression.",
	"[concepts][operators]",
	((class T, bool VExpected), T, VExpected),
	(int, true),
	(float, false),
	(shift_target<right>, false),
	(const shift_target<right>, false),
	(shift_target<>, false)
)
#pragma warning(default: 26444)
{
	REQUIRE(right_shiftable<T> == VExpected);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"left_shiftable_with should determine if two types can be used within an operator << expression.",
	"[concepts][operators]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(int, int, true),
	(float, int, false),
	(int, float, false),
	(shift_target<left>, int, true),
	(const shift_target<left>, int, true),
	(shift_target<left>, no_shift, false),
	(shift_target<>, int, false)
)
#pragma warning(default: 26444)
{
	REQUIRE(left_shiftable_with<TLhs, TRhs> == VExpected);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"left_shiftable should determine if a type can be used within an operator << expression.",
	"[concepts][operators]",
	((class T, bool VExpected), T, VExpected),
	(int, true),
	(float, false),
	(shift_target<left>, false),
	(const shift_target<left>, false),
	(shift_target<>, false)
)
#pragma warning(default: 26444)
{
	REQUIRE(left_shiftable<T> == VExpected);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"bidirectional_shiftable_with should determine if two types can be used within operator << and operator >> expressions.",
	"[concepts][operators]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(int, int, true),
	(float, int, false),
	(int, float, false),
	(shift_target<bidirectional>, int, true),
	(const shift_target<bidirectional>, int, true),
	(shift_target<left>, int, false),
	(shift_target<right>, int, false)
)
#pragma warning(default: 26444)
{
	REQUIRE(bidirectional_shiftable_with<TLhs, TRhs> == VExpected);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"bidirectional_shiftable should determine if a type can be used within operator << and operator >> expressions.",
	"[concepts][operators]",
	((class T, bool VExpected), T, VExpected),
	(int, true),
	(float, false),
	(shift_target<bidirectional>, false),
	(const shift_target<bidirectional>, false),
	(shift_target<left>, false),
	(shift_target<right>, false)
)
#pragma warning(default: 26444)
{
	REQUIRE(bidirectional_shiftable<T> == VExpected);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"right_shift_assignable_with should determine if two types can be used within an operator >>= expression.",
	"[concepts][operators]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(int, int, true),
	(float, int, false),
	(int, float, false),
	(shift_target<right_assign>, int, true),
	(const shift_target<right_assign>, int, false),
	(shift_target<right_assign>, no_shift, false),
	(shift_target<>, int, false)
)
#pragma warning(default: 26444)
{
	REQUIRE(right_shift_assignable_with<TLhs, TRhs> == VExpected);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"right_shift_assignable should determine if a type can be used within an operator >>= expression.",
	"[concepts][operators]",
	((class T, bool VExpected), T, VExpected),
	(int, true),
	(float, false),
	(shift_target<right_assign>, false),
	(const shift_target<right_assign>, false),
	(shift_target<>, false)
)
#pragma warning(default: 26444)
{
	REQUIRE(right_shift_assignable<T> == VExpected);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"left_shift_assignable_with should determine if two types can be used within an operator <<= expression.",
	"[concepts][operators]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(int, int, true),
	(float, int, false),
	(int, float, false),
	(shift_target<left_assign>, int, true),
	(const shift_target<left_assign>, int, false),
	(shift_target<left_assign>, no_shift, false),
	(shift_target<>, int, false)
)
#pragma warning(default: 26444)
{
	REQUIRE(left_shift_assignable_with<TLhs, TRhs> == VExpected);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"left_shift_assignable should determine if a type can be used within an operator <<= expression.",
	"[concepts][operators]",
	((class T, bool VExpected), T, VExpected),
	(int, true),
	(float, false),
	(shift_target<right_assign>, false),
	(const shift_target<right_assign>, false),
	(shift_target<>, false)
)
#pragma warning(default: 26444)
{
	REQUIRE(left_shift_assignable<T> == VExpected);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"bidirectional_shift_assignable_with should determine if two types can be used within operator <<= and operator >>= expressions.",
	"[concepts][operators]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(int, int, true),
	(float, int, false),
	(int, float, false),
	(shift_target<bidirectional_assign>, int, true),
	(const shift_target<bidirectional_assign>, int, false),
	(shift_target<left_assign>, int, false),
	(shift_target<right_assign>, int, false)
)
#pragma warning(default: 26444)
{
	REQUIRE(bidirectional_shift_assignable_with<TLhs, TRhs> == VExpected);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"bidirectional_shift_assignable should determine if a type can be used within operator <<= and operator =>> expressions.",
	"[concepts][operators]",
	((class T, bool VExpected), T, VExpected),
	(int, true),
	(float, false),
	(shift_target<bidirectional_assign>, false),
	(const shift_target<bidirectional_assign>, false),
	(shift_target<left_assign>, false),
	(shift_target<right_assign>, false)
)
#pragma warning(default: 26444)
{
	REQUIRE(bidirectional_shift_assignable<T> == VExpected);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"fully_shiftable_with should determine if two types can be used within all shift operator expressions.",
	"[concepts][operators]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(int, int, true),
	(float, int, false),
	(int, float, false),
	(shift_target<fully>, int, true),
	(const shift_target<fully>, int, false),
	(shift_target<fully ^ right>, int, false),
	(shift_target<fully ^ left>, int, false),
	(shift_target<fully ^ right_assign>, int, false),
	(shift_target<fully ^ left_assign>, int, false)
)
#pragma warning(default: 26444)
{
	REQUIRE(fully_shiftable_with<TLhs, TRhs> == VExpected);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"fully_shiftable should determine if a type can be used within all shift operator expressions.",
	"[concepts][operators]",
	((class T, bool VExpected), T, VExpected),
	(int, true),
	(float, false),
	(shift_target<fully>, false),
	(const shift_target<fully>, false),
	(shift_target<fully ^ right>, false),
	(shift_target<fully ^ left>, false),
	(shift_target<fully ^ right_assign>, false),
	(shift_target<fully ^ left_assign>, false)
)
#pragma warning(default: 26444)
{
	REQUIRE(fully_shiftable<T> == VExpected);
}
