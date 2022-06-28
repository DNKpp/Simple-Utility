//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "../helper.hpp"

#include "Simple-Utility/functional/predicate.hpp"

#include <algorithm>
#include <vector>
#include <optional>

using namespace sl::functional;

inline constexpr auto trueFunc = [](auto&&...) { return true; };
inline constexpr auto falseFunc = [](auto&&...) { return false; };

inline constexpr auto variadicAllEqual = [](auto&& v, auto&&... vs)
{
	return ((vs == v) && ... && true);
};

using empty_predicate_t = bool(*)();

inline constexpr predicate_fn<empty_predicate_t> truePredicate{ trueFunc };
inline constexpr predicate_fn<empty_predicate_t> falsePredicate{ falseFunc };

TEMPLATE_TEST_CASE_SIG(
	"predicate_fn is constructed from predicate.",
	"[functional][predicate]",
	((bool VDummy, template <class> class TMod), VDummy, TMod),
	(true, as_lvalue_ref_t),
	(true, as_const_lvalue_ref_t),
	(true, as_rvalue_ref_t)
)
{
	auto func{ trueFunc };

	predicate_fn<empty_predicate_t> predicate{ apply_mod<TMod>(func) };

	REQUIRE(predicate());
}

TEMPLATE_TEST_CASE_SIG(
	"predicate_fn is evaluated via operator ()",
	"[functional][predicate]",
	((bool VDummy, template <class> class TMod), VDummy, TMod),
	(true, as_lvalue_ref_t),
	(true, as_const_lvalue_ref_t),
	(true, as_rvalue_ref_t)
)
{
	auto [sourcePredicate, expectedResult] = GENERATE(
		table<empty_predicate_t,
		bool>({
			{ trueFunc, true },
			{ falseFunc, false }
			})
	);

	predicate_fn predicate{ sourcePredicate };

	REQUIRE(apply_mod<TMod>(predicate)() == expectedResult);
}

TEMPLATE_TEST_CASE_SIG(
	"predicate_fn is composable with operator && as left-hand-side",
	"[functional][predicate]",
	((bool VDummy, template <class> class TMod), VDummy, TMod),
	(true, as_lvalue_ref_t),
	(true, as_const_lvalue_ref_t),
	(true, as_rvalue_ref_t)
)
{
	const auto& [chainedFunction, expectedResult] = GENERATE(
		table<empty_predicate_t,
		bool>({
			{ trueFunc, true },
			{ falseFunc, false }
			})
	);

	predicate_fn predicate{ truePredicate };
	const predicate_fn composedPredicate = apply_mod<TMod>(predicate) && chainedFunction;

	REQUIRE(composedPredicate() == expectedResult);
}

TEMPLATE_TEST_CASE_SIG(
	"predicate_fn is composable with operator && as right-hand-side",
	"[functional][predicate]",
	((bool VDummy, template <class> class TMod), VDummy, TMod),
	(true, as_lvalue_ref_t),
	(true, as_const_lvalue_ref_t),
	(true, as_rvalue_ref_t)
)
{
	const auto& [chainedFunction, expectedResult] = GENERATE(
		table<empty_predicate_t,
		bool>({
			{ trueFunc, true },
			{ falseFunc, false }
			})
	);

	predicate_fn predicate{ truePredicate };
	const predicate_fn composedPredicate = chainedFunction && apply_mod<TMod>(predicate);

	REQUIRE(composedPredicate() == expectedResult);
}

TEMPLATE_TEST_CASE_SIG(
	"predicate_fn is composable as param on both sides of operator &&.",
	"[functional][predicate]",
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
	predicate_fn lhs{ lhsPredicate };
	predicate_fn rhs{ rhsPredicate };

	const predicate_fn composedPredicate = apply_mod<TLhsMod>(lhs) && apply_mod<TRhsMod>(rhs);

	REQUIRE(composedPredicate() == expectedResult);
}

TEST_CASE("operator && composed predicates get flattened", "[functional][predicate]")
{
	using result_t = std::remove_cvref_t<decltype(truePredicate && truePredicate && truePredicate)>;
	using expected_t = predicate_fn<composition_fn<
			operators::detail::conjunction_caller_fn, empty_predicate_t, empty_predicate_t, empty_predicate_t>
	>;

	STATIC_REQUIRE(std::same_as<result_t, expected_t>);
}

