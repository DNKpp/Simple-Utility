//          Copyright Dominic Koepke 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch.hpp>

#include "Simple-Utility/concepts/operators.hpp"

using namespace sl::concepts;

namespace
{
	template <class T>
	struct target_t
	{
		constexpr target_t() = default;

		constexpr T operator *()
		{
			return {};
		}

		constexpr T operator ->()
		{
			return {};
		}
	};
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"dereferencable should determine if a type can be used in unary operator * expressions.",
	"[concepts][operators][access]",
	((class T, bool VExpected), T, VExpected),
	(int, false),
	(target_t<int>, true),
	(const target_t<int>, false)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(dereferencable<T> == VExpected);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"dereferencable_r should determine if a type can be used in unary operator * expressions and the return type is convertible to the expected type.",
	"[concepts][operators][access]",
	((class T, class TReturn, bool VExpected), T, TReturn, VExpected),
	(int, int, false),
	(target_t<int>, int, true),
	(const target_t<int>, int, false),
	(target_t<int>, target_t<int>, false)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(dereferencable_r<T, TReturn> == VExpected);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"arrow_dereferencable should determine if a type can be used in unary operator -> expressions.",
	"[concepts][operators][access]",
	((class T, bool VExpected), T, VExpected),
	(int, false),
	(target_t<int>, true),
	(const target_t<int>, false)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(arrow_dereferencable<T> == VExpected);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"arrow_dereferencable_r should determine if a type can be used in unary operator -> expressions and the return type is convertible to the expected type.",
	"[concepts][operators][access]",
	((class T, class TReturn, bool VExpected), T, TReturn, VExpected),
	(int, int, false),
	(target_t<int>, int, true),
	(const target_t<int>, int, false),
	(target_t<int>, target_t<int>, false)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(arrow_dereferencable_r<T, TReturn> == VExpected);
}
