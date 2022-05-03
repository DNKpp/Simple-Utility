//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch.hpp>

#include "Simple-Utility/functional.hpp"

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

TEST_CASE("composition_fn invokes internal func1 and supplies func2 with the result.", "[functional][transform]")
{
	functional::detail::composition_fn comp{ add42, times3 };

	REQUIRE(std::invoke(comp, 1) == 129);
}

TEST_CASE("composition_fn can be nested.", "[functional][transform]")
{
	functional::detail::composition_fn comp{
		functional::detail::composition_fn{ add42, times3 },
		[](int x) { return x - 5; }
	};

	REQUIRE(std::invoke(comp, 1) == 124);
}

TEST_CASE("transform_fn can be used on the right side of operator | expressions.", "[functional][transform]")
{
	functional::detail::composition_fn comp = times3
											| functional::transform_fn{ add42 };

	REQUIRE(std::invoke(comp, 1) == 45);
}

TEST_CASE("transform_fn can be used on the left side of operator | expressions.", "[functional][transform]")
{
	functional::detail::composition_fn comp = functional::transform_fn{ add42 }
											| times3;

	REQUIRE(std::invoke(comp, 1) == 129);
}

TEST_CASE("transform_fn can be chained in arbitrary length.", "[functional][transform]")
{
	functional::detail::composition_fn comp = functional::transform_fn{ add42 }
											| functional::transform_fn{ add42 }
											| functional::transform_fn{ add42 };

	REQUIRE(std::invoke(comp, 1) == 127);
}

TEST_CASE("transform_fn can be used to project to different types.", "[functional][transform]")
{
	functional::detail::composition_fn comp = functional::transform_fn{ add42 }
											| [](int x) { return std::to_string(x); };

	REQUIRE(std::invoke(comp, 1) == "43");
}

TEST_CASE("more complex composition_fn is evaluated in deterministic manner.", "[functional][transform]")
{
	auto comp = (functional::transform_fn{ add42 }
				| times3)
				|
				(functional::transform_fn{ [](int x) { return x - 5; } }
				| times3);

	REQUIRE(std::invoke(comp, 3) == 390);
}
