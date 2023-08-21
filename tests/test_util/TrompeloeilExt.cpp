//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/test_util/TrompeloeilExt.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

TEST_CASE("Catch2 matchers can be used as argument for the trompeloeil_ext::matches matcher", "[test_util][test_util::trompeloeil]")
{
	constexpr std::array data{42, 43, 47};

	REQUIRE(!trompeloeil_ext::matches(Catch::Matchers::RangeEquals(std::vector{42, 47})).matches(data));
	REQUIRE(trompeloeil_ext::matches(Catch::Matchers::RangeEquals(std::vector{42, 43, 47})).matches(data));
}