TEMPLATE_TEST_CASE_SIG(
	"predicate_fn is composable with operator || as left-hand-side",
	"[functional][predicate]",
	((bool VDummy, template <class> class TMod), VDummy, TMod),
	(true, as_lvalue_ref_t),
	(true, as_const_lvalue_ref_t),
	(true, as_rvalue_ref_t)
)
{
	const auto& [chainedFunction, expectedResult] = GENERATE(
		table<empty_predicate_t,
		bool>({
			{ trueFunc, true },
			{ falseFunc, false }
			})
	);

	predicate_fn predicate{ falsePredicate };
	const predicate_fn composedPredicate = apply_mod<TMod>(predicate) || chainedFunction;

	REQUIRE(composedPredicate() == expectedResult);
}

TEMPLATE_TEST_CASE_SIG(
	"predicate_fn is composable with operator || as right-hand-side",
	"[functional][predicate]",
	((bool VDummy, template <class> class TMod), VDummy, TMod),
	(true, as_lvalue_ref_t),
	(true, as_const_lvalue_ref_t),
	(true, as_rvalue_ref_t)
)
{
	const auto& [chainedFunction, expectedResult] = GENERATE(
		table<empty_predicate_t,
		bool>({
			{ trueFunc, true },
			{ falseFunc, false }
			})
	);

	predicate_fn predicate{ falsePredicate };
	const predicate_fn composedPredicate = chainedFunction || apply_mod<TMod>(predicate);

	REQUIRE(composedPredicate() == expectedResult);
}

TEMPLATE_TEST_CASE_SIG(
	"predicate_fn is composable as param on both sides of operator ||.",
	"[functional][predicate]",
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
	predicate_fn lhs{ lhsPredicate };
	predicate_fn rhs{ rhsPredicate };

	const predicate_fn composedPredicate = apply_mod<TLhsMod>(lhs) || apply_mod<TRhsMod>(rhs);

	REQUIRE(composedPredicate() == expectedResult);
}

TEST_CASE("operator || composed predicates get flattened", "[functional][predicate]")
{
	using result_t = std::remove_cvref_t<decltype(truePredicate || truePredicate || truePredicate)>;
	using expected_t = predicate_fn<composition_fn<
			operators::detail::disjunction_caller_fn, empty_predicate_t, empty_predicate_t, empty_predicate_t>
	>;

	STATIC_REQUIRE(std::same_as<result_t, expected_t>);
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

	const predicate_fn composedPredicate = truePredicate && andFunc || orFunc;

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

	const predicate_fn composedPredicate = (falsePredicate || orFunc) && andFunc;

	REQUIRE(composedPredicate() == expectedResult);
}

TEMPLATE_TEST_CASE_SIG(
	"predicate_fn can be negated via operator !",
	"[functional][predicate]",
	((bool VDummy, template <class> class TMod), VDummy, TMod),
	(true, as_lvalue_ref_t),
	(true, as_const_lvalue_ref_t),
	(true, as_rvalue_ref_t)
)
{
	const auto& [basePredicate, expectedResult] = GENERATE(
		table<empty_predicate_t,
		bool>({
			{ trueFunc, false },
			{ falseFunc, true }
			})
	);

	predicate_fn predicate{ basePredicate };
	const predicate_fn negatedPredicate = !apply_mod<TMod>(predicate);

	REQUIRE(negatedPredicate() == expectedResult);
}

TEST_CASE("predicate_fn can be negated in arbitrary depth", "[functional][predicate]")
{
	REQUIRE((!!truePredicate)());
	REQUIRE(!(!!!truePredicate)());
	REQUIRE((!!!!truePredicate)());
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

	const predicate_fn composedPredicate = predicate_fn{ [](const int v) { return std::cmp_less(v, 10); } }
											&& [](const int v) { return std::cmp_less(0, v); };
	const predicate_fn finalPredicate = [](const std::string& str) { return std::stoi(str); }
										| composedPredicate;

	REQUIRE(finalPredicate(input) == expectedResult);
}

TEST_CASE("front curried arguments to predicate_fn will be applied in correct order.", "[functional][predicate]")
{
	const predicate_fn predicate = predicate_fn{
										[](const int i, const std::string& str, const std::optional<int>& opt)
										{
											return i == std::stoi(str) && i != opt;
										}
									} << 42 << "42";

	REQUIRE(predicate(std::nullopt));
}

