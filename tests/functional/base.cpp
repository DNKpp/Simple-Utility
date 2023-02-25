//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>

#include "../helper.hpp"

#include "Simple-Utility/functional/base.hpp"

using namespace sl::functional;

namespace
{
	struct operation_mock_fn
	{
		template <class TCallArgsTuple, class... TFunctions>
			requires (1 < sizeof...(TFunctions))
		[[nodiscard]]
		constexpr bool operator ()
		(
			TCallArgsTuple&& callArgsTuple,
			TFunctions&&... functions
		) const
		{
			return (std::apply(std::forward<TFunctions>(functions), callArgsTuple) == ...);
		}
	};

	inline constexpr auto trueFunc = [](auto&&...) { return true; };
	inline constexpr auto falseFunc = [](auto&&...) { return false; };

	inline constexpr closure_base_fn<std::remove_cvref_t<decltype(trueFunc)>> envelopedTrueFunc{ trueFunc };
	inline constexpr closure_base_fn<std::remove_cvref_t<decltype(falseFunc)>> envelopedFalseFunc{ falseFunc };
}

TEST_CASE("closure_base_fn satisfies derived_from_unified_base concept with closure_base_tag>.", "[functional][base]")
{
	using result_t = decltype(envelopedTrueFunc);
	STATIC_REQUIRE(sl::derived_from_unified_base<result_t, detail::closure_base_tag>);
}

TEMPLATE_TEST_CASE_SIG(
	"unwrap_functional_result determines the first nested non-closure type.",
	"[functional][base][trait]",
	((bool VDummy, class T, class TExpected), VDummy, T, TExpected),
	(true, bool(*)(int&), bool(*)(int&)),
	(true, bool(* const&)(int&), bool(* const&)(int&)),
	(true, bool(* &&)(int&), bool(* &&)(int&)),
	(true, decltype(trueFunc), decltype(trueFunc)),
	(true, std::remove_cvref_t<decltype(trueFunc)>&&, std::remove_cvref_t<decltype(trueFunc)>&&),
	(true, closure_base_fn<bool(*)(int&)>, bool(*)(int&)),
	(true, closure_base_fn<std::remove_cvref_t<decltype(trueFunc)>>, std::remove_cvref_t<decltype(trueFunc)>),
	(true, closure_base_fn<closure_base_fn<bool(*)(int&)>>, bool(*)(int&)),
	(true, closure_base_fn<closure_base_fn<std::remove_cvref_t<decltype(trueFunc)>>>, std::remove_cvref_t<decltype(trueFunc)>)
)
{
	STATIC_REQUIRE(std::same_as<detail::unwrap_functional_r_t<T>, TExpected>);
}

TEST_CASE("composition_fn satisfies derived_from_unified_base concept with composition_base_tag>.", "[functional][base]")
{
	using result_t = decltype(composition_fn{ operation_mock_fn{}, envelopedTrueFunc, trueFunc });
	STATIC_REQUIRE(sl::derived_from_unified_base<result_t, detail::composition_base_tag>);
}

TEST_CASE("composition_fn is constructible with functionals", "[functional][base]")
{
	using expected_t = composition_fn<
		operation_mock_fn,
		std::remove_cvref_t<decltype(falseFunc)>,
		std::remove_cvref_t<decltype(trueFunc)>
	>;
	STATIC_REQUIRE(std::same_as<decltype(composition_fn{ operation_mock_fn{}, falseFunc, trueFunc }), expected_t>);
}

TEST_CASE("composition_fn unwraps functionals on construction.", "[functional][base]")
{
	STATIC_REQUIRE(
		std::same_as<decltype(composition_fn{ operation_mock_fn{}, envelopedTrueFunc, trueFunc }),
		decltype(composition_fn{ operation_mock_fn{}, trueFunc, trueFunc })>
	);
}

TEMPLATE_TEST_CASE_SIG(
	"composition_fn is invocable in arbitrary state.",
	"[functional][base]",
	((auto... VArgs), VArgs...),
	(0),
	(1, nullptr)
)
{
	composition_fn trueComposition{ operation_mock_fn{}, falseFunc, envelopedFalseFunc };
	composition_fn falseComposition{ operation_mock_fn{}, envelopedFalseFunc, trueFunc };

	const auto& mod = GENERATE(make_all_ref_mods_generator());

	REQUIRE(std::invoke(cast(trueComposition, mod), VArgs...));
	REQUIRE(!std::invoke(cast(falseComposition, mod), VArgs...));
}

TEMPLATE_TEST_CASE_SIG(
	"value_fn::value_type holds decayed value.",
	"[functional][base]",
	((bool VDummy, class TSource, class TExpected), VDummy, TSource, TExpected),
	(true, int, int),
	(true, int&, int),
	(true, int*, int*),
	(true, char[5], char*),
	(true, const char[5], const char*),
	(true, const char*, const char*),
	(true, std::reference_wrapper<int>&, std::reference_wrapper<int>)
)
{
	using result_t = typename decltype(value_fn{ std::declval<TSource>() })::value_type;

	STATIC_REQUIRE(std::same_as<result_t, TExpected>);
}

TEMPLATE_TEST_CASE_SIG(
	"value_fn::reference_type holds reference type.",
	"[functional][base]",
	((bool VDummy, class TSource, class TExpected), VDummy, TSource, TExpected),
	(true, int, const int&),
	(true, int&, const int&),
	(true, int*, int* const&),
	(true, int* const, int* const&),
	(true, const int* const, const int* const&),
	(true, char[5], char* const&),
	(true, const char[5], const char* const&),
	(true, std::reference_wrapper<int>&, int&),
	(true, std::reference_wrapper<const int>&, const int&)
)
{
	using result_t = typename decltype(value_fn{ std::declval<TSource>() })::reference_type;

	STATIC_REQUIRE(std::same_as<result_t, TExpected>);
}

TEMPLATE_TEST_CASE_SIG(
	"value_fn supports arbitrary types.",
	"[functional][base]",
	((bool VDummy, template <class> class TMod, class TValue, class TExpected), VDummy, TMod, TValue, TExpected),
	(true, as_lvalue_ref_t, int, const int&),
	(true, as_const_lvalue_ref_t, int, const int&),
	(true, as_rvalue_ref_t, int, int),
	(true, as_const_rvalue_ref_t, int, int),

	(true, as_lvalue_ref_t, int*, int* const&),
	(true, as_const_lvalue_ref_t, int*, int* const&),
	(true, as_rvalue_ref_t, int*, int*),
	(true, as_const_rvalue_ref_t, int*, int*),

	(true, as_lvalue_ref_t, char[5], char* const&),
	(true, as_const_lvalue_ref_t, char[5], char* const&),
	(true, as_rvalue_ref_t, char[5], char*),
	(true, as_const_rvalue_ref_t, char[5], char*)
)
{
	TValue v{};
	value_fn value{ v };

	TExpected retrieved = type_mod<TMod>::cast(value)();

	REQUIRE(retrieved == v);
}

TEST_CASE("value_fn treats std::reference_wrapper types special.", "[functional][base]")
{
	const auto& refMod = GENERATE(make_all_ref_mods_generator());

	//! [value_fn wrapped]
	int x{ 42 };
	value_fn value{ std::ref(x) };

	int& x_ref = std::invoke(cast(value, refMod));
	x_ref = 1337;

	REQUIRE(x == 1337);
	//! [value_fn wrapped]
}
