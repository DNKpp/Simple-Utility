//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/Config.hpp"

#include <catch2/catch_test_macros.hpp>

#include <concepts>
#include <regex>
#include <utility>

template <auto v>
concept version_part = requires { { v } -> std::integral; }
						&& std::cmp_less_equal(0, v);

TEST_CASE("SL_UTILITY_VERSION_MAJOR value must be convertible to an integral type.", "[config]")
{
	STATIC_REQUIRE(version_part<SL_UTILITY_VERSION_MAJOR>);
}

TEST_CASE("SL_UTILITY_VERSION_MINOR value must be convertible to an integral type.", "[config]")
{
	STATIC_REQUIRE(version_part<SL_UTILITY_VERSION_MINOR>);
}

TEST_CASE("SL_UTILITY_VERSION_PATCH value must be convertible to an integral type.", "[config]")
{
	STATIC_REQUIRE(version_part<SL_UTILITY_VERSION_PATCH>);
}

TEST_CASE("SL_UTILITY_VERSION is a dotted integer triple.", "[config]")
{
	const std::regex pattern{R"(\d+\.\d+\.\d+)"};
	std::cmatch match{};

	REQUIRE(std::regex_match(SL_UTILITY_VERSION, match, pattern));
}