TEMPLATE_TEST_CASE_SIG(
	"predicate_fn can be front curried with operator << in arbitrary length",
	"[functional][predicate]",
	((template <class> class TMod, bool VExpected, int... VValues), TMod, VExpected, VValues...),
	(as_lvalue_ref_t, true, 42),
	(as_lvalue_ref_t, false, 42, 1337),
	(as_lvalue_ref_t, true, 42, 42, 42),
	(as_const_lvalue_ref_t, true, 42),
	(as_const_lvalue_ref_t, false, 42, 1337),
	(as_const_lvalue_ref_t, true, 42, 42, 42),
	(as_rvalue_ref_t, true, 42),
	(as_rvalue_ref_t, false, 42, 1337),
	(as_rvalue_ref_t, true, 42, 42, 42)
)
{
	predicate_fn predicate{ variadicAllEqual };
	const predicate_fn curriedPredicate = (apply_mod<TMod>(predicate) << ... << VValues);

	REQUIRE(curriedPredicate() == VExpected);
}

TEST_CASE("front curried predicate_fn can be piped", "[functional][predicate]")
{
	predicate_fn predicate = predicate_fn{ variadicAllEqual } << 42 << -2
							| predicate_fn{ std::equal_to{} } << false;

	REQUIRE(predicate());
}

TEST_CASE("back curried arguments to predicate_fn will be applied in correct order.", "[functional][predicate]")
{
	const predicate_fn predicate = predicate_fn{
										[](const int i, const std::string& str, const std::optional<int>& opt)
										{
											return i == std::stoi(str) && i != opt;
										}
									} >> "42" >> std::nullopt;

	REQUIRE(predicate(42));

	//predicate();
}

TEMPLATE_TEST_CASE_SIG(
	"predicate_fn can be back curried with operator >> from the right-hand-side in arbitrary length",
	"[functional][predicate]",
	((template <class> class TMod, bool VExpected, int... VValues), TMod, VExpected, VValues...),
	(as_lvalue_ref_t, true, 42),
	(as_lvalue_ref_t, false, 42, 1337),
	(as_lvalue_ref_t, true, 42, 42, 42),
	(as_const_lvalue_ref_t, true, 42),
	(as_const_lvalue_ref_t, false, 42, 1337),
	(as_const_lvalue_ref_t, true, 42, 42, 42),
	(as_rvalue_ref_t, true, 42),
	(as_rvalue_ref_t, false, 42, 1337),
	(as_rvalue_ref_t, true, 42, 42, 42)
)
{
	predicate_fn predicate{ variadicAllEqual };
	const predicate_fn curriedPredicate = (apply_mod<TMod>(predicate) >> ... >> VValues);

	REQUIRE(curriedPredicate() == VExpected);
}

TEST_CASE("back curried predicate_fn can be piped", "[functional][predicate]")
{
	predicate_fn predicate = predicate_fn{ variadicAllEqual } >> 42 >> -2
							| predicate_fn{ std::equal_to{} } << false;

	REQUIRE(predicate());
}

TEST_CASE("predicate_fn accepts functional objects wrapped into a std::reference_wrapper", "[functional][predicate][example]")
{
	//! [predicate wrapped]
	const std::vector sourceInts{ 0, 1, 2, 3 };
	auto skipFirst = [i{ 0 }](auto&&...) mutable { return static_cast<bool>(i++); };

	const auto result = std::ranges::count_if(
		sourceInts,
		predicate_fn{ std::ref(skipFirst) }
	);

	REQUIRE(result == 3);
	//! [predicate wrapped]
}

TEMPLATE_TEST_CASE_SIG(
	"predicate_fn is equal compare composable with operator == as left-hand-side",
	"[functional][predicate]",
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
	auto [chainedFunction, expectedResult] = GENERATE(
		table<empty_predicate_t,
		bool>({
			{ trueFunc, false },
			{ falseFunc, true }
			})
	);

	predicate_fn predicate{ falsePredicate };
	const predicate_fn composedPredicate = apply_mod<TLhsMod>(predicate) == apply_mod<TRhsMod>(chainedFunction);

	REQUIRE(composedPredicate() == expectedResult);
}

TEMPLATE_TEST_CASE_SIG(
	"predicate_fn is equal compare composable with operator == as right-hand-side",
	"[functional][predicate]",
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
	auto [chainedFunction, expectedResult] = GENERATE(
		table<empty_predicate_t,
		bool>({
			{ trueFunc, false },
			{ falseFunc, true }
			})
	);

	predicate_fn predicate{ falsePredicate };
	const predicate_fn composedPredicate = apply_mod<TLhsMod>(chainedFunction) == apply_mod<TRhsMod>(predicate);

	REQUIRE(composedPredicate() == expectedResult);
}

