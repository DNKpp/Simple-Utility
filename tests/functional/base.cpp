//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "../helper.hpp"

#include <string>

#include "Simple-Utility/functional/base.hpp"

using namespace sl::functional;

namespace
{
	struct operation_mock_fn
	{
		template <class TCallArgsTuple, class... TFunctions>
			requires (1 < sizeof...(TFunctions))
		[[nodiscard]]
		constexpr bool operator ()(
			TCallArgsTuple&& callArgsTuple,
			TFunctions&&... functions
		) const
		{
			return (std::apply(std::forward<TFunctions>(functions), callArgsTuple) == ...);
		}
	};

	inline constexpr auto trueFunc = [](auto&&...) { return true; };
	inline constexpr auto falseFunc = [](auto&&...) { return false; };

	inline constexpr closure_base_fn<std::remove_cvref_t<decltype(trueFunc)>> envelopedTrueFunc{trueFunc};
	inline constexpr closure_base_fn<std::remove_cvref_t<decltype(falseFunc)>> envelopedFalseFunc{falseFunc};
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
	using result_t = decltype(composition_fn{operation_mock_fn{}, envelopedTrueFunc, trueFunc});
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
	composition_fn trueComposition{operation_mock_fn{}, falseFunc, envelopedFalseFunc};
	composition_fn falseComposition{operation_mock_fn{}, envelopedFalseFunc, trueFunc};

	const auto& mod = GENERATE(make_all_ref_mods_generator());

	REQUIRE(std::invoke(cast(trueComposition, mod), VArgs...));
	REQUIRE(!std::invoke(cast(falseComposition, mod), VArgs...));
}

TEMPLATE_PRODUCT_TEST_CASE(
	"value_fn::value_type holds decayed value.",
	"[functional][base]",
	(
		as_const_lvalue_ref_t,
		as_lvalue_ref_t,
		as_const_rvalue_ref_t,
		as_rvalue_ref_t
	),
	(
		int,
		int*,
		const int*,
		char[5],
		const char[5],
		char[5][2],
		const char[5][2],
		std::string,
		std::reference_wrapper<int>
	)
)
{
	using CTAD_value_fn = decltype(value_fn{std::declval<TestType>()});
	using value_type = typename CTAD_value_fn::value_type;

	STATIC_REQUIRE(std::same_as<value_type, std::decay_t<TestType>>);
}

TEMPLATE_TEST_CASE(
	"Invoking value_fn yields expected types.",
	"[functional][base]",
	int,
	int*,
	const int*,
	std::string,
	std::reference_wrapper<int>,
	std::reference_wrapper<const int>
)
{
	using const_lvalue_result_t = std::invoke_result_t<as_const_lvalue_ref_t<value_fn<TestType>>>;
	using lvalue_result_t = std::invoke_result_t<as_lvalue_ref_t<value_fn<TestType>>>;
	using const_rvalue_result_t = std::invoke_result_t<as_const_rvalue_ref_t<value_fn<TestType>>>;
	using rvalue_result_t = std::invoke_result_t<as_rvalue_ref_t<value_fn<TestType>>>;

	STATIC_REQUIRE(std::is_same_v<const_lvalue_result_t, as_const_lvalue_ref_t<TestType>>);
	STATIC_REQUIRE(std::is_same_v<lvalue_result_t, as_const_lvalue_ref_t<TestType>>);
	STATIC_REQUIRE(std::is_same_v<const_rvalue_result_t, as_const_rvalue_ref_t<TestType>>);
	STATIC_REQUIRE(std::is_same_v<rvalue_result_t, as_rvalue_ref_t<TestType>>);
}

TEMPLATE_LIST_TEST_CASE("value_fn supports trivial types.", "[functional][base]", all_ref_mods_list)
{
	//! [value_fn trivial]
	value_fn value{int{42}};

	const int x = std::invoke(TestType::cast(value));

	REQUIRE(x == 42);
	//! [value_fn trivial]
}

TEMPLATE_LIST_TEST_CASE("value_fn supports array types.", "[functional][base]", all_ref_mods_list)
{
	//! [value_fn array]
	int arr[3]{42, 1337, -1};
	value_fn value{arr};

	const int* dataBegin = std::invoke(TestType::cast(value));

	REQUIRE(dataBegin == arr);
	//! [value_fn array]
}

TEMPLATE_LIST_TEST_CASE("value_fn supports complex types.", "[functional][base]", all_ref_mods_list)
{
	//! [value_fn complex]
	value_fn value{std::string{"Hello, World!"}};

	const std::string str = std::invoke(TestType::cast(value));

	REQUIRE_THAT(str, Catch::Matchers::Equals("Hello, World!"));
	//! [value_fn complex]
}

TEMPLATE_LIST_TEST_CASE("value_fn supports std::reference_wrapper types.", "[functional][base]", all_ref_mods_list)
{
	//! [value_fn wrapped]
	int x{42};
	value_fn value{std::ref(x)};

	int& x_ref = std::invoke(TestType::cast(value));
	x_ref = 1337;

	REQUIRE(x == 1337);
	//! [value_fn wrapped]
}
