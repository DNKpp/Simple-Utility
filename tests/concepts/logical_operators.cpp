//          Copyright Dominic Koepke 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch.hpp>

#include "Simple-Utility/concepts/operators.hpp"

#include "../helper.hpp"

using namespace sl::concepts;

namespace
{
	enum logical
	{
		none = 0,
		complement_op = 1 << 1,
		invert_op = 1 << 2,

		conjunctive_op = 1 << 3,
		disjunctive_op = 1 << 4,
		ex_disjunctive_op = 1 << 5,

		conjunctive_with_op = 1 << 6,
		disjunctive_with_op = 1 << 7,
		ex_disjunctive_with_op = 1 << 8,

		conjunctive_assign_op = 1 << 9,
		disjunctive_assign_op = 1 << 10,
		ex_disjunctive_assign_op = 1 << 11,

		conjunctive_assign_with_op = 1 << 12,
		disjunctive_assign_with_op = 1 << 13,
		ex_disjunctive_assign_with_op = 1 << 14,

		fully = complement_op | invert_op
				| conjunctive_op | disjunctive_op | ex_disjunctive_op
				| conjunctive_with_op | disjunctive_with_op | ex_disjunctive_with_op
				| conjunctive_assign_op | disjunctive_assign_op | ex_disjunctive_assign_op
				| conjunctive_assign_with_op | disjunctive_assign_with_op | ex_disjunctive_assign_with_op
	};

	template <int VLogical = none>
	struct logical_target
	{
		logical_target() = default;

		explicit logical_target(int)
		{
		}

		logical_target operator ~() const
			requires ((VLogical & complement_op) != 0)
		{
			return {};
		}

		logical_target operator !() const
			requires ((VLogical & invert_op) != 0)
		{
			return {};
		}

		logical_target operator &(const logical_target&) const
			requires ((VLogical & conjunctive_op) != 0)
		{
			return {};
		}

		logical_target operator |(const logical_target&) const
			requires ((VLogical & disjunctive_op) != 0)
		{
			return {};
		}

		logical_target operator ^(const logical_target&) const
			requires ((VLogical & ex_disjunctive_op) != 0)
		{
			return {};
		}

		logical_target operator &(int) const
			requires ((VLogical & conjunctive_with_op) != 0)
		{
			return {};
		}

		logical_target operator |(int) const
			requires ((VLogical & disjunctive_with_op) != 0)
		{
			return {};
		}

		logical_target operator ^(int) const
			requires ((VLogical & ex_disjunctive_with_op) != 0)
		{
			return {};
		}

		logical_target& operator &=(const logical_target&) const
			requires ((VLogical & conjunctive_assign_op) != 0)
		{
			return *this;
		}

		logical_target& operator |=(const logical_target&) const
			requires ((VLogical & disjunctive_assign_op) != 0)
		{
			return *this;
		}

		logical_target& operator ^=(const logical_target&) const
			requires ((VLogical & ex_disjunctive_assign_op) != 0)
		{
			return *this;
		}

		logical_target& operator &=(int) const
			requires ((VLogical & conjunctive_assign_with_op) != 0)
		{
			return *this;
		}

		logical_target& operator |=(int) const
			requires ((VLogical & disjunctive_assign_with_op) != 0)
		{
			return *this;
		}

		logical_target& operator ^=(int) const
			requires ((VLogical & ex_disjunctive_assign_with_op) != 0)
		{
			return *this;
		}
	};

	// complemented
	TESTABLE_UNARY_CONCEPT(complemented);

	TESTABLE_UNARY_CONCEPT_R(complemented_r);

	// invertible
	TESTABLE_UNARY_CONCEPT(invertible);

	TESTABLE_UNARY_CONCEPT_R(invertible_r);

	// conjunctive
	TESTABLE_UNARY_CONCEPT(conjunctive);

	TESTABLE_UNARY_CONCEPT_R(conjunctive_r);

	TESTABLE_UNARY_CONCEPT(conjunctive_assign);

	TESTABLE_UNARY_CONCEPT_R(conjunctive_assign_r);

	TESTABLE_BINARY_CONCEPT(conjunctive_with);

	TESTABLE_BINARY_CONCEPT_R(conjunctive_with_r);

	TESTABLE_BINARY_CONCEPT(conjunctive_assign_with);

	TESTABLE_BINARY_CONCEPT_R(conjunctive_assign_with_r);

	// disjunctive

	TESTABLE_UNARY_CONCEPT(disjunctive);

	TESTABLE_UNARY_CONCEPT_R(disjunctive_r);

	TESTABLE_BINARY_CONCEPT(disjunctive_with);

	TESTABLE_BINARY_CONCEPT_R(disjunctive_with_r);

	TESTABLE_UNARY_CONCEPT(disjunctive_assign);