TEMPLATE_TEST_CASE_SIG(
	"predicate_fn is equal comapre composable as param on both sides of operator ==.",
	"[functional][predicate]",
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
	const auto& [lhsPredicate, rhsPredicate, expectedResult] = GENERATE(
		table<empty_predicate_t,
		empty_predicate_t,
		bool>({
			{ falseFunc, falseFunc, true },
			{ trueFunc, falseFunc, false },
			{ falseFunc, trueFunc, false },
			{ trueFunc, trueFunc, true }
			})
	);
	predicate_fn lhs{ lhsPredicate };
	predicate_fn rhs{ rhsPredicate };

	const predicate_fn composedPredicate = apply_mod<TLhsMod>(lhs) == apply_mod<TRhsMod>(rhs);

	REQUIRE(composedPredicate() == expectedResult);
}

TEST_CASE("predicate_fn is equal compare composable with operator in arbitrary depth.", "[functional][predicate]")
{
	REQUIRE((falsePredicate == falsePredicate == truePredicate)());
	REQUIRE(!(falsePredicate == falsePredicate == falsePredicate)());
	REQUIRE((truePredicate == (falsePredicate == falsePredicate))());
	REQUIRE((truePredicate == truePredicate == truePredicate)());
	REQUIRE(((falsePredicate == falsePredicate) == (falsePredicate == falsePredicate))());
}

TEMPLATE_TEST_CASE_SIG(
	"predicate_fn is not_equal compare composable with operator != as left-hand-side",
	"[functional][predicate]",
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
	auto [chainedFunction, expectedResult] = GENERATE(
		table<empty_predicate_t,
		bool>({
			{ trueFunc, true },
			{ falseFunc, false }
			})
	);

	predicate_fn predicate{ falsePredicate };
	const predicate_fn composedPredicate = apply_mod<TLhsMod>(predicate) != apply_mod<TRhsMod>(chainedFunction);

	REQUIRE(composedPredicate() == expectedResult);
}

TEMPLATE_TEST_CASE_SIG(
	"predicate_fn is not_equal compare composable with operator != as right-hand-side",
	"[functional][predicate]",
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
	auto [chainedFunction, expectedResult] = GENERATE(
		table<empty_predicate_t,
		bool>({
			{ trueFunc, true },
			{ falseFunc, false }
			})
	);

	predicate_fn predicate{ falsePredicate };
	const predicate_fn composedPredicate = apply_mod<TLhsMod>(chainedFunction) != apply_mod<TRhsMod>(predicate);

	REQUIRE(composedPredicate() == expectedResult);
}

TEMPLATE_TEST_CASE_SIG(
	"predicate_fn is not_equal comapre composable as param on both sides of operator !=.",
	"[functional][predicate]",
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
	const auto& [lhsPredicate, rhsPredicate, expectedResult] = GENERATE(
		table<empty_predicate_t,
		empty_predicate_t,
		bool>({
			{ falseFunc, falseFunc, false },
			{ trueFunc, falseFunc, true },
			{ falseFunc, trueFunc, true },
			{ trueFunc, trueFunc, false }
			})
	);
	predicate_fn lhs{ lhsPredicate };
	predicate_fn rhs{ rhsPredicate };

	const predicate_fn composedPredicate = apply_mod<TLhsMod>(lhs) != apply_mod<TRhsMod>(rhs);

	REQUIRE(composedPredicate() == expectedResult);
}

TEST_CASE("predicate_fn is not_equal compare composable with operator in arbitrary depth.", "[functional][predicate]")
{
	REQUIRE((falsePredicate != truePredicate != falsePredicate)());
	REQUIRE(!(falsePredicate != falsePredicate != falsePredicate)());
	REQUIRE((truePredicate != (falsePredicate != falsePredicate))());
	REQUIRE((falsePredicate != falsePredicate != truePredicate)());
	REQUIRE(((falsePredicate != falsePredicate) != (falsePredicate != truePredicate))());
}

TEMPLATE_TEST_CASE_SIG(
	"predicate_fn is equivalence compare composable with operator <=> as left-hand-side",
	"[functional][predicate]",
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
	auto [chainedFunction, expectedResult] = GENERATE(
		table<empty_predicate_t,
		bool>({
			{ trueFunc, false },
			{ falseFunc, true }
			})
	);

	predicate_fn predicate{ falsePredicate };
	const predicate_fn composedPredicate = apply_mod<TLhsMod>(predicate) <=> apply_mod<TRhsMod>(chainedFunction);

	REQUIRE(composedPredicate() == expectedResult);
}

