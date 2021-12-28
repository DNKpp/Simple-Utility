//          Copyright Dominic Koepke 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch.hpp>

#include "unique_handle.hpp"

using namespace sl;

TEST_CASE("unique_handle should be default constructible.", "[unique_handle]")
{
	unique_handle<int> handle{};

	// nothing to check here
}

TEST_CASE("default constructed unique_handle should not contain a value.", "[unique_handle]")
{
	const unique_handle<int> handle{};

	REQUIRE(!handle.has_value());
	REQUIRE(!handle);
}
