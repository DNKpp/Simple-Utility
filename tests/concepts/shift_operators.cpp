//          Copyright Dominic Koepke 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch.hpp>

#include "../helper.hpp"

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
		shift_target() = default;

		explicit shift_target(int)
		{
		}

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

	// right shiftable
	TESTABLE_BINARY_CONCEPT(right_shiftable_with);

	TESTABLE_BINARY_CONCEPT_R(right_shiftable_with_r);

	TESTABLE_UNARY_CONCEPT(right_shiftable);

	TESTABLE_UNARY_CONCEPT_R(right_shiftable_r);

	TESTABLE_BINARY_CONCEPT(right_shift_assignable_with);

	TESTABLE_BINARY_CONCEPT_R(right_shift_assignable_with_r);

	TESTABLE_UNARY_CONCEPT(right_shift_assignable);

	TESTABLE_UNARY_CONCEPT_R(right_shift_assignable_r);

	// left shiftable
	TESTABLE_BINARY_CONCEPT(left_shiftable_with);

	TESTABLE_BINARY_CONCEPT_R(left_shiftable_with_r);

	TESTABLE_UNARY_CONCEPT(left_shiftable);

	TESTABLE_UNARY_CONCEPT_R(left_shiftable_r);

	TESTABLE_BINARY_CONCEPT(left_shift_assignable_with);

	TESTABLE_BINARY_CONCEPT_R(left_shift_assignable_with_r);

	TESTABLE_UNARY_CONCEPT(left_shift_assignable);

	TESTABLE_UNARY_CONCEPT_R(left_shift_assignable_r);

	// bidirectional shiftable
	TESTABLE_BINARY_CONCEPT(bidirectional_shiftable_with);

	TESTABLE_BINARY_CONCEPT_R(bidirectional_shiftable_with_r);

	TESTABLE_UNARY_CONCEPT(bidirectional_shiftable);

	TESTABLE_UNARY_CONCEPT_R(bidirectional_shiftable_r);

	TESTABLE_BINARY_CONCEPT(bidirectional_shift_assignable_with);

	TESTABLE_BINARY_CONCEPT_R(bidirectional_shift_assignable_with_r);

	TESTABLE_UNARY_CONCEPT(bidirectional_shift_assignable);

	TESTABLE_UNARY_CONCEPT_R(bidirectional_shift_assignable_r);

	// fully shiftabele
	TESTABLE_BINARY_CONCEPT(fully_shiftable_with);

	TESTABLE_BINARY_CONCEPT_R(fully_shiftable_with_r);

	TESTABLE_UNARY_CONCEPT(fully_shiftable);

	TESTABLE_UNARY_CONCEPT_R(fully_shiftable_r);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"right_shiftable_with(_r) should determine if two types can be used within an operator >> expression.",
	"[concepts][operators][shift]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(testable_right_shiftable_with, testable_right_shiftable_with_r),
	(
		(int, int, true),
		(float, int, false),
		(int, float, false),
		(shift_target<right>, int, true),
		(const shift_target<right>, int, true),
		(shift_target<right>, empty_t, false),
		(shift_target<>, int, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"right_shiftable(_r) should determine if a type can be used within an operator >> expression.",
	"[concepts][operators][shift]",
	((class T, bool VExpected), T, VExpected),
	(testable_right_shiftable, testable_right_shiftable_r),
	(
		(int, true),
		(float, false),
		(shift_target<right>, false),
		(const shift_target<right>, false),
		(shift_target<>, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"left_shiftable_with(_r) should determine if two types can be used within an operator << expression.",
	"[concepts][operators][shift]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(testable_left_shiftable_with, testable_left_shiftable_with_r),
	(
		(int, int, true),
		(float, int, false),
		(int, float, false),
		(shift_target<left>, int, true),
		(const shift_target<left>, int, true),
		(shift_target<left>, empty_t, false),
		(shift_target<>, int, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"left_shiftable(_r) should determine if a type can be used within an operator << expression.",
	"[concepts][operators][shift]",
	((class T, bool VExpected), T, VExpected),
	(testable_left_shiftable, testable_left_shiftable_r),
	(
		(int, true),
		(float, false),
		(shift_target<left>, false),
		(const shift_target<left>, false),
		(shift_target<>, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"bidirectional_shiftable_with(_r) should determine if two types can be used within operator << and operator >> expressions.",
	"[concepts][operators][shift]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(testable_bidirectional_shiftable_with, testable_bidirectional_shiftable_with_r),
	(
		(int, int, true),
		(float, int, false),
		(int, float, false),
		(shift_target<bidirectional>, int, true),
		(const shift_target<bidirectional>, int, true),
		(shift_target<left>, int, false),
		(shift_target<right>, int, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"bidirectional_shiftable(_r) should determine if a type can be used within operator << and operator >> expressions.",
	"[concepts][operators][shift]",
	((class T, bool VExpected), T, VExpected),
	(testable_bidirectional_shiftable, testable_bidirectional_shiftable_r),
	(
		(int, true),
		(float, false),
		(shift_target<bidirectional>, false),
		(const shift_target<bidirectional>, false),
		(shift_target<left>, false),
		(shift_target<right>, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"right_shift_assignable_with(_r) should determine if two types can be used within an operator >>= expression.",
	"[concepts][operators][shift]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(testable_right_shift_assignable_with, testable_right_shift_assignable_with_r),
	(
		(int, int, true),
		(float, int, false),
		(int, float, false),
		(shift_target<right_assign>, int, true),
		(const shift_target<right_assign>, int, false),
		(shift_target<right_assign>, empty_t, false),
		(shift_target<>, int, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"right_shift_assignable(_r) should determine if a type can be used within an operator >>= expression.",
	"[concepts][operators][shift]",
	((class T, bool VExpected), T, VExpected),
	(testable_right_shift_assignable, testable_right_shift_assignable_r),
	(
		(int, true),
		(float, false),
		(shift_target<right_assign>, false),
		(const shift_target<right_assign>, false),
		(shift_target<>, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"left_shift_assignable_with(_r) should determine if two types can be used within an operator <<= expression.",
	"[concepts][operators][shift]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(testable_left_shift_assignable_with, testable_left_shift_assignable_with_r),
	(
		(int, int, true),
		(float, int, false),
		(int, float, false),
		(shift_target<left_assign>, int, true),
		(const shift_target<left_assign>, int, false),
		(shift_target<left_assign>, empty_t, false),
		(shift_target<>, int, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"left_shift_assignable(_r) should determine if a type can be used within an operator <<= expression.",
	"[concepts][operators][shift]",
	((class T, bool VExpected), T, VExpected),
	(testable_left_shift_assignable, testable_left_shift_assignable_r),
	(
		(int, true),
		(float, false),
		(shift_target<right_assign>, false),
		(const shift_target<right_assign>, false),
		(shift_target<>, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"bidirectional_shift_assignable_with(_r) should determine if two types can be used within operator <<= and operator >>= expressions.",
	"[concepts][operators][shift]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(testable_bidirectional_shift_assignable_with, testable_bidirectional_shift_assignable_with_r),
	(
		(int, int, true),
		(float, int, false),
		(int, float, false),
		(shift_target<bidirectional_assign>, int, true),
		(const shift_target<bidirectional_assign>, int, false),
		(shift_target<left_assign>, int, false),
		(shift_target<right_assign>, int, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"bidirectional_shift_assignable(_r) should determine if a type can be used within operator <<= and operator =>> expressions.",
	"[concepts][operators][shift]",
	((class T, bool VExpected), T, VExpected),
	(testable_bidirectional_shift_assignable, testable_bidirectional_shift_assignable_r),
	(
		(int, true),
		(float, false),
		(shift_target<bidirectional_assign>, false),
		(const shift_target<bidirectional_assign>, false),
		(shift_target<left_assign>, false),
		(shift_target<right_assign>, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"fully_shiftable_with(_r) should determine if two types can be used within all shift operator expressions.",
	"[concepts][operators][shift]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(testable_fully_shiftable_with, testable_fully_shiftable_with_r),
	(
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
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"fully_shiftable(_r) should determine if a type can be used within all shift operator expressions.",
	"[concepts][operators][shift]",
	((class T, bool VExpected), T, VExpected),
	(testable_fully_shiftable, testable_fully_shiftable_r),
	(
		(int, true),
		(float, false),
		(shift_target<fully>, false),
		(const shift_target<fully>, false),
		(shift_target<fully ^ right>, false),
		(shift_target<fully ^ left>, false),
		(shift_target<fully ^ right_assign>, false),
		(shift_target<fully ^ left_assign>, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"(right|left|bidirectional)_shiftable_with_r should determine if return type satisfies expectations.",
	"[concepts][operators][shift]",
	((class TLhs, class TRhs, bool VExpected, class TReturn), TLhs, TRhs, VExpected, TReturn),
	(testable_right_shiftable_with_r, testable_left_shiftable_with_r, testable_bidirectional_shiftable_with_r),
	(
		(int, int, true, int),
		(int, int, true, const int),
		(int, int, true, int&&),
		(int, int, true, const int&),
		(int, int, false, int&),
		(int, int, false, shift_target<>), // explicit ctor
		(shift_target<fully>, int, true, shift_target<fully>),
		(shift_target<fully>, int, true, const shift_target<fully>),
		(shift_target<fully>, int, true, shift_target<fully>&&),
		(shift_target<fully>, int, true, const shift_target<fully>&),
		(shift_target<fully>, int, false, int),
		(shift_target<fully>, int, false, shift_target<fully>&)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"(right|left|bidirectional)_shift_r should determine if return type satisfies expectations.",
	"[concepts][operators][shift]",
	((class T, bool VExpected, class TReturn), T, VExpected, TReturn),
	(testable_right_shiftable_r, testable_left_shiftable_r, testable_bidirectional_shiftable_r),
	(
		(int, true, int),
		(int, true, const int),
		(int, true, int&&),
		(int, true, const int&),
		(int, false, int&),
		(int, false, shift_target<>) // explicit ctor
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"(right|left|bidirectional)_shift_assignable_with_r should determine if return type satisfies expectations.",
	"[concepts][operators][shift]",
	((class TLhs, class TRhs, bool VExpected, class TReturn), TLhs, TRhs, VExpected, TReturn),
	(testable_right_shift_assignable_with_r, testable_left_shift_assignable_with_r, testable_bidirectional_shift_assignable_with_r),
	(
		(int, int, true, int),
		(int, int, true, const int),
		(int, int, true, int&),
		(int, int, true, const int&),
		(int, int, false, int&&),
		(int, int, false, empty_t),
		(int, int, false, shift_target<>), // explicit ctor
		(shift_target<fully>, int, true, shift_target<fully>),
		(shift_target<fully>, int, true, const shift_target<fully>),
		(shift_target<fully>, int, false, int),
		(shift_target<fully>, int, true, shift_target<fully>&),
		(shift_target<fully>, int, true, const shift_target<fully>&),
		(shift_target<fully>, int, false, shift_target<fully>&&),
		(shift_target<fully>, int, false, empty_t)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"(right|left|bidirectional)_shift_assignable_r should determine if return type satisfies expectations.",
	"[concepts][operators][shift]",
	((class T, bool VExpected, class TReturn), T, VExpected, TReturn),
	(testable_right_shift_assignable_r, testable_left_shift_assignable_r, testable_bidirectional_shift_assignable_r),
	(
		(int, true, int),
		(int, true, const int),
		(int, true, int&),
		(int, true, const int&),
		(int, false, int&&),
		(int, false, shift_target<>), // explicit ctor
		(int, false, empty_t)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}
