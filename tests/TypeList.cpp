//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/TypeList.hpp"

#include <catch2/catch_template_test_macros.hpp>

#include "helper.hpp"

namespace tl = sl::type_list;

TEMPLATE_TEST_CASE_SIG(
	"std::tuple_size works as expected with type_list::TypeList.",
	"[type_list]",
	((std::size_t expected, class... Ts), expected, Ts...),
	(0),
	(1, int),
	(2, float, int),
	(3, int, int, int)
)
{
	// ReSharper disable once CppUseTypeTraitAlias
	STATIC_REQUIRE(std::tuple_size<tl::TypeList<Ts...>>::value == expected);
	STATIC_REQUIRE(std::tuple_size_v<tl::TypeList<Ts...>> == expected);
}

TEMPLATE_TEST_CASE_SIG(
	"std::tuple_element works as expected with type_list::TypeList.",
	"[type_list]",
	((class Expected, std::size_t index, class... Ts), Expected, index, Ts...),
	(int, 0, int),
	(float, 1, int, float, int&),
	(int&, 2, int, float, int&)
)
{
	// ReSharper disable once CppUseTypeTraitAlias
	STATIC_REQUIRE(std::same_as<Expected, typename std::tuple_element<index, tl::TypeList<Ts...>>::type>);
	STATIC_REQUIRE(std::same_as<Expected, std::tuple_element_t<index, tl::TypeList<Ts...>>>);
}
