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

TEMPLATE_TEST_CASE_SIG(
	"transform_fn is invocable.",
	"[functional][transform]",
	((bool VDummy, template <class> class TMod), VDummy, TMod),
	(true, as_lvalue_ref_t),
	(true, as_const_lvalue_ref_t),
	(true, as_rvalue_ref_t)
)
{
	transform_fn transform{ times3 };

	REQUIRE(apply_mod<TMod>(transform)(42) == 126);
}

TEMPLATE_TEST_CASE_SIG(
	"transform_fn is pipe composable with operator | as left-hand-side",
	"[functional][transform]",
	((bool VDummy, template <class> class TLhsMod, template <class> class TRhsMod), VDummy, TLhsMod, TRhsMod),
	(true, as_lvalue_ref_t, as_lvalue_ref_t),
	(true, as_lvalue_ref_t, as_const_lvalue_ref_t),
	(true, as_lvalue_ref_t, as_rvalue_ref_t),
	(true, as_const_lvalue_ref_t, as_lvalue_ref_t),
	(true, as_const_lvalue_ref_t, as_const_lvalue_ref_t),
	(true, as_const_lvalue_ref_t, as_rvalue_ref_t),
	(true, as_rvalue_ref_t, as_lvalue_ref_t),
	(true, as_rvalue_ref_t, as_const_lvalue_ref_t),
	(true, as_rvalue_ref_t, as_rvalue_ref_t)
)
{
	auto transform_raw = times3;
	transform_fn transform{ add42 };

	const transform_fn composedTransform = apply_mod<TLhsMod>(transform) | apply_mod<TRhsMod>(transform_raw);

	REQUIRE(composedTransform(1337) == 4137);
}

TEMPLATE_TEST_CASE_SIG(
	"transform_fn is pipe composable with operator | as right-hand-side",
	"[functional][transform]",
	((bool VDummy, template <class> class TLhsMod, template <class> class TRhsMod), VDummy, TLhsMod, TRhsMod),
	(true, as_lvalue_ref_t, as_lvalue_ref_t),
	(true, as_lvalue_ref_t, as_const_lvalue_ref_t),
	(true, as_lvalue_ref_t, as_rvalue_ref_t),
	(true, as_const_lvalue_ref_t, as_lvalue_ref_t),
	(true, as_const_lvalue_ref_t, as_const_lvalue_ref_t),
	(true, as_const_lvalue_ref_t, as_rvalue_ref_t),
	(true, as_rvalue_ref_t, as_lvalue_ref_t),
	(true, as_rvalue_ref_t, as_const_lvalue_ref_t),
	(true, as_rvalue_ref_t, as_rvalue_ref_t)
)
{
	auto transform_raw = times3;
	transform_fn transform{ add42 };

	const transform_fn composedTransform = apply_mod<TLhsMod>(transform_raw) | apply_mod<TRhsMod>(transform);

	REQUIRE(composedTransform(1337) == 4053);
}

TEMPLATE_TEST_CASE_SIG(
	"transform_fn is pipe composable as both sides of operator |.",
	"[functional][transform]",
	((bool VDummy, template <class> class TLhsMod, template <class> class TRhsMod), VDummy, TLhsMod, TRhsMod),
	(true, as_lvalue_ref_t, as_lvalue_ref_t),
	(true, as_lvalue_ref_t, as_const_lvalue_ref_t),
	(true, as_lvalue_ref_t, as_rvalue_ref_t),
	(true, as_const_lvalue_ref_t, as_lvalue_ref_t),
	(true, as_const_lvalue_ref_t, as_const_lvalue_ref_t),
	(true, as_const_lvalue_ref_t, as_rvalue_ref_t),
	(true, as_rvalue_ref_t, as_lvalue_ref_t),
	(true, as_rvalue_ref_t, as_const_lvalue_ref_t),
	(true, as_rvalue_ref_t, as_rvalue_ref_t)
)
{
	transform_fn transformLhs{ times3 };
	transform_fn transformRhs{ add42 };

	const transform_fn composedTransform = apply_mod<TLhsMod>(transformLhs) | apply_mod<TRhsMod>(transformRhs);

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

TEMPLATE_TEST_CASE_SIG(
	"as casts the given parameter when invoked.",
	"[functional][transform]",
	((class TTarget, auto VSource, auto VExpected), TTarget, VSource, VExpected),
	(int, 42ul, 42),
	(char, 42, '*'),
	(std::optional<int>, 3, 3)
)
{
	STATIC_REQUIRE(std::same_as<TTarget, decltype(as<TTarget>(VSource))>);
	REQUIRE(as<TTarget>(VSource) == VExpected);
}
