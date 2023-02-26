//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>

#include <array>
#include <vector>

#include "Simple-Utility/concepts/utility.hpp"

using namespace sl::concepts;

TEMPLATE_TEST_CASE_SIG(
	"unique_types and are_types_unique_v determine if all types of a param pack are unique.",
	"[concepts]",
	((bool VExpected, class... TArgs), VExpected, TArgs...),
	(false, int, int),
	(true, int, float, int&, int&&, const int),
	(false, int, float, int&, int&&, const int, int&)
)
{
	STATIC_REQUIRE(are_types_unique_v<TArgs...> == VExpected);
	STATIC_REQUIRE(unique_types<TArgs...> == VExpected);
}

TEMPLATE_TEST_CASE_SIG(
	"apply_invocable checks whether the function is invocable with tuple elements.",
	"[concepts][tuple]",
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

	STATIC_REQUIRE(apply_invocable<function_t, TTuple> == VExpected);
}

TEMPLATE_TEST_CASE_SIG(
	"nothrow_apply_invocable checks whether the function is invocable with tuple elements without throwing.",
	"[concepts][tuple]",
	((bool VExpected, class TTuple), VExpected, TTuple),
	(true, std::tuple<int>),
	(false, std::tuple<int&>),
	(false, std::tuple<float>)
)
{
	constexpr auto func = []<class T>(T&&) noexcept(std::same_as<int, T>) {};
	using function_t = decltype(func);

	STATIC_REQUIRE(nothrow_apply_invocable<function_t, TTuple> == VExpected);
}

TEMPLATE_TEST_CASE_SIG(
	"tuple_like concept determines if a type can be used as tuple.",
	"[tuple][trait]",
	((bool VExpected, class TTuple), VExpected, TTuple),
	(false, std::vector<int>),
	(true, std::tuple<>),
	(true, std::tuple<int>),
	(true, std::pair<int, int>),
	(true, std::array<int, 0>)
)
{
	STATIC_REQUIRE(tuple_like<TTuple> == VExpected);
}
