//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "Simple-Utility/functional/predicate.hpp"

using namespace sl;

constexpr auto trueFunc = [](auto&&...) { return true; };
constexpr auto falseFunc = [](auto&&...) { return false; };

TEST_CASE("predicate_fn is composable with operator && as left-hand-side", "[functional][predicate]")
{
	const auto& [chainedFunction, expectedResult] = GENERATE(table<bool(*)(), bool>({ { trueFunc, true }, { falseFunc, false } }));

	const functional::predicate_fn composedPredicate = functional::predicate_fn{ trueFunc } && chainedFunction;

	REQUIRE(composedPredicate() == expectedResult);
}

TEST_CASE("predicate_fn is composable with operator && as right-hand-side", "[functional][predicate]")
{
	const auto& [chainedFunction, expectedResult] = GENERATE(table<bool(*)(), bool>({ { trueFunc, true }, { falseFunc, false } }));

	const functional::predicate_fn composedPredicate = chainedFunction && functional::predicate_fn{ trueFunc };

	REQUIRE(composedPredicate() == expectedResult);
}

TEST_CASE("predicate_fn is composable with operator || as left-hand-side", "[functional][predicate]")
{
	const auto& [chainedFunction, expectedResult] = GENERATE(table<bool(*)(), bool>({ { trueFunc, true }, { falseFunc, false } }));

	const functional::predicate_fn composedPredicate = functional::predicate_fn{ falseFunc } || chainedFunction;

	REQUIRE(composedPredicate() == expectedResult);
}

TEST_CASE("predicate_fn is composable with operator || as right-hand-side", "[functional][predicate]")
{
	const auto& [chainedFunction, expectedResult] = GENERATE(table<bool(*)(), bool>({ { trueFunc, true }, { falseFunc, false } }));

	const functional::predicate_fn composedPredicate = chainedFunction || functional::predicate_fn{ falseFunc };

	REQUIRE(composedPredicate() == expectedResult);
}

TEST_CASE("predicate_fn follows the common operator hierarchy.", "[functional][predicate]")
{
	const auto& [andFunc, orFunc, expectedResult] = GENERATE(
		table<bool(*)(),
		bool(*)(),
		bool>({
			{ falseFunc, falseFunc, false },
			{ falseFunc, trueFunc, true },
			{ trueFunc, falseFunc, true },
			{ trueFunc, trueFunc, true },
		})
	);

	const functional::predicate_fn composedPredicate = functional::predicate_fn{ trueFunc } && andFunc || orFunc;

	REQUIRE(composedPredicate() == expectedResult);
}

TEST_CASE("predicate_fn common operator hierarchy can be adjusted with ().", "[functional][predicate]")
{
	const auto& [andFunc, orFunc, expectedResult] = GENERATE(
		table<bool(*)(),
		bool(*)(),
		bool>({
			{ falseFunc, falseFunc, false },
			{ falseFunc, trueFunc, false },
			{ trueFunc, falseFunc, false },
			{ trueFunc, trueFunc, true },
		})
	);

	const functional::predicate_fn composedPredicate = (functional::predicate_fn{ falseFunc } || orFunc) && andFunc;

	REQUIRE(composedPredicate() == expectedResult);
}
