//          Copyright Dominic Koepke 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch.hpp>

#include "Simple-Utility/concepts/operators.hpp"

using namespace sl::concepts;

namespace
{
	template <class T, bool VResult>
	struct complemented_testable
	{
		static constexpr bool value{ complemented<T> == VResult };
	};

	template <class T, bool VResult, class... TArgs>
	struct complemented_r_testable
	{
		static constexpr bool value{ complemented_r<T, TArgs...> == VResult };
	};

	template <class T, bool VResult>
	struct invertible_testable
	{
		static constexpr bool value{ invertible<T> == VResult };
	};

	template <class T, bool VResult, class... TArgs>
	struct invertible_r_testable
	{
		static constexpr bool value{ invertible_r<T, TArgs...> == VResult };
	};

	template <class TLhs, class TRhs, bool VResult>
	struct conjunctive_with_testable
	{
		static constexpr bool value{ conjunctive_with<TLhs, TRhs> == VResult };
	};

	template <class TLhs, class TRhs, bool VResult, class... TArgs>
	struct conjunctive_with_r_testable
	{
		static constexpr bool value{ conjunctive_with_r<TLhs, TRhs, TArgs...> == VResult };
	};

	template <class T, bool VResult>
	struct conjunctive_testable
	{
		static constexpr bool value{ conjunctive<T> == VResult };
	};

	template <class T, bool VResult, class... TArgs>
	struct conjunctive_r_testable
	{
		static constexpr bool value{ conjunctive_r<T, TArgs...> == VResult };
	};

	template <class TLhs, class TRhs, bool VResult>
	struct disjunctive_with_testable
	{
		static constexpr bool value{ disjunctive_with<TLhs, TRhs> == VResult };
	};

	template <class TLhs, class TRhs, bool VResult, class... TArgs>
	struct disjunctive_with_r_testable
	{
		static constexpr bool value{ disjunctive_with_r<TLhs, TRhs, TArgs...> == VResult };
	};

	template <class T, bool VResult>
	struct disjunctive_testable
	{
		static constexpr bool value{ disjunctive<T> == VResult };
	};

	template <class T, bool VResult, class... TArgs>
	struct disjunctive_r_testable
	{
		static constexpr bool value{ disjunctive_r<T, TArgs...> == VResult };
	};

	struct fail
	{
	};
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"complemented(_r) should determine if a type can be used within an operator ~ expression.",
	"[concepts][operators]",
	((class T, bool VExpected), T, VExpected),
	(complemented_testable, complemented_r_testable),
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
	"[concepts][operators]",
	((class T, bool VExpected), T, VExpected),
	(invertible_testable, invertible_r_testable),
	(
		(int, true),
		(float, true),
		(fail, false)
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
	"[concepts][operators]",
	((class T, bool VExpected, class TResult), T, VExpected, TResult),
	(complemented_r_testable, invertible_r_testable),
	(
		(int, true, int),
		(int, true, float),
		(int, false, int&),
		(int, false, fail)
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
	"[concepts][operators]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(conjunctive_with_testable, conjunctive_with_r_testable,
		disjunctive_with_testable, disjunctive_with_r_testable),
	(
		(int, int, true),
		(int, float, false),
		(int, fail, false)
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
	"[concepts][operators]",
	((class T, bool VExpected), T, VExpected),
	(conjunctive_testable, conjunctive_r_testable, disjunctive_testable, disjunctive_r_testable),
	(
		(int, true),
		(float, false),
		(fail, false)
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
	"[concepts][operators]",
	((class T, bool VExpected, class TResult), T, VExpected, TResult),
	(conjunctive_r_testable, disjunctive_r_testable),
	(
		(int, true, int),
		(int, true, float),
		(int, false, int&),
		(int, false, fail)
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
	"[concepts][operators]",
	((class TLhs, class TRhs, bool VExpected, class TResult), TLhs, TRhs, VExpected, TResult),
	(conjunctive_with_r_testable, disjunctive_with_r_testable),
	(
		(int, int, true, int),
		(int, int, true, float),
		(int, int, false, int&),
		(int, int, false, fail)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}