	TESTABLE_UNARY_CONCEPT_R(disjunctive_assign_r);

	TESTABLE_BINARY_CONCEPT(disjunctive_assign_with);

	TESTABLE_BINARY_CONCEPT_R(disjunctive_assign_with_r);

	// exclusive disjunctive

	TESTABLE_UNARY_CONCEPT(exclusive_disjunctive);

	TESTABLE_UNARY_CONCEPT_R(exclusive_disjunctive_r);

	TESTABLE_UNARY_CONCEPT(exclusive_disjunctive_assign);

	TESTABLE_UNARY_CONCEPT_R(exclusive_disjunctive_assign_r);

	TESTABLE_BINARY_CONCEPT(exclusive_disjunctive_with);

	TESTABLE_BINARY_CONCEPT_R(exclusive_disjunctive_with_r);

	TESTABLE_BINARY_CONCEPT(exclusive_disjunctive_assign_with);

	TESTABLE_BINARY_CONCEPT_R(exclusive_disjunctive_assign_with_r);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"complemented(_r) should determine if a type can be used within an operator ~ expression.",
	"[concepts][operators][logically]",
	((class T, bool VExpected), T, VExpected),
	(testable_complemented, testable_complemented_r),
	(
		(int, true),
		(float, false),
		(logical_target<complement_op>, true),
		(logical_target<fully ^ complement_op>, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"invertible(_r) should determine if a type can be used within an operator ! expression.",
	"[concepts][operators][logically]",
	((class T, bool VExpected), T, VExpected),
	(testable_invertible, testable_invertible_r),
	(
		(int, true),
		(float, true),
		(fail_t, false),
		(logical_target<invert_op>, true),
		(logical_target<fully ^ invert_op>, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"unary logically _r concepts should determine if a return type of an expression can be converted to the expected one.",
	"[concepts][operators][logically]",
	((class T, bool VExpected, class TResult), T, VExpected, TResult),
	(testable_complemented_r, testable_invertible_r),
	(
		(int, true, int),
		(int, true, float),
		(int, false, int&),
		(int, false, fail_t),
		(logical_target<fully>, true, logical_target<fully>),
		(logical_target<fully>, false, int)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"conjunctive_with(_r) should determine if two types can be used within an operator & expression.",
	"[concepts][operators][logically]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(testable_conjunctive_with, testable_conjunctive_with_r),
	(
		(int, int, true),
		(int, float, false),
		(int, fail_t, false),
		(logical_target<conjunctive_with_op>, int, true),
		(logical_target<fully ^ conjunctive_with_op>, int, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"conjunctive_assign_with(_r) should determine if two types can be used within an operator &= expression.",
	"[concepts][operators][logically]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(testable_conjunctive_assign_with, testable_conjunctive_assign_with_r),
	(
		(int, int, true),
		(int, float, false),
		(int, fail_t, false),
		(logical_target<conjunctive_assign_with_op>, int, true),
		(logical_target<fully ^ conjunctive_assign_with_op>, int, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"disjunctive_with(_r) should determine if two types can be used within an operator | expression.",
	"[concepts][operators][logically]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(testable_disjunctive_with, testable_disjunctive_with_r),
	(
		(int, int, true),
		(int, float, false),
		(int, fail_t, false),
		(logical_target<disjunctive_with_op>, int, true),
		(logical_target<fully ^ disjunctive_with_op>, int, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"disjunctive_assign_with(_r) should determine if two types can be used within an operator |= expression.",
	"[concepts][operators][logically]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(testable_disjunctive_assign_with, testable_disjunctive_assign_with_r),
	(
		(int, int, true),
		(int, float, false),
		(int, fail_t, false),
		(logical_target<disjunctive_assign_with_op>, int, true),
		(logical_target<fully ^ disjunctive_assign_with_op>, int, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"exclusive_disjunctive_with(_r) should determine if two types can be used within an operator ^ expression.",
	"[concepts][operators][logically]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(testable_exclusive_disjunctive_with, testable_exclusive_disjunctive_with_r),
	(
		(int, int, true),
		(int, float, false),
		(int, fail_t, false),
		(logical_target<ex_disjunctive_with_op>, int, true),
		(logical_target<fully ^ ex_disjunctive_with_op>, int, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"exclusive_disjunctive_assign_with(_r) should determine if two types can be used within an operator ^= expression.",
	"[concepts][operators][logically]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(testable_exclusive_disjunctive_assign_with, testable_exclusive_disjunctive_assign_with_r),
	(
		(int, int, true),
		(int, float, false),
		(int, fail_t, false),
		(logical_target<ex_disjunctive_assign_with_op>, int, true),
		(logical_target<fully ^ ex_disjunctive_assign_with_op>, int, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"conjunctive(_r) should determine if a type can be used within an operator & expression.",
	"[concepts][operators][logically]",
	((class T, bool VExpected), T, VExpected),
	(testable_conjunctive, testable_conjunctive_r),
	(
		(int, true),
		(float, false),
		(fail_t, false),
		(logical_target<conjunctive_op>, true),
		(logical_target<fully ^ conjunctive_op>, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"conjunctive_assign(_r) should determine if a type can be used within an operator &= expression.",
	"[concepts][operators][logically]",
	((class T, bool VExpected), T, VExpected),
	(testable_conjunctive_assign, testable_conjunctive_assign_r),
	(
		(int, true),
		(float, false),
		(fail_t, false),
		(logical_target<conjunctive_assign_op>, true),
		(logical_target<fully ^ conjunctive_assign_op>, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"disjunctive(_r) should determine if a type can be used within an operator | expression.",
	"[concepts][operators][logically]",
	((class T, bool VExpected), T, VExpected),
	(testable_disjunctive, testable_disjunctive_r),
	(
		(int, true),
		(float, false),
		(fail_t, false),
		(logical_target<disjunctive_op>, true),
		(logical_target<fully ^ disjunctive_op>, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"disjunctive_assign(_r) should determine if a type can be used within an operator |= expression.",
	"[concepts][operators][logically]",
	((class T, bool VExpected), T, VExpected),
	(testable_disjunctive_assign, testable_disjunctive_assign_r),
	(
		(int, true),
		(float, false),
		(fail_t, false),
		(logical_target<disjunctive_assign_op>, true),
		(logical_target<fully ^ disjunctive_assign_op>, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"exclusive_disjunctive(_r) should determine if a type can be used within an operator ^ expression.",
	"[concepts][operators][logically]",
	((class T, bool VExpected), T, VExpected),
	(testable_exclusive_disjunctive, testable_exclusive_disjunctive_r),
	(
		(int, true),
		(float, false),
		(fail_t, false),
		(logical_target<ex_disjunctive_op>, true),
		(logical_target<fully ^ ex_disjunctive_op>, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"exclusive_disjunctive_assign(_r) should determine if a type can be used within an operator ^= expression.",
	"[concepts][operators][logically]",
	((class T, bool VExpected), T, VExpected),
	(testable_exclusive_disjunctive_assign, testable_exclusive_disjunctive_assign_r),
	(
		(int, true),
		(float, false),
		(fail_t, false),
		(logical_target<ex_disjunctive_assign_op>, true),
		(logical_target<fully ^ ex_disjunctive_assign_op>, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"binary logically _r concepts should determine if a return type of an expression can be converted to the expected one.",
	"[concepts][operators][logically]",
	((class T, bool VExpected, class TResult), T, VExpected, TResult),
	(testable_conjunctive_r, testable_disjunctive_r, testable_exclusive_disjunctive_r),
	(
		(int, true, int),
		(int, true, float),
		(int, false, int&),
		(int, false, fail_t),
		(logical_target<fully>, true, logical_target<fully>),
		(logical_target<fully>, false, int)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"binary logically with_r concepts should determine if a return type of an expression can be converted to the expected one.",
	"[concepts][operators][logically]",
	((class TLhs, class TRhs, bool VExpected, class TResult), TLhs, TRhs, VExpected, TResult),
	(testable_conjunctive_with_r, testable_disjunctive_with_r, testable_exclusive_disjunctive_with_r),
	(
		(int, int, true, int),
		(int, int, true, float),
		(int, int, false, int&),
		(int, int, false, fail_t),
		(logical_target<fully>, int, true, logical_target<fully>),
		(logical_target<fully>, int, false, int)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"binary logically assign_r concepts should determine if a return type of an expression can be converted to the expected one.",
	"[concepts][operators][logically]",
	((class T, bool VExpected, class TResult), T, VExpected, TResult),
	(testable_conjunctive_assign_r, testable_disjunctive_assign_r, testable_exclusive_disjunctive_assign_r),
	(
		(int, true, int),
		(int, true, float),
		(int, true, int&),
		(int, false, fail_t),
		(logical_target<fully>, true, logical_target<fully>&),
		(logical_target<fully>, false, int)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"binary logically assign_with_r concepts should determine if a return type of an expression can be converted to the expected one.",
	"[concepts][operators][logically]",
	((class TLhs, class TRhs, bool VExpected, class TResult), TLhs, TRhs, VExpected, TResult),
	(testable_conjunctive_assign_with_r, testable_disjunctive_assign_with_r, testable_exclusive_disjunctive_assign_with_r),
	(
		(int, int, true, int),
		(int, int, true, float),
		(int, int, true, int&),
		(int, int, false, fail_t),
		(logical_target<fully>, int, true, logical_target<fully>&),
		(logical_target<fully>, int, false, int)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}