TEMPLATE_TEST_CASE_SIG(
	"predicate_fn is equivalence compare composable with operator <=> as right-hand-side",
	"[functional][predicate]",
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
	auto [chainedFunction, expectedResult] = GENERATE(
		table<empty_predicate_t,
		bool>({
			{ trueFunc, false },
			{ falseFunc, true }
			})
	);

	predicate_fn predicate{ falsePredicate };
	const predicate_fn composedPredicate = apply_mod<TLhsMod>(chainedFunction) <=> apply_mod<TRhsMod>(predicate);

	REQUIRE(composedPredicate() == expectedResult);
}

TEMPLATE_TEST_CASE_SIG(
	"predicate_fn is equivalence comapre composable as param on both sides of operator <=>.",
	"[functional][predicate]",
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
	auto [lhsPredicate, rhsPredicate, expectedResult] = GENERATE(
		table<empty_predicate_t,
		empty_predicate_t,
		bool>({
			{ falseFunc, falseFunc, true },
			{ trueFunc, falseFunc, false },
			{ falseFunc, trueFunc, false },
			{ trueFunc, trueFunc, true }
			})
	);
	predicate_fn lhs{ lhsPredicate };
	predicate_fn rhs{ rhsPredicate };

	const predicate_fn composedPredicate = apply_mod<TLhsMod>(lhs) <=> apply_mod<TRhsMod>(rhs);

	REQUIRE(composedPredicate() == expectedResult);
}

TEST_CASE("predicate_fn is equivalence compare composable with operator in arbitrary depth.", "[functional][predicate]")
{
	REQUIRE((falsePredicate <=> falsePredicate <=> falsePredicate)());
	REQUIRE(!(falsePredicate <=> truePredicate <=> falsePredicate)());
	REQUIRE((falsePredicate <=> (falsePredicate <=> falsePredicate))());
	REQUIRE((truePredicate <=> truePredicate <=> truePredicate)());
	REQUIRE(((falsePredicate <=> falsePredicate) <=> (falsePredicate <=> falsePredicate))());
}

TEMPLATE_TEST_CASE("less compares its two parameters", "[functional][predicate]", int, float)
{
	const auto& [value1, value2, expectedResult] = GENERATE(
		table<int,
		int,
		bool>({
			{ 42, 43, true },
			{ 42, 42, false }
			})
	);

	const bool result = less(static_cast<TestType>(value1), static_cast<TestType>(value2));

	REQUIRE(result == expectedResult);
}

TEMPLATE_TEST_CASE("greater compares its two parameters", "[functional][predicate]", int, float)
{
	const auto& [value1, value2, expectedResult] = GENERATE(
		table<int,
		int,
		bool>({
			{ 43, 42, true },
			{ 42, 42, false }
			})
	);

	const bool result = greater(static_cast<TestType>(value1), static_cast<TestType>(value2));

	REQUIRE(result == expectedResult);
}

TEMPLATE_TEST_CASE("less_equal compares its two parameters", "[functional][predicate]", int, float)
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

	const bool result = less_equal(static_cast<TestType>(value1), static_cast<TestType>(value2));

	REQUIRE(result == expectedResult);
}

TEMPLATE_TEST_CASE("greater_equal compares its two parameters", "[functional][predicate]", int, float)
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

	const bool result = greater_equal(static_cast<TestType>(value1), static_cast<TestType>(value2));

	REQUIRE(result == expectedResult);
}

TEMPLATE_TEST_CASE("equal compares its two parameters", "[functional][predicate]", int, float)
{
	const auto& [value1, value2, expectedResult] = GENERATE(
		table<int,
		int,
		bool>({
			{ 42, 42, true },
			{ 43, 42, false }
			})
	);

	const bool result = equal(static_cast<TestType>(value1), static_cast<TestType>(value2));

	REQUIRE(result == expectedResult);
}

TEMPLATE_TEST_CASE("not_equal compares its two parameters", "[functional][predicate]", int, float)
{
	const auto& [value1, value2, expectedResult] = GENERATE(
		table<int,
		int,
		bool>({
			{ 43, 42, true },
			{ 42, 42, false }
			})
	);

	const bool result = not_equal(static_cast<TestType>(value1), static_cast<TestType>(value2));

	REQUIRE(result == expectedResult);
}
