//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/test_util/Catch2Ext.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

TEST_CASE("catch_ext::RangesEmpty matches empty ranges.", "[test_util][test_util::catch2]")
{
	REQUIRE_THAT(std::vector<int>{}, catch_ext::RangesEmpty{});
	REQUIRE_THAT(std::vector{42}, !catch_ext::RangesEmpty{});
}
