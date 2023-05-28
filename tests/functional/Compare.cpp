//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "../helper.hpp"

#include "Simple-Utility/functional/Compare.hpp"

using namespace sl::functional;

TEMPLATE_TEST_CASE("less compares its two parameters", "[functional][compare]", int, float)
{
	const auto& [value1, value2, expectedResult] = GENERATE(
		(table<int, int, bool>)({
			{ 42, 43, true },
			{ 42, 42, false }
			})
	);

	const bool result = compare::less(static_cast<TestType>(value1), static_cast<TestType>(value2));

	REQUIRE(result == expectedResult);
}

TEMPLATE_TEST_CASE("greater compares its two parameters", "[functional][compare]", int, float)
{
	const auto& [value1, value2, expectedResult] = GENERATE(
		(table<int, int, bool>)({
			{ 43, 42, true },
			{ 42, 42, false }
			})
	);

	const bool result = compare::greater(static_cast<TestType>(value1), static_cast<TestType>(value2));

	REQUIRE(result == expectedResult);
}

TEMPLATE_TEST_CASE("less_equal compares its two parameters", "[functional][compare]", int, float)
{
	const auto& [value1, value2, expectedResult] = GENERATE(
		(table<int, int, bool>)({
			{ 41, 42, true },
			{ 42, 42, true },
			{ 43, 42, false }
			})
	);

	const bool result = compare::less_equal(static_cast<TestType>(value1), static_cast<TestType>(value2));

	REQUIRE(result == expectedResult);
}

TEMPLATE_TEST_CASE("greater_equal compares its two parameters", "[functional][compare]", int, float)
{
	const auto& [value1, value2, expectedResult] = GENERATE(
		(table<int, int, bool>)({
			{ 43, 42, true },
			{ 42, 42, true },
			{ 41, 42, false }
			})
	);

	const bool result = compare::greater_equal(static_cast<TestType>(value1), static_cast<TestType>(value2));

	REQUIRE(result == expectedResult);
}

TEMPLATE_TEST_CASE("equal compares its two parameters", "[functional][compare]", int, float)
{
	const auto& [value1, value2, expectedResult] = GENERATE(
		(table<int, int, bool>)({
			{ 42, 42, true },
			{ 43, 42, false }
			})
	);

	const bool result = compare::equal(static_cast<TestType>(value1), static_cast<TestType>(value2));

	REQUIRE(result == expectedResult);
}

TEMPLATE_TEST_CASE("not_equal compares its two parameters", "[functional][compare]", int, float)
{
	const auto& [value1, value2, expectedResult] = GENERATE(
		(table<int, int, bool>)({
			{ 43, 42, true },
			{ 42, 42, false }
			})
	);

	const bool result = compare::not_equal(static_cast<TestType>(value1), static_cast<TestType>(value2));

	REQUIRE(result == expectedResult);
}

TEST_CASE("three_way compares its two parameters", "[functional][compare]")
{
	const auto& [value1, value2, expectedResult] = GENERATE(
		(table<int, int, std::strong_ordering>)({
			{ 43, 42, std::strong_ordering::greater },
			{ 42, 42, std::strong_ordering::equal }
			})
	);

	const std::strong_ordering result = compare::three_way(value1, value2);

	REQUIRE(result == expectedResult);
}
