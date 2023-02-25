//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "../helper.hpp"

#include "Simple-Utility/functional/transform.hpp"

#include <optional>

using namespace sl::functional;

namespace
{
	constexpr auto add42 = [](auto x) { return x + 42; };
	constexpr auto times3 = [](auto x) { return x * 3; };

	constexpr auto variadicAdd = [](std::integral auto... args) { return (args + ... + 0); };
}

TEST_CASE("transform_fn is invocable.", "[functional][transform]")
{
	transform_fn transform{ times3 };

	const auto& refMod = GENERATE(make_all_ref_mods_generator());
	REQUIRE(std::invoke(cast(transform, refMod), 42) == 126);
}

TEST_CASE("transform_fn is pipe composable with operator | as left-hand-side", "[functional][transform]")
{
	auto transform_raw = times3;
	transform_fn transform{ add42 };

	const auto& lhsRefMod = GENERATE(make_all_ref_mods_generator());
	const auto& rhsRefMod = GENERATE(make_all_ref_mods_generator());
	const transform_fn composedTransform = cast(transform, lhsRefMod) | cast(transform_raw, rhsRefMod);

	REQUIRE(composedTransform(1337) == 4137);
}

TEST_CASE("transform_fn is pipe composable with operator | as right-hand-side", "[functional][transform]")
{
	auto transform_raw = times3;
	transform_fn transform{ add42 };

	const auto& lhsRefMod = GENERATE(make_all_ref_mods_generator());
	const auto& rhsRefMod = GENERATE(make_all_ref_mods_generator());
	const transform_fn composedTransform = cast(transform_raw, lhsRefMod) | cast(transform, rhsRefMod);

	REQUIRE(composedTransform(1337) == 4053);
}

TEST_CASE("transform_fn is pipe composable as both sides of operator |.", "[functional][transform]")
{
	transform_fn transformLhs{ times3 };
	transform_fn transformRhs{ add42 };

	const auto& lhsRefMod = GENERATE(make_all_ref_mods_generator());
	const auto& rhsRefMod = GENERATE(make_all_ref_mods_generator());
	const transform_fn composedTransform = cast(transformLhs, lhsRefMod) | cast(transformRhs, rhsRefMod);

	REQUIRE(composedTransform(1337) == 4053);
}

TEST_CASE("transform_fn can be chained in arbitrary length.", "[functional][transform]")
{
	const transform_fn composedTransform = transform_fn{ add42 }
											| transform_fn{ add42 }
											| add42;

	REQUIRE(composedTransform(1) == 127);
}

TEST_CASE("transform_fn can be used to project to different types.", "[functional][transform][example]")
{
	//! [functional piped]
	transform_fn comp = transform_fn{ add42 }
						| [](const int x) { return std::to_string(x); };

	REQUIRE(comp(1) == "43");
	//! [functional piped]
}

TEST_CASE("more complex composition_fn is evaluated in deterministic manner.", "[functional][transform]")
{
	transform_fn comp = (transform_fn{ add42 } | times3)
						| (transform_fn{ [](const int x) { return x - 5; } } | times3);

	REQUIRE(comp(3) == 390);
}

TEMPLATE_TEST_CASE_SIG(
	"transform_fn can be front curried with operator << from in arbitrary length",
	"[functional][transform]",
	((int VExpected, int... VValues), VExpected, VValues...),
	(42, 42),
	(1379, 42, 1337),
	(1377, 42, 1337, -2)
)
{
	transform_fn transform{ variadicAdd };

	const auto& refMod = GENERATE(make_all_ref_mods_generator());
	const transform_fn curriedTransform = (cast(transform, refMod) << ... << VValues);

	REQUIRE(curriedTransform() == VExpected);
}

TEST_CASE("front curried arguments to transform_fn will be applied in correct order.", "[functional][transform]")
{
	const transform_fn transform = transform_fn{
										[](const int i, const std::string& str, const int mod) { return (i + std::stoi(str)) * mod; }
									} << 42 << "1337";

	REQUIRE(transform(2) == 2*1379);
}

TEST_CASE("front curried transform_fn can be piped", "[functional][transform]")
{
	transform_fn transform = transform_fn{ variadicAdd } << 42 << -2
							| transform_fn{ std::multiplies{} } << 2;

	REQUIRE(transform(7, 3) == 100);
}

TEMPLATE_TEST_CASE_SIG(
	"transform_fn can be back curried with operator >> in arbitrary length",
	"[functional][transform]",
	((int VExpected, int... VValues), VExpected, VValues...),
	(42, 42),
	(1379, 42, 1337),
	(1377, 42, 1337, -2)
)
{
	transform_fn transform{ variadicAdd };

	const auto& refMod = GENERATE(make_all_ref_mods_generator());
	const transform_fn curriedTransform = (cast(transform, refMod) >> ... >> VValues);

	REQUIRE(curriedTransform() == VExpected);
}

TEST_CASE("back curried arguments to transform_fn will be applied in correct order.", "[functional][transform]")
{
	const transform_fn transform = transform_fn{
										[](const int i, const std::string& str, const int mod) { return (i + std::stoi(str)) * mod; }
									} >> "1337" >> 2;

	REQUIRE(transform(42) == 2*1379);
}

TEST_CASE("back curried transform_fn can be piped", "[functional][transform]")
{
	transform_fn transform = transform_fn{ variadicAdd } >> 42 >> -2
							| transform_fn{ std::multiplies{} } >> 2;

	REQUIRE(transform(7, 3) == 100);
}

template <class T>
struct explicitly_constructible
{
	T t{};

	explicit explicitly_constructible(T t)
		: t{ t }
	{}

	[[nodiscard]]
	bool operator ==(T other) const
	{
		return t == other;
	}
};

TEMPLATE_TEST_CASE_SIG(
	"as casts the given parameter when invoked.",
	"[functional][transform]",
	((class TTarget, auto VSource, auto VExpected), TTarget, VSource, VExpected),
	(int, 42ul, 42),
	(char, 42, '*'),
	(std::optional<int>, 3, 3),
	(explicitly_constructible<int>, 1337, 1337)
)
{
	STATIC_REQUIRE(std::same_as<TTarget, decltype(as<TTarget>(VSource))>);
	REQUIRE(as<TTarget>(VSource) == VExpected);
}

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

	REQUIRE(plus(lhs, rhs) == expectedResult);
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

	REQUIRE(minus(lhs, rhs) == expectedResult);
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

	REQUIRE(multiplies(lhs, rhs) == expectedResult);
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

	REQUIRE(divides(lhs, rhs) == expectedResult);
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

	REQUIRE(modulus(lhs, rhs) == expectedResult);
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

	REQUIRE(negate(value) == expectedResult);
}
