//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/catch_approx.hpp>

#include "Simple-Utility/math.hpp"

using namespace sl;

TEMPLATE_TEST_CASE(
	"remquo forwards the result from std::remquo",
	"[math]",
	float,
	double,
	long double,
	std::int8_t,
	std::int16_t,
	std::int32_t,
	std::int64_t
)
{
	using T = TestType;

	const T x = GENERATE(T{0}, -T{0}, T{42}, std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity());
	const T y = GENERATE(T{42}, std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity());
	const auto [expected_remainder, expected_quo] = [&]
	{
		int quo{};
		auto rem = std::remquo(x, y, &quo);
		return std::tuple{ rem, quo };
	}();

	const auto [remainder, quo] = math::remquo(x, y);

	REQUIRE((std::isnan(remainder) && std::isnan(expected_remainder) || remainder == expected_remainder));
	REQUIRE(quo == expected_quo);
}

TEMPLATE_TEST_CASE(
	"frexp forwards the result from std::frexp",
	"[math]",
	float,
	double,
	long double,
	std::int8_t,
	std::int16_t,
	std::int32_t,
	std::int64_t
)
{
	using T = TestType;

	SECTION("value is +- 0")
	{
		const T value = GENERATE(T{0}, -T{0});

		const auto [fraction, exp] = math::frexp(value);

		REQUIRE(fraction == value); // NOLINT(clang-diagnostic-implicit-int-float-conversion)
		REQUIRE(exp == 0);
	}

	if constexpr (std::floating_point<T>)
	{
		SECTION("value is +- infinity")
		{
			const T value = GENERATE(std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity());

			const auto [fraction, exp] = math::frexp(value);

			REQUIRE(fraction == value); // NOLINT(clang-diagnostic-float-equal)
			// exp is undefined
		}

		SECTION("value is NaN")
		{
			const auto [fraction, exp] = math::frexp(std::numeric_limits<T>::quiet_NaN());

			REQUIRE(std::isnan(fraction));
			// exp is undefined
		}
	}

	SECTION("value is valid")
	{
		const auto [fraction, exp] = math::frexp(T{ 123 });

		REQUIRE(fraction == Catch::Approx{0.9609375});
		REQUIRE(exp == 7);
	}
}

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
			{T{123.45}, T{123}, T{.45}}, // NOLINT(clang-diagnostic-implicit-float-conversion)
			{-T{0}, -T{0}, -T{0}},
			{-std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity(), -T{0}},
			})
	);

	const auto [integral, fraction] = math::modf(value);

	REQUIRE(integral == expected_integral); // NOLINT(clang-diagnostic-float-equal)
	REQUIRE(fraction == Catch::Approx{expected_fraction});
}
