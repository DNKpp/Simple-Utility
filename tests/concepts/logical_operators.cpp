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
