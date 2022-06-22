//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>

#include "Simple-Utility/functional/tuple.hpp"

using namespace sl::functional;

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG(
	"get forwards tuple to the specific get implementation and returns as received.",
	"[functional]",
	((class TQuery, class TExpected, auto... VValue), TQuery, TExpected, VValue...),
	(char, char&, 'c'),
	( int, int&, 'c', 42),
	(char, char&, 'c', 42)
)
#pragma warning(default: 26444)
{
	std::tuple tuple{ VValue... };

	TExpected e = tuple::get<TQuery>(tuple);

	REQUIRE(std::get<TQuery>(tuple) == e);
}

TEMPLATE_TEST_CASE_SIG(
	"get_at forwards tuple to the specific get implementation and returns as received.",
	"[functional]",
	((std::size_t VIndex, class TExpected, auto... VValue), VIndex, TExpected, VValue...),
	(0, char&, 'c'),
	(1, int&, 'c', 42),
	(0, char&, 'c', 42)
)
{
	std::tuple tuple{ VValue... };

	TExpected e = tuple::get_at<VIndex>(tuple);

	REQUIRE(std::get<VIndex>(tuple) == e);
}

