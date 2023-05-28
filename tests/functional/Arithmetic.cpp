//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "../helper.hpp"

#include "Simple-Utility/functional/Arithmetic.hpp"

using namespace sl::functional;

TEST_CASE("plus forwards the params to operator + and returns the result.", "[functional][transform][arithmetic]")
{
	const auto [lhs, rhs, expectedResult] = GENERATE(
		(table<int, int, int>)({
			{0, 0, 0},
			{0, 1, 1},
			{1, 0, 1},
			{1, 1, 2}
			})
	);

	REQUIRE(arithmetic::plus(lhs, rhs) == expectedResult);
}

TEST_CASE("minus forwards the params to operator - and returns the result.", "[functional][transform][arithmetic]")
{
	const auto [lhs, rhs, expectedResult] = GENERATE(
		(table<int, int, int>)({
			{0, 0, 0},
			{0, 1, -1},
			{1, 0, 1},
			{1, 1, 0}
			})
	);

	REQUIRE(arithmetic::minus(lhs, rhs) == expectedResult);
}

TEST_CASE("multiplies forwards the params to operator * and returns the result.", "[functional][transform][arithmetic]")
{
	const auto [lhs, rhs, expectedResult] = GENERATE(
		(table<int, int, int>)({
			{1, 1, 1},
			{0, 1, 0},
			{1, 0, 0},
			{2, 1, 2},
			{1, 2, 2}
			})
	);

	REQUIRE(arithmetic::multiplies(lhs, rhs) == expectedResult);
}

TEST_CASE("divides forwards the params to operator / and returns the result.", "[functional][transform][arithmetic]")
{
	const auto [lhs, rhs, expectedResult] = GENERATE(
		(table<int, int, int>)({
			{1, 1, 1},
			{0, 1, 0},
			{2, 1, 2},
			{1, 2, 0}
			})
	);

	REQUIRE(arithmetic::divides(lhs, rhs) == expectedResult);
}

TEST_CASE("modulus forwards the params to operator % and returns the result.", "[functional][transform][arithmetic]")
{
	const auto [lhs, rhs, expectedResult] = GENERATE(
		(table<int, int, int>)({
			{1, 1, 0},
			{0, 1, 0},
			{2, 1, 0},
			{1, 2, 1}
			})
	);

	REQUIRE(arithmetic::modulus(lhs, rhs) == expectedResult);
}

TEST_CASE("negate forwards the params to unary operator - and returns the result.", "[functional][transform][arithmetic]")
{
	const auto [value, expectedResult] = GENERATE(
		(table<int, int>)({
			{1, -1},
			{0, 0},
			{-1, 1}
			})
	);

	REQUIRE(arithmetic::negate(value) == expectedResult);
}
