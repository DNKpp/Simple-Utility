//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/catch_approx.hpp>

#include "Simple-Utility/math.hpp"

using namespace sl;

TEMPLATE_TEST_CASE(
	"modf forwards the result from std::modf",
	"[math]",
	float,
	double,
	long double
)
{
	using T = TestType;

	// examples taken from https://en.cppreference.com/w/cpp/numeric/math/modf
	const auto [value, expected_integral, expected_fraction] = GENERATE(
		(table<T, T, T>)({
			{T{123.45}, T{123}, T{.45}},  // NOLINT(clang-diagnostic-implicit-float-conversion)
			{-T{0}, -T{0}, -T{0}},
			{-std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity(), -T{0}},
			})
	);

	const auto [integral, fraction] = math::modf(value);

	REQUIRE(integral == expected_integral);  // NOLINT(clang-diagnostic-float-equal)
	REQUIRE(fraction == Catch::Approx{expected_fraction});
}
