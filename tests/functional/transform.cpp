//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>

#include "../helper.hpp"

#include "Simple-Utility/functional/transform.hpp"

#include <optional>

using namespace sl::functional;

namespace
{
	constexpr auto add42 = [](auto x) { return x + 42; };
	constexpr auto times3 = [](auto x) { return x * 3; };

	constexpr auto variadicAdd = [](std::integral auto ... args) { return (args + ... + 0); };
}

TEST_CASE("transform_fn invokes internal function on invocation.", "[functional][transform]")
{
	transform_fn proj{ add42 };

	REQUIRE(proj(1) == 43);
}

TEST_CASE("transform_fn can be used on the right side of operator | expressions.", "[functional][transform]")
{
	transform_fn comp = times3
						| transform_fn{ add42 };

	REQUIRE(comp(1) == 45);
}

TEST_CASE("transform_fn can be used on the left side of operator | expressions.", "[functional][transform]")
{
	transform_fn comp = transform_fn{ add42 }
						| times3;

	REQUIRE(comp(1) == 129);
}

TEST_CASE("transform_fn can be chained in arbitrary length.", "[functional][transform]")
{
	transform_fn comp = transform_fn{ add42 }
						| transform_fn{ add42 }
						| transform_fn{ add42 };

	REQUIRE(comp(1) == 127);
}

TEST_CASE("transform_fn can be used to project to different types.", "[functional][transform]")
{
	//! [functional piped]
	transform_fn comp = transform_fn{ add42 }
						| [](const int x) { return std::to_string(x); };

	REQUIRE(comp(1) == "43");
	//! [functional piped]
}

TEST_CASE("more complex composition_fn is evaluated in deterministic manner.", "[functional][transform]")
{
	transform_fn comp = (
							transform_fn{ add42 }
							| times3
						)
						| (
							transform_fn{ [](int x) { return x - 5; } }
							| times3
						);

	REQUIRE(comp(3) == 390);
}

TEST_CASE("front curried arguments to transform_fn will be applied in correct order.", "[functional][transform]")
{
	const transform_fn transform = transform_fn{
										[](const int i, const std::string& str, const int mod) { return (i + std::stoi(str)) * mod; }
									} << 42 << "1337";

	REQUIRE(transform(2) == 2*1379);
}

TEMPLATE_TEST_CASE_SIG(
	"transform_fn can be front curried with operator << from in arbitrary length",
	"[functional][transform]",
	((template <class> class TMod, int VExpected, int... VValues), TMod, VExpected, VValues...),
	(as_lvalue_ref_t, 42, 42),
	(as_lvalue_ref_t, 1379, 42, 1337),
	(as_lvalue_ref_t, 1377, 42, 1337, -2),
	(as_const_lvalue_ref_t, 42, 42),
	(as_const_lvalue_ref_t, 1379, 42, 1337),
	(as_const_lvalue_ref_t, 1377, 42, 1337, -2),
	(as_rvalue_ref_t, 42, 42),
	(as_rvalue_ref_t, 1379, 42, 1337),
	(as_rvalue_ref_t, 1377, 42, 1337, -2)
)
{
	transform_fn transform{ variadicAdd };
	const transform_fn curriedTransform = (apply_mod<TMod>(transform) << ... << VValues);

	REQUIRE(curriedTransform() == VExpected);
}

TEST_CASE("front curried transform_fn can be piped", "[functional][transform]")
{
	transform_fn transform = transform_fn{ variadicAdd } << 42 << -2
							| transform_fn{ std::multiplies{} } << 2;

	REQUIRE(transform(7, 3) == 100);
}

TEST_CASE("back curried arguments to transform_fn will be applied in correct order.", "[functional][transform]")
{
	const transform_fn transform = transform_fn{
										[](const int i, const std::string& str, const int mod) { return (i + std::stoi(str)) * mod; }
									} >> "1337" >> 2;

	REQUIRE(transform(42) == 2*1379);
}

TEMPLATE_TEST_CASE_SIG(
	"transform_fn can be back curried with operator >> in arbitrary length",
	"[functional][transform]",
	((template <class> class TMod, int VExpected, int... VValues), TMod, VExpected, VValues...),
	(as_lvalue_ref_t, 42, 42),
	(as_lvalue_ref_t, 1379, 42, 1337),
	(as_lvalue_ref_t, 1377, 42, 1337, -2),
	(as_const_lvalue_ref_t, 42, 42),
	(as_const_lvalue_ref_t, 1379, 42, 1337),
	(as_const_lvalue_ref_t, 1377, 42, 1337, -2),
	(as_rvalue_ref_t, 42, 42),
	(as_rvalue_ref_t, 1379, 42, 1337),
	(as_rvalue_ref_t, 1377, 42, 1337, -2)
)
{
	transform_fn transform{ variadicAdd };
	const transform_fn curriedTransform = (apply_mod<TMod>(transform) >> ... >> VValues);

	REQUIRE(curriedTransform() == VExpected);
}

TEST_CASE("back curried transform_fn can be piped", "[functional][transform]")
{
	transform_fn transform = transform_fn{ variadicAdd } >> 42 >> -2
							| transform_fn{ std::multiplies{} } >> 2;

	REQUIRE(transform(7, 3) == 100);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG(
	"as casts the given parameter when invoked.",
	"[functional]",
	((class TTarget, auto VSource, auto VExpected), TTarget, VSource, VExpected),
	(int, 42ul, 42),
	(char, 42, '*'),
	(std::optional<int>, 3, 3)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(std::same_as<TTarget, decltype(as<TTarget>(VSource))>);
	REQUIRE(as<TTarget>(VSource) == VExpected);
}
