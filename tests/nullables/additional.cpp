//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>

// test all-in-one include here
#include "Simple-Utility/nullables.hpp"

using namespace sl::nullables;

TEMPLATE_TEST_CASE(
	"provided types satisfy the input_nullables requirements",
	"[nullables][concept]",
	std::unique_ptr<int>,
	std::shared_ptr<int>,
	std::optional<int>
)
{
	STATIC_REQUIRE(input_nullable<TestType>);
}

TEMPLATE_TEST_CASE(
	"provided types satisfy the nullables requirements",
	"[nullables][concept]",
	std::unique_ptr<int>,
	std::shared_ptr<int>,
	std::optional<int>
)
{
	STATIC_REQUIRE(nullable<TestType>);
}
