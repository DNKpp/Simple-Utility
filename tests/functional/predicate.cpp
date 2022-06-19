//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "Simple-Utility/functional/predicate.hpp"

#include <ranges>
#include <vector>

using namespace sl;

inline constexpr auto trueFunc = [](auto&&...) { return true; };
inline constexpr auto falseFunc = [](auto&&...) { return false; };

using empty_predicate_t = bool(*)();

inline constexpr functional::predicate_fn<empty_predicate_t> truePredicate{ trueFunc };
inline constexpr functional::predicate_fn<empty_predicate_t> falsePredicate{ falseFunc };

using predicate_reference_list_t = std::tuple<functional::predicate_fn<empty_predicate_t>&,
											const functional::predicate_fn<empty_predicate_t>&,
											functional::predicate_fn<empty_predicate_t>&&>;

TEST_CASE("predicate_fn is constructed from predicate.", "[functional][predicate]")
{
	functional::predicate_fn<empty_predicate_t> predicate{ trueFunc };

	REQUIRE(predicate());
}

TEMPLATE_LIST_TEST_CASE(
	"predicate_fn is evaluated via operator ()",
	"[functional][predicate]",
	predicate_reference_list_t
)
{
	auto [sourcePredicate, expectedResult] = GENERATE(
		table<empty_predicate_t,
		bool>({
			{ trueFunc, true },
			{ falseFunc, false }
			})
	);

	functional::predicate_fn predicate{ sourcePredicate };

	REQUIRE(static_cast<TestType>(predicate)() == expectedResult);
}

TEMPLATE_LIST_TEST_CASE(
	"predicate_fn is composable with operator && as left-hand-side",
	"[functional][predicate]",
	predicate_reference_list_t
)
{
	const auto& [chainedFunction, expectedResult] = GENERATE(
		table<empty_predicate_t,
		bool>({
			{ trueFunc, true },
			{ falseFunc, false }
			})
	);

	functional::predicate_fn predicate{ truePredicate };
	const functional::predicate_fn composedPredicate = static_cast<TestType>(predicate) && chainedFunction;

	REQUIRE(composedPredicate() == expectedResult);
}

TEMPLATE_LIST_TEST_CASE(
	"predicate_fn is composable with operator && as right-hand-side",
	"[functional][predicate]",
	predicate_reference_list_t
)
{
	const auto& [chainedFunction, expectedResult] = GENERATE(
		table<empty_predicate_t,
		bool>({
			{ trueFunc, true },
			{ falseFunc, false }
			})
	);

	functional::predicate_fn predicate{ truePredicate };
	const functional::predicate_fn composedPredicate = chainedFunction && static_cast<TestType>(predicate);

	REQUIRE(composedPredicate() == expectedResult);
}

TEMPLATE_TEST_CASE_SIG(
	"predicate_fn is composable as param on both sides of operator &&.",
	"[functional][predicate]",
	((bool VDummy, class TLhs, class TRhs), VDummy, TLhs, TRhs),
	(true, functional::predicate_fn<empty_predicate_t>&, functional::predicate_fn<empty_predicate_t>&),
	(true, functional::predicate_fn<empty_predicate_t>&, const functional::predicate_fn<empty_predicate_t>&),
	(true, functional::predicate_fn<empty_predicate_t>&, functional::predicate_fn<empty_predicate_t>&&),
	(true, const functional::predicate_fn<empty_predicate_t>&, functional::predicate_fn<empty_predicate_t>&),
	(true, const functional::predicate_fn<empty_predicate_t>&, const functional::predicate_fn<empty_predicate_t>&),
	(true, const functional::predicate_fn<empty_predicate_t>&, functional::predicate_fn<empty_predicate_t>&&),
	(true, functional::predicate_fn<empty_predicate_t>&&, functional::predicate_fn<empty_predicate_t>&),
	(true, functional::predicate_fn<empty_predicate_t>&&, const functional::predicate_fn<empty_predicate_t>&),
	(true, functional::predicate_fn<empty_predicate_t>&&, functional::predicate_fn<empty_predicate_t>&&)
)
{
	const auto& [lhsPredicate, rhsPredicate, expectedResult] = GENERATE(
		table<empty_predicate_t,
		empty_predicate_t,
		bool>({
			{ falseFunc, falseFunc, false },
			{ trueFunc, falseFunc, false },
			{ falseFunc, trueFunc, false },
			{ trueFunc, trueFunc, true }
			})
	);
	functional::predicate_fn lhs{ lhsPredicate };
	functional::predicate_fn rhs{ rhsPredicate };

	const functional::predicate_fn composedPredicate = static_cast<TLhs>(lhs) && static_cast<TRhs>(rhs);

	REQUIRE(composedPredicate() == expectedResult);
}

TEMPLATE_LIST_TEST_CASE(
	"predicate_fn is composable with operator || as left-hand-side",
	"[functional][predicate]",
	predicate_reference_list_t
)
{
	const auto& [chainedFunction, expectedResult] = GENERATE(
		table<empty_predicate_t,
		bool>({
			{ trueFunc, true },
			{ falseFunc, false }
			})
	);

	functional::predicate_fn predicate{ falsePredicate };
	const functional::predicate_fn composedPredicate = static_cast<TestType>(predicate) || chainedFunction;

	REQUIRE(composedPredicate() == expectedResult);
}

