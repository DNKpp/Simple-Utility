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

#ifdef SL_UTILITY_NO_UNIQUE_ADDRESS_ENABLED
TEST_CASE("closure_base_fn is an empty type if contained function is an empty type.", "[functional][base]")
{
	STATIC_REQUIRE(std::is_empty_v<decltype(closure_base_fn{trueFunc})>);
}
#endif

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
	"composition_fn is invocable with any possible param combination.",
	"[functional][base]",
	((auto... VArgs), VArgs...),
	(0),
	(1, nullptr)
)
{
	composition_fn trueComposition{operation_mock_fn{}, falseFunc, envelopedFalseFunc};
	composition_fn falseComposition{operation_mock_fn{}, envelopedFalseFunc, trueFunc};

	REQUIRE(trueComposition());
	REQUIRE(!falseComposition());
}

TEMPLATE_LIST_TEST_CASE(
	"composition_fn is invocable from any reference type.",
	"[functional][base]",
	all_ref_mods_list
)
{
	composition_fn trueComposition{operation_mock_fn{}, falseFunc, envelopedFalseFunc};

	REQUIRE(std::invoke(TestType::cast(trueComposition), 42));
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

TEMPLATE_TEST_CASE_SIG(
	"Invoking value_fn yields expected types.",
	"[functional][base]",
	((bool dummy, class Expected, class Input), dummy, Expected, Input),
	(true, int, int),
	(true, int*, int*),
	(true, const int*, const int*),
	(true, std::string, std::string),
	(true, int&, std::reference_wrapper<int>),
	(true, const int&, std::reference_wrapper<const int>)
)
{
	using const_lvalue_result_t = std::invoke_result_t<as_const_lvalue_ref_t<value_fn<Input>>>;
	using lvalue_result_t = std::invoke_result_t<as_lvalue_ref_t<value_fn<Input>>>;
	using const_rvalue_result_t = std::invoke_result_t<as_const_rvalue_ref_t<value_fn<Input>>>;
	using rvalue_result_t = std::invoke_result_t<as_rvalue_ref_t<value_fn<Input>>>;

	STATIC_REQUIRE(std::same_as<Expected, const_lvalue_result_t>);
	STATIC_REQUIRE(std::same_as<Expected, lvalue_result_t>);
	STATIC_REQUIRE(std::same_as<Expected, const_rvalue_result_t>);
	STATIC_REQUIRE(std::same_as<Expected, rvalue_result_t>);
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

TEMPLATE_LIST_TEST_CASE("value_fn is copy- and move-constructible.", "[functional][base]", all_ref_mods_list)
{
	value_fn source{int{42}};
	const value_fn target{TestType::cast(source)};

	STATIC_REQUIRE(std::same_as<int, typename std::remove_cvref_t<decltype(target)>::value_type>);

	REQUIRE(target() == 42);
}

TEMPLATE_LIST_TEST_CASE("value_fn is copy- and move-assignable.", "[functional][base]", all_ref_mods_list)
{
	value_fn source{int{42}};
	value_fn<int> target{1337};

	target = TestType::cast(source);

	REQUIRE(target() == 42);
}
