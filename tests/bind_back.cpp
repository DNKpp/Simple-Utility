//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_test_macros.hpp>

#include "Simple-Utility/bind_back.hpp"

using namespace sl;

TEST_CASE("bind_back binds value as tail to given functional", "[functional]")
{
	const auto binaryFunc = [](const int x, const std::string& str) { return x + std::stoi(str); };
	auto curriedFunc = bind_back(binaryFunc, "1337");

	REQUIRE(curriedFunc(42) == 1379);
	REQUIRE(std::as_const(curriedFunc)(42) == 1379);
	REQUIRE(std::move(curriedFunc)(42) == 1379);
}

TEST_CASE("bind_back binds multiple values as tail to given functional", "[functional]")
{
	const auto binaryFunc = [](const int x, const std::string& str) { return x + std::stoi(str); };
	auto curriedFunc = bind_back(binaryFunc, 42, "1337");

	REQUIRE(curriedFunc() == 1379);
	REQUIRE(std::as_const(curriedFunc)() == 1379);
	REQUIRE(std::move(curriedFunc)() == 1379);
}
