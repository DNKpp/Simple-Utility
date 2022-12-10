//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>

#include "Simple-Utility/concepts/operators.hpp"

#include "../helper.hpp"

using namespace sl::concepts;

namespace
{
	enum arithmetical
	{
		none = 0,
		negate_op = 1,

		plus_op = 1 << 1,
		minus_op = 1 << 2,
		multiplies_op = 1 << 3,
		divides_op = 1 << 4,
		modulus_op = 1 << 5,

		plus_with_op = 1 << 6,
		minus_with_op = 1 << 7,
		multiplies_with_op = 1 << 8,
		divides_with_op = 1 << 9,
		modulus_with_op = 1 << 10,

		plus_assign_op = 1 << 11,
		minus_assign_op = 1 << 12,
		multiplies_assign_op = 1 << 13,
		divides_assign_op = 1 << 14,
		modulus_assign_op = 1 << 15,

		plus_assign_with_op = 1 << 16,
		minus_assign_with_op = 1 << 17,
		multiplies_assign_with_op = 1 << 18,
		divides_assign_with_op = 1 << 19,
		modulus_assign_with_op = 1 << 20,

		fully = negate_op
				| plus_op | minus_op | multiplies_op | divides_op
				| plus_with_op | minus_with_op | multiplies_with_op | divides_with_op
				| plus_assign_op | minus_assign_op | multiplies_assign_op | divides_assign_op
				| plus_assign_with_op | minus_assign_with_op | multiplies_assign_with_op | divides_assign_with_op
	};

	template <int VArithmetical = none>
	struct arithmetical_target
	{
		arithmetical_target() = default;

		explicit arithmetical_target(int)
		{ }

		arithmetical_target operator -() const
			requires ((VArithmetical & negate_op) != 0)
		{
			return {};
		}

		arithmetical_target operator +(const arithmetical_target&) const
			requires ((VArithmetical & plus_op) != 0)
		{
			return {};
		}

		arithmetical_target operator -(const arithmetical_target&) const
			requires ((VArithmetical & minus_op) != 0)
		{
			return {};
		}

		arithmetical_target operator *(const arithmetical_target&) const
			requires ((VArithmetical & multiplies_op) != 0)
		{
			return {};
		}

		arithmetical_target operator /(const arithmetical_target&) const
			requires ((VArithmetical & divides_op) != 0)
		{
			return {};
		}

		arithmetical_target operator %(const arithmetical_target&) const
			requires ((VArithmetical & modulus_op) != 0)
		{
			return {};
		}

		arithmetical_target operator +(int) const
			requires ((VArithmetical & plus_with_op) != 0)
		{
			return {};
		}

		arithmetical_target operator -(int) const
			requires ((VArithmetical & minus_with_op) != 0)
		{
			return {};
		}

		arithmetical_target operator *(int) const
			requires ((VArithmetical & multiplies_with_op) != 0)
		{
			return {};
		}

		arithmetical_target operator /(int) const
			requires ((VArithmetical & divides_with_op) != 0)
		{
			return {};
		}

		arithmetical_target operator %(int) const
			requires ((VArithmetical & modulus_with_op) != 0)
		{
			return {};
		}

		arithmetical_target& operator +=(const arithmetical_target&) const
			requires ((VArithmetical & plus_assign_op) != 0)
		{
			return *this;
		}

		arithmetical_target& operator -=(const arithmetical_target&) const
			requires ((VArithmetical & minus_assign_op) != 0)
		{
			return *this;
		}

		arithmetical_target& operator *=(const arithmetical_target&) const
			requires ((VArithmetical & multiplies_assign_op) != 0)
		{
			return *this;
		}

		arithmetical_target& operator /=(const arithmetical_target&) const
			requires ((VArithmetical & divides_assign_op) != 0)
		{
			return *this;
		}

		arithmetical_target& operator %=(const arithmetical_target&) const
			requires ((VArithmetical & modulus_assign_op) != 0)
		{
			return *this;
		}

		arithmetical_target& operator +=(int) const
			requires ((VArithmetical & plus_assign_with_op) != 0)
		{
			return *this;
		}

		arithmetical_target& operator -=(int) const
			requires ((VArithmetical & minus_assign_with_op) != 0)
		{
			return *this;
		}

		arithmetical_target& operator *=(int) const
			requires ((VArithmetical & multiplies_assign_with_op) != 0)
		{
			return *this;
		}

