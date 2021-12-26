//          Copyright Dominic Koepke 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch.hpp>

#include "Simple-Utility/concepts/operators.hpp"

using namespace sl::concepts;

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"right_shiftable_with should determine if two types can be used within an operator >> expression.",
	"[concepts][operators]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(int, int, true),
	(float, int, false),
	(int, float, false)
)
#pragma warning(default: 26444)
{
	REQUIRE(right_shiftable_with<TLhs, TRhs> == VExpected);
}
