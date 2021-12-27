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
		(float, false)
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
		(empty_t, false)
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
		(int, false, empty_t)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"binary logically with(_r) should determine if two types can be used within the specific operator expression.",
	"[concepts][operators][logically]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(testable_conjunctive_with, testable_conjunctive_with_r,
		testable_conjunctive_assign_with, testable_conjunctive_assign_with_r,
		testable_disjunctive_with, testable_disjunctive_with_r,
		testable_disjunctive_assign_with, testable_disjunctive_assign_with_r,
		testable_exclusive_disjunctive_with, testable_exclusive_disjunctive_with_r,
		testable_exclusive_disjunctive_assign_with, testable_exclusive_disjunctive_assign_with_r),
	(
		(int, int, true),
		(int, float, false),
		(int, empty_t, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"binary logically (_r) should determine if a type can be used within the specific operator expression.",
	"[concepts][operators][logically]",
	((class T, bool VExpected), T, VExpected),
	(testable_conjunctive, testable_conjunctive_r,
		testable_conjunctive_assign, testable_conjunctive_assign_r,
		testable_disjunctive, testable_disjunctive_r,
		testable_disjunctive_assign, testable_disjunctive_assign_r,
		testable_exclusive_disjunctive, testable_exclusive_disjunctive_r,
		testable_exclusive_disjunctive_assign, testable_exclusive_disjunctive_assign_r),
	(
		(int, true),
		(float, false),
		(empty_t, false)
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
		(int, false, empty_t)
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
		(int, int, false, empty_t)
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
		(int, false, empty_t)
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
		(int, int, false, empty_t)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}
