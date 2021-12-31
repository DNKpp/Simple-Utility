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

	// fully shiftable
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
	(is_right_shiftable_with, is_right_shiftable_with_r),
	(
		(int, int, true),
		(float, int, false),
		(int, float, false),
		(shift_target<right>, int, true),
		(const shift_target<right>, int, true),
		(shift_target<right>, fail_t, false),
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
	(is_right_shiftable, is_right_shiftable_r),
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
	(is_left_shiftable_with, is_left_shiftable_with_r),
	(
		(int, int, true),
		(float, int, false),
		(int, float, false),
		(shift_target<left>, int, true),
		(const shift_target<left>, int, true),
		(shift_target<left>, fail_t, false),
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
	(is_left_shiftable, is_left_shiftable_r),
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
	(is_bidirectional_shiftable_with, is_bidirectional_shiftable_with_r),
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
	(is_bidirectional_shiftable, is_bidirectional_shiftable_r),
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
	(is_right_shift_assignable_with, is_right_shift_assignable_with_r),
	(
		(int, int, true),
		(float, int, false),
		(int, float, false),
		(shift_target<right_assign>, int, true),
		(const shift_target<right_assign>, int, false),
		(shift_target<right_assign>, fail_t, false),
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
	(is_right_shift_assignable, is_right_shift_assignable_r),
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
	(is_left_shift_assignable_with, is_left_shift_assignable_with_r),
	(
		(int, int, true),
		(float, int, false),
		(int, float, false),
		(shift_target<left_assign>, int, true),
		(const shift_target<left_assign>, int, false),
		(shift_target<left_assign>, fail_t, false),
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
	(is_left_shift_assignable, is_left_shift_assignable_r),
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
	(is_bidirectional_shift_assignable_with, is_bidirectional_shift_assignable_with_r),
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
	(is_bidirectional_shift_assignable, is_bidirectional_shift_assignable_r),
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
	(is_fully_shiftable_with, is_fully_shiftable_with_r),
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
	(is_fully_shiftable, is_fully_shiftable_r),
	(
		(int, true),
		(float, false),
		(shift_target<fully>, false),	// not shift with itself
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
	((class TLhs, class TRhs, class TReturn, bool VExpected), TLhs, TRhs, TReturn, VExpected),
	(is_r_right_shiftable_with_r, is_r_left_shiftable_with_r, is_r_bidirectional_shiftable_with_r),
	(
		(int, int, int, true),
		(int, int, const int, true),
		(int, int, int&&, true),
		(int, int, const int&, true),
		(int, int, int&, false),
		(int, int, shift_target<>, false), // explicit ctor
		(shift_target<fully>, int, shift_target<fully>, true),
		(shift_target<fully>, int, const shift_target<fully>, true),
		(shift_target<fully>, int, shift_target<fully>&&, true),
		(shift_target<fully>, int, const shift_target<fully>&, true),
		(shift_target<fully>, int, int, false),
		(shift_target<fully>, int, shift_target<fully>&, false)
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
	((class T, class TReturn, bool VExpected), T, TReturn, VExpected),
	(is_r_right_shiftable_r, is_r_left_shiftable_r, is_r_bidirectional_shiftable_r),
	(
		(int, int, true),
		(int, const int, true),
		(int, int&&, true),
		(int, const int&, true),
		(int, int&, false),
		(int, shift_target<>, false) // explicit ctor
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
	((class TLhs, class TRhs, class TReturn, bool VExpected), TLhs, TRhs, TReturn, VExpected),
	(is_r_right_shift_assignable_with_r, is_r_left_shift_assignable_with_r, is_r_bidirectional_shift_assignable_with_r),
	(
		(int, int, int, true),
		(int, int, const int, true),
		(int, int, int&, true),
		(int, int, const int&, true),
		(int, int, int&&, false),
		(int, int, fail_t, false),
		(int, int, shift_target<>, false), // explicit ctor
		(shift_target<fully>, int, shift_target<fully>, true),
		(shift_target<fully>, int, const shift_target<fully>, true),
		(shift_target<fully>, int, int, false),
		(shift_target<fully>, int, shift_target<fully>&, true),
		(shift_target<fully>, int, const shift_target<fully>&, true),
		(shift_target<fully>, int, shift_target<fully>&&, false),
		(shift_target<fully>, int, fail_t, false)
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
	((class T, class TReturn, bool VExpected), T, TReturn, VExpected),
	(is_r_right_shift_assignable_r, is_r_left_shift_assignable_r, is_r_bidirectional_shift_assignable_r),
	(
		(int, int, true),
		(int, const int, true),
		(int, int&, true),
		(int, const int&, true),
		(int, int&&, false),
		(int, shift_target<>, false), // explicit ctor
		(int, fail_t, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}
