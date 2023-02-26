//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>

#include "Simple-Utility/concepts/utility.hpp"

using namespace sl::concepts;

TEMPLATE_TEST_CASE_SIG(
	"unique_types and are_types_unique_v determine if all types of a param pack are unique.",
	"[concepts]",
	((bool VExpected, class... TArgs), VExpected, TArgs...),
	(false, int, int),
	(true, int, float, int&, int&&, const int),
	(false, int, float, int&, int&&, const int, int&)
)
{
	STATIC_REQUIRE(are_types_unique_v<TArgs...> == VExpected);
	STATIC_REQUIRE(unique_types<TArgs...> == VExpected);
}