		arithmetical_target& operator /=(int) const
			requires ((VArithmetical & divides_assign_with_op) != 0)
		{
			return *this;
		}

		arithmetical_target& operator %=(int) const
			requires ((VArithmetical & modulus_assign_with_op) != 0)
		{
			return *this;
		}
	};

	// negate
	TESTABLE_UNARY_CONCEPT(negate);

	TESTABLE_UNARY_CONCEPT_R(negate_r);

	// plus
	TESTABLE_UNARY_CONCEPT(plus);

	TESTABLE_UNARY_CONCEPT_R(plus_r);

	TESTABLE_UNARY_CONCEPT(plus_assign);

	TESTABLE_UNARY_CONCEPT_R(plus_assign_r);

	TESTABLE_BINARY_CONCEPT(plus_with);

	TESTABLE_BINARY_CONCEPT_R(plus_with_r);

	TESTABLE_BINARY_CONCEPT(plus_assign_with);

	TESTABLE_BINARY_CONCEPT_R(plus_assign_with_r);

	// minus

	TESTABLE_UNARY_CONCEPT(minus);

	TESTABLE_UNARY_CONCEPT_R(minus_r);

	TESTABLE_BINARY_CONCEPT(minus_with);

	TESTABLE_BINARY_CONCEPT_R(minus_with_r);

	TESTABLE_UNARY_CONCEPT(minus_assign);

	TESTABLE_UNARY_CONCEPT_R(minus_assign_r);

	TESTABLE_BINARY_CONCEPT(minus_assign_with);

	TESTABLE_BINARY_CONCEPT_R(minus_assign_with_r);

	// multiplies

	TESTABLE_UNARY_CONCEPT(multiplies);

	TESTABLE_UNARY_CONCEPT_R(multiplies_r);

	TESTABLE_UNARY_CONCEPT(multiplies_assign);

	TESTABLE_UNARY_CONCEPT_R(multiplies_assign_r);

	TESTABLE_BINARY_CONCEPT(multiplies_with);

	TESTABLE_BINARY_CONCEPT_R(multiplies_with_r);

	TESTABLE_BINARY_CONCEPT(multiplies_assign_with);

	TESTABLE_BINARY_CONCEPT_R(multiplies_assign_with_r);

	// divides

	TESTABLE_UNARY_CONCEPT(divides);

	TESTABLE_UNARY_CONCEPT_R(divides_r);

	TESTABLE_UNARY_CONCEPT(divides_assign);

	TESTABLE_UNARY_CONCEPT_R(divides_assign_r);

	TESTABLE_BINARY_CONCEPT(divides_with);

	TESTABLE_BINARY_CONCEPT_R(divides_with_r);

	TESTABLE_BINARY_CONCEPT(divides_assign_with);

	TESTABLE_BINARY_CONCEPT_R(divides_assign_with_r);

	// modulus

	TESTABLE_UNARY_CONCEPT(modulus);

	TESTABLE_UNARY_CONCEPT_R(modulus_r);

	TESTABLE_UNARY_CONCEPT(modulus_assign);

	TESTABLE_UNARY_CONCEPT_R(modulus_assign_r);

	TESTABLE_BINARY_CONCEPT(modulus_with);

	TESTABLE_BINARY_CONCEPT_R(modulus_with_r);

	TESTABLE_BINARY_CONCEPT(modulus_assign_with);

	TESTABLE_BINARY_CONCEPT_R(modulus_assign_with_r);

	// arithmetically combinable

	TESTABLE_UNARY_CONCEPT(arithmetically_combinable);

	TESTABLE_UNARY_CONCEPT_R(arithmetically_combinable_r);

	TESTABLE_BINARY_CONCEPT(arithmetically_combinable_with);

	TESTABLE_BINARY_CONCEPT_R(arithmetically_combinable_with_r);

	// arithmetically assignable

	TESTABLE_UNARY_CONCEPT(arithmetically_assignable);

	TESTABLE_UNARY_CONCEPT_R(arithmetically_assignable_r);

	TESTABLE_BINARY_CONCEPT(arithmetically_assignable_with);

	TESTABLE_BINARY_CONCEPT_R(arithmetically_assignable_with_r);

	// fully arithmetically

	TESTABLE_UNARY_CONCEPT(fully_arithmetical);

	TESTABLE_UNARY_CONCEPT_R_R(fully_arithmetical_r);

	TESTABLE_BINARY_CONCEPT(fully_arithmetical_with);