TEMPLATE_LIST_TEST_CASE(
	"predicate_fn is composable with operator || as right-hand-side",
	"[functional][predicate]",
	predicate_reference_list_t
)
{
	const auto& [chainedFunction, expectedResult] = GENERATE(
		table<empty_predicate_t,
		bool>({
			{ trueFunc, true },
			{ falseFunc, false }
			})
	);

	functional::predicate_fn predicate{ falsePredicate };
	const functional::predicate_fn composedPredicate = chainedFunction || static_cast<TestType>(predicate);

	REQUIRE(composedPredicate() == expectedResult);
}

TEMPLATE_TEST_CASE_SIG(
	"predicate_fn is composable as param on both sides of operator ||.",
	"[functional][predicate]",
	((bool VDummy, class TLhs, class TRhs), VDummy, TLhs, TRhs),
	(true, functional::predicate_fn<empty_predicate_t>&, functional::predicate_fn<empty_predicate_t>&),
	(true, functional::predicate_fn<empty_predicate_t>&, const functional::predicate_fn<empty_predicate_t>&),
	(true, functional::predicate_fn<empty_predicate_t>&, functional::predicate_fn<empty_predicate_t>&&),
	(true, const functional::predicate_fn<empty_predicate_t>&, functional::predicate_fn<empty_predicate_t>&),
	(true, const functional::predicate_fn<empty_predicate_t>&, const functional::predicate_fn<empty_predicate_t>&),
	(true, const functional::predicate_fn<empty_predicate_t>&, functional::predicate_fn<empty_predicate_t>&&),
	(true, functional::predicate_fn<empty_predicate_t>&&, functional::predicate_fn<empty_predicate_t>&),
	(true, functional::predicate_fn<empty_predicate_t>&&, const functional::predicate_fn<empty_predicate_t>&),
	(true, functional::predicate_fn<empty_predicate_t>&&, functional::predicate_fn<empty_predicate_t>&&)
)
{
	const auto& [lhsPredicate, rhsPredicate, expectedResult] = GENERATE(
		table<empty_predicate_t,
		empty_predicate_t,
		bool>({
			{ falseFunc, falseFunc, false },
			{ trueFunc, falseFunc, true },
			{ falseFunc, trueFunc, true },
			{ trueFunc, trueFunc, true }
			})
	);
	functional::predicate_fn lhs{ lhsPredicate };
	functional::predicate_fn rhs{ rhsPredicate };

	const functional::predicate_fn composedPredicate = static_cast<TLhs>(lhs) || static_cast<TRhs>(rhs);

	REQUIRE(composedPredicate() == expectedResult);
}

TEST_CASE("predicate_fn follows the common operator hierarchy.", "[functional][predicate]")
{
	const auto& [andFunc, orFunc, expectedResult] = GENERATE(
		table<empty_predicate_t,
		empty_predicate_t,
		bool>({
			{ falseFunc, falseFunc, false },
			{ falseFunc, trueFunc, true },
			{ trueFunc, falseFunc, true },
			{ trueFunc, trueFunc, true },
			})
	);

	const functional::predicate_fn composedPredicate = truePredicate && andFunc || orFunc;

	REQUIRE(composedPredicate() == expectedResult);
}

TEST_CASE("predicate_fn common operator hierarchy can be adjusted with ().", "[functional][predicate]")
{
	const auto& [andFunc, orFunc, expectedResult] = GENERATE(
		table<empty_predicate_t,
		empty_predicate_t,
		bool>({
			{ falseFunc, falseFunc, false },
			{ falseFunc, trueFunc, false },
			{ trueFunc, falseFunc, false },
			{ trueFunc, trueFunc, true },
			})
	);

	const functional::predicate_fn composedPredicate = (falsePredicate || orFunc) && andFunc;

	REQUIRE(composedPredicate() == expectedResult);
}

TEMPLATE_LIST_TEST_CASE(
	"predicate_fn can be negated via operator !",
	"[functional][predicate]",
	predicate_reference_list_t
)
{
	const auto& [basePredicate, expectedResult] = GENERATE(
		table<empty_predicate_t,
		bool>({
			{ trueFunc, false },
			{ falseFunc, true }
			})
	);

	functional::predicate_fn predicate{ basePredicate };
	const functional::predicate_fn negatedPredicate = !static_cast<TestType>(predicate);

	REQUIRE(negatedPredicate() == expectedResult);
}

TEST_CASE("predicate_fn is composable via operator |", "[functional][predicate]")
{
	const auto& [input, expectedResult] = GENERATE(
		table<std::string,
		bool>({
			{ "9", true },
			{ "0", false },
			{ "10", false }
			})
	);

	const functional::predicate_fn composedPredicate = functional::predicate_fn{ [](const int v) { return std::cmp_less(v, 10); } }
														&& [](const int v) { return std::cmp_less(0, v); };
	const functional::predicate_fn finalPredicate = [](const std::string& str) { return std::stoi(str); }
													| composedPredicate;

	REQUIRE(finalPredicate(input) == expectedResult);
}

