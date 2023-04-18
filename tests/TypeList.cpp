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

TEMPLATE_TEST_CASE_SIG(
	"concepts::type_list_like checks for valid types.",
	"[type_list][concept]",
	((bool expected, class T), expected, T),
	(false, std::vector<int>),
	(true, tl::TypeList<int>),
	(true, std::tuple<>),
	(true, std::tuple<int>),
	(true, std::pair<int, int>),
	(true, std::array<int, 0>)
)
{
	STATIC_REQUIRE(sl::concepts::type_list_like<T> == expected);
}

TEMPLATE_TEST_CASE_SIG(
	"type_list::populated_from_t fills the target container with content from source.",
	"[type_list]",
	((bool dummy, class Input, template <class...> class TargetContainer, class Expected), dummy, Input, TargetContainer, Expected),
	(true, std::tuple<>, tl::TypeList, tl::TypeList<>),
	(true, std::tuple<int>, tl::TypeList, tl::TypeList<int>),
	(true, std::tuple<float, int>, tl::TypeList, tl::TypeList<float, int>),
	(true, std::tuple<float, tl::TypeList<int>>, tl::TypeList, tl::TypeList<float, tl::TypeList<int>>),
	(true, tl::TypeList<float, tl::TypeList<int>>, std::tuple, std::tuple<float, tl::TypeList<int>>)
)
{
	STATIC_REQUIRE(std::same_as<Expected, typename tl::populate_from<TargetContainer, Input>::type>);
	STATIC_REQUIRE(std::same_as<Expected, tl::populated_from_t<TargetContainer, Input>>);
}