	TESTABLE_BINARY_CONCEPT_R_R(fully_arithmetical_with_r);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG(
	"negate(_r) should determine if a type can be used within an unary operator - expression.",
	"[concepts][operators][arithmetic]",
	((class T, bool VExpected), T, VExpected),
	(is_negate, is_negate_r),
	(
		(int, true),
		(arithmetical_target<negate_op>, true),
		(arithmetical_target<fully ^ negate_op>, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG(
	"negate_r should determine if a return type of the expression can be converted to the expected one.",
	"[concepts][operators][arithmetic]",
	((class T, class TResult, bool VExpected), T, TResult, VExpected),
	(int, int, true),
	(int, float, true),
	(int, int&, false),
	(int, fail_t, false),
	(arithmetical_target<fully>, arithmetical_target<fully>, true),
	(arithmetical_target<fully>, int, false)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(is_r_negate_r<T, TResult, VExpected>::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG(
	"plus_with(_r) should determine if two types can be used within an operator + expression.",
	"[concepts][operators][arithmetic]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(is_plus_with, is_plus_with_r),
	(
		(int, int, true),
		(int, float, true),
		(int, fail_t, false),
		(arithmetical_target<plus_with_op>, int, true),
		(arithmetical_target<fully ^ plus_with_op>, int, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG(
	"plus_assign_with(_r) should determine if two types can be used within an operator += expression.",
	"[concepts][operators][arithmetic]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(is_plus_assign_with, is_plus_assign_with_r),
	(
		(int, int, true),
		(int, float, true),
		(int, fail_t, false),
		(arithmetical_target<plus_assign_with_op>, int, true),
		(arithmetical_target<fully ^ plus_assign_with_op>, int, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG(
	"minus_with(_r) should determine if two types can be used within an operator - expression.",
	"[concepts][operators][arithmetic]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(is_minus_with, is_minus_with_r),
	(
		(int, int, true),
		(int, float, true),
		(int, fail_t, false),
		(arithmetical_target<minus_with_op>, int, true),
		(arithmetical_target<fully ^ minus_with_op>, int, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG(
	"minus_assign_with(_r) should determine if two types can be used within an operator -= expression.",
	"[concepts][operators][arithmetic]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(is_minus_assign_with, is_minus_assign_with_r),
	(
		(int, int, true),
		(int, float, true),
		(int, fail_t, false),
		(arithmetical_target<minus_assign_with_op>, int, true),
		(arithmetical_target<fully ^ minus_assign_with_op>, int, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG(
	"multiplies_with(_r) should determine if two types can be used within an operator * expression.",
	"[concepts][operators][arithmetic]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(is_multiplies_with, is_multiplies_with_r),
	(
		(int, int, true),
		(int, float, true),
		(int, fail_t, false),
		(arithmetical_target<multiplies_with_op>, int, true),
		(arithmetical_target<fully ^ multiplies_with_op>, int, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG(
	"multiplies_assign_with(_r) should determine if two types can be used within an operator *= expression.",
	"[concepts][operators][arithmetic]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(is_multiplies_assign_with, is_multiplies_assign_with_r),
	(
		(int, int, true),
		(int, float, true),
		(int, fail_t, false),
		(arithmetical_target<multiplies_assign_with_op>, int, true),
		(arithmetical_target<fully ^ multiplies_assign_with_op>, int, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG(
	"divides_with(_r) should determine if two types can be used within an operator / expression.",
	"[concepts][operators][arithmetic]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(is_divides_with, is_divides_with_r),
	(
		(int, int, true),
		(int, float, true),
		(int, fail_t, false),
		(arithmetical_target<divides_with_op>, int, true),
		(arithmetical_target<fully ^ divides_with_op>, int, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG(
	"divides_assign_with(_r) should determine if two types can be used within an operator /= expression.",
	"[concepts][operators][arithmetic]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(is_divides_assign_with, is_divides_assign_with_r),
	(
		(int, int, true),
		(int, float, true),
		(int, fail_t, false),
		(arithmetical_target<divides_assign_with_op>, int, true),
		(arithmetical_target<fully ^ divides_assign_with_op>, int, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG(
	"modulus_with(_r) should determine if two types can be used within an operator % expression.",
	"[concepts][operators][arithmetic]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(is_modulus_with, is_modulus_with_r),
	(
		(int, int, true),
		(int, float, false),
		(int, fail_t, false),
		(arithmetical_target<modulus_with_op>, int, true),
		(arithmetical_target<fully>, int, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG(
	"modulus_assign_with(_r) should determine if two types can be used within an operator %= expression.",
	"[concepts][operators][arithmetic]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(is_modulus_assign_with, is_modulus_assign_with_r),
	(
		(int, int, true),
		(int, float, false),
		(int, fail_t, false),
		(arithmetical_target<modulus_assign_with_op>, int, true),
		(arithmetical_target<fully>, int, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG(
	"plus(_r) should determine if a type can be used within an operator + expression.",
	"[concepts][operators][arithmetic]",
	((class T, bool VExpected), T, VExpected),
	(is_plus, is_plus_r),
	(
		(int, true),
		(float, true),
		(fail_t, false),
		(arithmetical_target<plus_op>, true),
		(arithmetical_target<fully ^ plus_op>, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG(
	"plus_assign(_r) should determine if a type can be used within an operator += expression.",
	"[concepts][operators][arithmetic]",
	((class T, bool VExpected), T, VExpected),
	(is_plus_assign, is_plus_assign_r),
	(
		(int, true),
		(float, true),
		(fail_t, false),
		(arithmetical_target<plus_assign_op>, true),
		(arithmetical_target<fully ^ plus_assign_op>, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG(
	"minus(_r) should determine if a type can be used within an operator - expression.",
	"[concepts][operators][arithmetic]",
	((class T, bool VExpected), T, VExpected),
	(is_minus, is_minus_r),
	(
		(int, true),
		(float, true),
		(fail_t, false),
		(arithmetical_target<minus_op>, true),
		(arithmetical_target<fully ^ minus_op>, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG(
	"minus_assign(_r) should determine if a type can be used within an operator -= expression.",
	"[concepts][operators][arithmetic]",
	((class T, bool VExpected), T, VExpected),
	(is_minus_assign, is_minus_assign_r),
	(
		(int, true),
		(float, true),
		(fail_t, false),
		(arithmetical_target<minus_assign_op>, true),
		(arithmetical_target<fully ^ minus_assign_op>, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG(
	"multiplies(_r) should determine if a type can be used within an operator * expression.",
	"[concepts][operators][arithmetic]",
	((class T, bool VExpected), T, VExpected),
	(is_multiplies, is_multiplies_r),
	(
		(int, true),
		(float, true),
		(fail_t, false),
		(arithmetical_target<multiplies_op>, true),
		(arithmetical_target<fully ^ multiplies_op>, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG(
	"multiplies_assign(_r) should determine if a type can be used within an operator *= expression.",
	"[concepts][operators][arithmetic]",
	((class T, bool VExpected), T, VExpected),
	(is_multiplies_assign, is_multiplies_assign_r),
	(
		(int, true),
		(float, true),
		(fail_t, false),
		(arithmetical_target<multiplies_assign_op>, true),
		(arithmetical_target<fully ^ multiplies_assign_op>, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG(
	"divides(_r) should determine if a type can be used within an operator / expression.",
	"[concepts][operators][arithmetic]",
	((class T, bool VExpected), T, VExpected),
	(is_divides, is_divides_r),
	(
		(int, true),
		(float, true),
		(fail_t, false),
		(arithmetical_target<divides_op>, true),
		(arithmetical_target<fully ^ divides_op>, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG(
	"divides_assign(_r) should determine if a type can be used within an operator /= expression.",
	"[concepts][operators][arithmetic]",
	((class T, bool VExpected), T, VExpected),
	(is_divides_assign, is_divides_assign_r),
	(
		(int, true),
		(float, true),
		(fail_t, false),
		(arithmetical_target<divides_assign_op>, true),
		(arithmetical_target<fully ^ divides_assign_op>, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG(
	"modulus(_r) should determine if a type can be used within an operator % expression.",
	"[concepts][operators][arithmetic]",
	((class T, bool VExpected), T, VExpected),
	(is_modulus, is_modulus_r),
	(
		(int, true),
		(float, false),
		(fail_t, false),
		(arithmetical_target<modulus_op>, true),
		(arithmetical_target<fully>, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG(
	"modulus_assign(_r) should determine if a type can be used within an operator %= expression.",
	"[concepts][operators][arithmetic]",
	((class T, bool VExpected), T, VExpected),
	(is_modulus_assign, is_modulus_assign_r),
	(
		(int, true),
		(float, false),
		(fail_t, false),
		(arithmetical_target<modulus_assign_op>, true),
		(arithmetical_target<fully>, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG(
	"binary arithmetically _r concepts should determine if a return type of an expression can be converted to the expected one.",
	"[concepts][operators][arithmetic]",
	((class T, class TResult, bool VExpected), T, TResult, VExpected),
	(is_r_plus_r, is_r_minus_r, is_r_multiplies_r, is_r_divides_r, is_r_modulus_r, is_r_arithmetically_combinable_r),
	(
		(int, int, true),
		(int, float, true),
		(int, int&, false),
		(int, fail_t, false),
		(arithmetical_target<fully | modulus_op>, arithmetical_target<fully | modulus_op>, true),
		(arithmetical_target<fully>, int, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG(
	"binary arithmetically with_r concepts should determine if a return type of an expression can be converted to the expected one.",
	"[concepts][operators][arithmetic]",
	((class TLhs, class TRhs, class TResult, bool VExpected), TLhs, TRhs, TResult, VExpected),
	(is_r_plus_with_r, is_r_minus_with_r, is_r_multiplies_with_r, is_r_divides_with_r, is_r_modulus_with_r,
		is_r_arithmetically_combinable_with_r),
	(
		(int, int, int, true),
		(int, int, float, true),
		(int, int, int&, false),
		(int, int, fail_t, false),
		(arithmetical_target<fully | modulus_with_op>, int, arithmetical_target<fully | modulus_with_op>, true),
		(arithmetical_target<fully>, int, int, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG(
	"binary arithmetically assign_r concepts should determine if a return type of an expression can be converted to the expected one.",
	"[concepts][operators][arithmetic]",
	((class T, class TResult, bool VExpected), T, TResult, VExpected),
	(is_r_plus_assign_r, is_r_minus_assign_r, is_r_multiplies_assign_r, is_r_divides_assign_r, is_r_modulus_assign_r,
		is_r_arithmetically_assignable_r),
	(
		(int, int, true),
		(int, float, true),
		(int, int&, true),
		(int, fail_t, false),
		(arithmetical_target<fully | modulus_assign_op>, arithmetical_target<fully | modulus_assign_op>&, true),
		(arithmetical_target<fully>, int, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG(
	"binary arithmetically assign_with_r concepts should determine if a return type of an expression can be converted to the expected one.",
	"[concepts][operators][arithmetic]",
	((class TLhs, class TRhs, class TResult, bool VExpected), TLhs, TRhs, TResult, VExpected),
	(is_r_plus_assign_with_r, is_r_minus_assign_with_r, is_r_multiplies_assign_with_r, is_r_divides_assign_with_r,
		is_r_modulus_assign_with_r, is_r_arithmetically_assignable_with_r),
	(
		(int, int, int, true),
		(int, int, float, true),
		(int, int, int&, true),
		(int, int, fail_t, false),
		(arithmetical_target<fully | modulus_assign_with_op>, int, arithmetical_target<fully | modulus_assign_with_op>&, true),
		(arithmetical_target<fully>, int, int, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"arithmetically_combinable(_r) should determine if a type can be used within operator +, -, * and / expressions.",
	"[concepts][operators][arithmetic]",
	((class T, bool VExpected), T, VExpected),
	(is_arithmetically_combinable, is_arithmetically_combinable_r),
	(
		(int, true),
		(float, true),
		(fail_t, false),
		(arithmetical_target<fully>, true),
		(arithmetical_target<fully ^ plus_op>, false),
		(arithmetical_target<fully ^ minus_op>, false),
		(arithmetical_target<fully ^ multiplies_op>, false),
		(arithmetical_target<fully ^ divides_op>, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"arithmetically_combinable_with(_r) should determine if two types can be used within operator +, -, * and / expressions.",
	"[concepts][operators][arithmetic]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(is_arithmetically_combinable_with, is_arithmetically_combinable_with_r),
	(
		(int, int, true),
		(int, float, true),
		(int, fail_t, false),
		(arithmetical_target<fully>, int, true),
		(arithmetical_target<fully ^ plus_with_op>, int, false),
		(arithmetical_target<fully ^ minus_with_op>, int, false),
		(arithmetical_target<fully ^ multiplies_with_op>, int, false),
		(arithmetical_target<fully ^ divides_with_op>, int, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"arithmetically_assignable(_r) should determine if a type can be used within operator +=, -=, *= and /= expressions.",
	"[concepts][operators][arithmetic]",
	((class T, bool VExpected), T, VExpected),
	(is_arithmetically_assignable, is_arithmetically_assignable_r),
	(
		(int, true),
		(float, true),
		(fail_t, false),
		(arithmetical_target<fully>, true),
		(arithmetical_target<fully ^ plus_assign_op>, false),
		(arithmetical_target<fully ^ minus_assign_op>, false),
		(arithmetical_target<fully ^ multiplies_assign_op>, false),
		(arithmetical_target<fully ^ divides_assign_op>, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"arithmetically_assignable_with(_r) should determine if two types can be used within operator +=, -=, *= and /= expressions.",
	"[concepts][operators][arithmetic]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(is_arithmetically_assignable_with, is_arithmetically_assignable_with_r),
	(
		(int, int, true),
		(int, float, true),
		(int, fail_t, false),
		(arithmetical_target<fully>, int, true),
		(arithmetical_target<fully ^ plus_assign_with_op>, int, false),
		(arithmetical_target<fully ^ minus_assign_with_op>, int, false),
		(arithmetical_target<fully ^ multiplies_assign_with_op>, int, false),
		(arithmetical_target<fully ^ divides_assign_with_op>, int, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"fully_arithmetical_with(_r) should determine if two types can be used within operator +, -, *, /, +=, -=, *=, /= and unary - expressions.",
	"[concepts][operators][arithmetic]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(is_fully_arithmetical_with, is_fully_arithmetical_with_r),
	(
		(int, int, true),
		(int, float, true),
		(int, fail_t, false),
		(arithmetical_target<fully>, int, true),
		(arithmetical_target<fully ^ negate_op>, int, false),
		(arithmetical_target<fully ^ plus_with_op>, int, false),
		(arithmetical_target<fully ^ minus_with_op>, int, false),
		(arithmetical_target<fully ^ multiplies_with_op>, int, false),
		(arithmetical_target<fully ^ divides_with_op>, int, false),
		(arithmetical_target<fully ^ plus_assign_with_op>, int, false),
		(arithmetical_target<fully ^ minus_assign_with_op>, int, false),
		(arithmetical_target<fully ^ multiplies_assign_with_op>, int, false),
		(arithmetical_target<fully ^ divides_assign_with_op>, int, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"fully_arithmetical_with_r should determine if the return types of each expression can be converted to the expected one.",
	"[concepts][operators][arithmetic]",
	((class TLhs, class TRhs, class TCombineResult, class TAssignResult, bool VExpected),
		TLhs, TRhs, TCombineResult, TAssignResult, VExpected),
	(is_rr_fully_arithmetical_with_r),
	(
		(int, int, int, int&, true),
		(int, int, int, int&&, false),
		(arithmetical_target<fully>, int, arithmetical_target<fully>, arithmetical_target<fully>&, true),
		(arithmetical_target<fully>, int, int, arithmetical_target<fully>&, false),
		(arithmetical_target<fully>, int, arithmetical_target<fully>, arithmetical_target<fully>&&, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"fully_arithmetical(_r) should determine a type can be used within operator +, -, *, /, +=, -=, *=, /= and unary - expressions.",
	"[concepts][operators][arithmetic]",
	((class T, bool VExpected), T, VExpected),
	(is_fully_arithmetical, is_fully_arithmetical_r),
	(
		(int, true),
		(float, true),
		(fail_t, false),
		(arithmetical_target<fully>, true),
		(arithmetical_target<fully ^ negate_op>, false),
		(arithmetical_target<fully ^ plus_op>, false),
		(arithmetical_target<fully ^ minus_op>, false),
		(arithmetical_target<fully ^ multiplies_op>, false),
		(arithmetical_target<fully ^ divides_op>, false),
		(arithmetical_target<fully ^ plus_assign_op>, false),
		(arithmetical_target<fully ^ minus_assign_op>, false),
		(arithmetical_target<fully ^ multiplies_assign_op>, false),
		(arithmetical_target<fully ^ divides_assign_op>, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"fully_arithmetical_r should determine if the return types of each expression can be converted to the expected one.",
	"[concepts][operators][arithmetic]",
	((class T, class TCombineResult, class TAssignResult, bool VExpected), T, TCombineResult, TAssignResult, VExpected),
	(is_rr_fully_arithmetical_r),
	(
		(int, int, int&, true),
		(int, int, int&&, false),
		(arithmetical_target<fully>, arithmetical_target<fully>, arithmetical_target<fully>&, true),
		(arithmetical_target<fully>, int, arithmetical_target<fully>&, false),
		(arithmetical_target<fully>, arithmetical_target<fully>, arithmetical_target<fully>&&, false)
	)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(TestType::value);
}