TEST_CASE("predicate_fn accepts functional objects wrapped into a std::reference_wrapper", "[functional][predicate]")
{
	//! [predicate wrapped]
	const std::vector sourceInts{ 0, 1, 2, 3 };
	auto skipFirst = [i{ 0 }](auto&&...) mutable { return static_cast<bool>(i++); };

	const auto result = std::ranges::count_if(
		sourceInts,
		functional::predicate_fn{ std::ref(skipFirst) }
	);

	REQUIRE(result == 3);
	//! [predicate wrapped]
}

using binary_int_predicate = bool(*)(int, int);
using binary_int_predicate_reference_list_t = std::tuple<functional::binary_predicate_fn<binary_int_predicate>&,
														const functional::binary_predicate_fn<binary_int_predicate>&,
														functional::binary_predicate_fn<binary_int_predicate>&&>;

TEMPLATE_LIST_TEST_CASE(
	"binary_predicate_fn accepts currying of the first parameter.",
	"[functional][predicate]",
	binary_int_predicate_reference_list_t
)
{
	functional::binary_predicate_fn<bool(*)(int, int)> compare{
		[](const int lhs, const int rhs) { return lhs < rhs; }
	};
	const functional::predicate_fn curried{ static_cast<TestType>(compare).bind_first(42) };

	REQUIRE(curried(43));
	REQUIRE(!curried(42));
}

TEMPLATE_LIST_TEST_CASE(
	"binary_predicate_fn accepts currying of the second parameter.",
	"[functional][predicate]",
	binary_int_predicate_reference_list_t
)
{
	functional::binary_predicate_fn<bool(*)(int, int)> compare{
		[](const int lhs, const int rhs) { return lhs < rhs; }
	};
	const functional::predicate_fn curried{ static_cast<TestType>(compare).bind_second(42) };

	REQUIRE(curried(41));
	REQUIRE(!curried(42));
}

TEST_CASE("binary_predicate_fn can be used with stl algorithms.", "[functional][predicate]")
{
	const std::vector sourceInts{ 0, 1, 2, 3 };

	const auto result = std::ranges::count_if(
		sourceInts,
		functional::greater.bind_second(1)
	);

	REQUIRE(result == 2);
}

TEMPLATE_TEST_CASE("less compares its two parameters","[functional][predicate]", int, float)
{
	const auto& [value1, value2, expectedResult] = GENERATE(
		table<int,
		int,
		bool>({
			{ 42, 43, true },
			{ 42, 42, false }
			})
	);

	const bool result = functional::less(static_cast<TestType>(value1), static_cast<TestType>(value2));

	REQUIRE(result == expectedResult);
}

TEMPLATE_TEST_CASE("greater compares its two parameters","[functional][predicate]", int, float)
{
	const auto& [value1, value2, expectedResult] = GENERATE(
		table<int,
		int,
		bool>({
			{ 43, 42, true },
			{ 42, 42, false }
			})
	);

	const bool result = functional::greater(static_cast<TestType>(value1), static_cast<TestType>(value2));

	REQUIRE(result == expectedResult);
}

TEMPLATE_TEST_CASE("less_equal compares its two parameters","[functional][predicate]", int, float)
{
	const auto& [value1, value2, expectedResult] = GENERATE(
		table<int,
		int,
		bool>({
			{ 41, 42, true },
			{ 42, 42, true },
			{ 43, 42, false }
			})
	);

	const bool result = functional::less_equal(static_cast<TestType>(value1), static_cast<TestType>(value2));

	REQUIRE(result == expectedResult);
}

TEMPLATE_TEST_CASE("greater_equal compares its two parameters","[functional][predicate]", int, float)
{
	const auto& [value1, value2, expectedResult] = GENERATE(
		table<int,
		int,
		bool>({
			{ 43, 42, true },
			{ 42, 42, true },
			{ 41, 42, false }
			})
	);

	const bool result = functional::greater_equal(static_cast<TestType>(value1), static_cast<TestType>(value2));

	REQUIRE(result == expectedResult);
}

TEMPLATE_TEST_CASE("equal compares its two parameters","[functional][predicate]", int, float)
{
	const auto& [value1, value2, expectedResult] = GENERATE(
		table<int,
		int,
		bool>({
			{ 42, 42, true },
			{ 43, 42, false }
			})
	);

	const bool result = functional::equal(static_cast<TestType>(value1), static_cast<TestType>(value2));

	REQUIRE(result == expectedResult);
}

TEMPLATE_TEST_CASE("not_equal compares its two parameters","[functional][predicate]", int, float)
{
	const auto& [value1, value2, expectedResult] = GENERATE(
		table<int,
		int,
		bool>({
			{ 43, 42, true },
			{ 42, 42, false }
			})
	);

	const bool result = functional::not_equal(static_cast<TestType>(value1), static_cast<TestType>(value2));

	REQUIRE(result == expectedResult);
}
