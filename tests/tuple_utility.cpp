//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>

#include "Simple-Utility/tuple_utility.hpp"

using namespace sl;

TEMPLATE_TEST_CASE_SIG(
	"tuple_cat_result yields type of the concatenated tuple.",
	"[tuple][trait]",
	((bool VDummy, class TExpected, class... TTuples), VDummy, TExpected, TTuples...),
	(true, std::tuple<int, float, int&>, const std::tuple<int, float, int&>&, std::tuple<>&),
	(true, std::tuple<int, float, int&>, std::tuple<>&&, const std::tuple<int, float, int&>&),
	(true, std::tuple<int, float>, const std::tuple<int, float>&)
)
{
	STATIC_REQUIRE(std::same_as<typename tuple_cat_result<TTuples...>::type, TExpected>);
	STATIC_REQUIRE(std::same_as<tuple_cat_result_t<TTuples...>, TExpected>);
}

TEMPLATE_TEST_CASE_SIG(
	"apply_invoke_result yields type of the apply invocation.",
	"[tuple][trait]",
	((bool VDummy, class TExpected, class TTuple), VDummy, TExpected, TTuple),
	(true, const int&, const std::tuple<int, float, int&>&),
	(true, int&, std::tuple<int&, float, int>&),
	(true, const float&, const std::tuple<const float&, int>&)
)
{
	constexpr auto get_front = []<class TFront, class... TIgnored>(TFront&& front, TIgnored&&...) -> TFront
	{
		return std::forward<TFront>(front);
	};
	using get_front_t = decltype(get_front);

	STATIC_REQUIRE(std::same_as<typename apply_invoke_result<get_front_t, TTuple>::type, TExpected>);
	STATIC_REQUIRE(std::same_as<apply_invoke_result_t<get_front_t, TTuple>, TExpected>);
}

TEMPLATE_TEST_CASE_SIG(
	"is_apply_invocable checks whether the function is invocable with tuple elements.",
	"[tuple][trait]",
	((bool VExpected, class TTuple), VExpected, TTuple),
	(true, std::tuple<int, float&>),
	(true, const std::tuple<int, float&>),
	(true, std::tuple<int&, float&>),
	(false, std::tuple<int&, const float&>),
	(false, std::tuple<int>),
	(false, std::tuple<int, char>)
)
{
	using function_t = void(*)(int, float&);

	STATIC_REQUIRE(is_apply_invocable<function_t, TTuple>::value == VExpected);
	STATIC_REQUIRE(is_apply_invocable_v<function_t, TTuple> == VExpected);
}

TEMPLATE_TEST_CASE_SIG(
	"is_nothrow_apply_invocable checks whether the function is invocable with tuple elements without throwing.",
	"[tuple][trait]",
	((bool VExpected, class TTuple), VExpected, TTuple),
	(true, std::tuple<int>),
	(false, std::tuple<int&>),
	(false, std::tuple<float>)
)
{
	constexpr auto func = []<class T>(T&&) noexcept(std::same_as<int, T>) {};
	using function_t = decltype(func);

	STATIC_REQUIRE(is_nothrow_apply_invocable<function_t, TTuple>::value == VExpected);
	STATIC_REQUIRE(is_nothrow_apply_invocable_v<function_t, TTuple> == VExpected);
}
