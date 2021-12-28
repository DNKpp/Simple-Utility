//          Copyright Dominic Koepke 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch.hpp>

#include "../helper.hpp"

#include "Simple-Utility/concepts/stl_symmetrical.hpp"

using namespace sl::concepts;

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"constructible_by should behave like std::constructible_from with one constructor argument",
	"[concepts][stl_ext]",
	((class TSource, class TTarget, bool VExpected), TSource, TTarget, VExpected),
	(int, int, true),
	(fail_t, int, false),
	(int, fail_t, false),
	(int, float, true)
)
#pragma warning(default: 26444)
{
	REQUIRE(constructible_by<TSource, TTarget> == VExpected);
}
