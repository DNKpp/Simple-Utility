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

	struct fail {};
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

