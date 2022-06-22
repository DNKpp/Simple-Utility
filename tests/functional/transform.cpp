//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>

#include "Simple-Utility/functional/transform.hpp"

#include <optional>

using namespace sl;

namespace
{
	constexpr auto add42 = [](auto x) { return x + 42; };
	constexpr auto times3 = [](auto x) { return x * 3; };
}

TEST_CASE("transform_fn invokes internal function on invocation.", "[functional][transform]")
{
	functional::transform_fn proj{ add42 };

	REQUIRE(std::invoke(proj, 1) == 43);
}

TEST_CASE("transform_fn can be used on the right side of operator | expressions.", "[functional][transform]")
{
	functional::transform_fn comp = times3
									| functional::transform_fn{ add42 };

	REQUIRE(std::invoke(comp, 1) == 45);
}

TEST_CASE("transform_fn can be used on the left side of operator | expressions.", "[functional][transform]")
{
	functional::transform_fn comp = functional::transform_fn{ add42 }
									| times3;

	REQUIRE(std::invoke(comp, 1) == 129);
}

TEST_CASE("transform_fn can be chained in arbitrary length.", "[functional][transform]")
{
	functional::transform_fn comp = functional::transform_fn{ add42 }
									| functional::transform_fn{ add42 }
									| functional::transform_fn{ add42 };

	REQUIRE(std::invoke(comp, 1) == 127);
}

TEST_CASE("transform_fn can be used to project to different types.", "[functional][transform]")
{
	//! [functional piped]
	functional::transform_fn comp = functional::transform_fn{ add42 }
									| [](const int x) { return std::to_string(x); };

	REQUIRE(comp(1) == "43");
	//! [functional piped]
}

TEST_CASE("more complex composition_fn is evaluated in deterministic manner.", "[functional][transform]")
{
	functional::transform_fn comp = (functional::transform_fn{ add42 }
									| times3)
									|
									(functional::transform_fn{ [](int x) { return x - 5; } }
									| times3);

	REQUIRE(std::invoke(comp, 3) == 390);
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
	STATIC_REQUIRE(std::same_as<TTarget, decltype(functional::as<TTarget>(VSource))>);
	REQUIRE(functional::as<TTarget>(VSource) == VExpected);
}
