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

template <class T>
struct test_transformation
{
	using type = tl::TypeList<T>;
};

TEMPLATE_PRODUCT_TEST_CASE(
	"type_list::transform transforms each element.",
	"[type_list]",
	trait_test,
	(
		(tl::TypeList<>, tl::TypeList<>),
		(tl::TypeList<int>, tl::TypeList<tl::TypeList<int>>),
		(tl::TypeList<float, int>, tl::TypeList<tl::TypeList<float>, tl::TypeList<int>>),
		(tl::TypeList<float, tl::TypeList<int>>, tl::TypeList<tl::TypeList<float>, tl::TypeList<tl::TypeList<int>>>),
		(std::tuple<float, int>, std::tuple<tl::TypeList<float>, tl::TypeList<int>>)
	)
)
{
	STATIC_REQUIRE(std::same_as<expected_t<TestType>, typename tl::transform<test_transformation, input_t<TestType>>::type>);
	STATIC_REQUIRE(std::same_as<expected_t<TestType>, tl::transform_t<test_transformation, input_t<TestType>>>);
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
	STATIC_REQUIRE(std::same_as<Expected, typename tl::populated_from<TargetContainer, Input>::type>);
	STATIC_REQUIRE(std::same_as<Expected, tl::populated_from_t<TargetContainer, Input>>);
}

TEMPLATE_PRODUCT_TEST_CASE(
	"type_list::tail_t trims the first element from the container.",
	"[type_list]",
	trait_test,
	(
		(tl::TypeList<>, tl::TypeList<>),
		(tl::TypeList<int>, tl::TypeList<>),
		(tl::TypeList<float, int>, tl::TypeList<int>),
		(tl::TypeList<float, tl::TypeList<int>>, tl::TypeList<tl::TypeList<int>>),
		(std::tuple<float, int>, std::tuple<int>)
	)
)
{
	STATIC_REQUIRE(std::same_as<expected_t<TestType>, typename tl::tail<input_t<TestType>>::type>);
	STATIC_REQUIRE(std::same_as<expected_t<TestType>, tl::tail_t<input_t<TestType>>>);
}

TEMPLATE_PRODUCT_TEST_CASE(
	"type_list::front_t yields the type of the first element.",
	"[type_list]",
	trait_test,
	(
		(tl::TypeList<int>, int),
		(tl::TypeList<float, int>, float),
		(tl::TypeList<tl::TypeList<float>, int>, tl::TypeList<float>),
		(std::tuple<float, int>, float)
	)
)
{
	STATIC_REQUIRE(std::same_as<expected_t<TestType>, typename tl::front<input_t<TestType>>::type>);
	STATIC_REQUIRE(std::same_as<expected_t<TestType>, tl::front_t<input_t<TestType>>>);
}

TEMPLATE_PRODUCT_TEST_CASE(
	"type_list::back_t yields the type of the last element.",
	"[type_list]",
	trait_test,
	(
		(tl::TypeList<int>, int),
		(tl::TypeList<float, int>, int),
		(tl::TypeList<float, tl::TypeList<int>>, tl::TypeList<int>),
		(std::tuple<float, int>, int)
	)
)
{
	STATIC_REQUIRE(std::same_as<expected_t<TestType>, typename tl::back<input_t<TestType>>::type>);
	STATIC_REQUIRE(std::same_as<expected_t<TestType>, tl::back_t<input_t<TestType>>>);
}

TEMPLATE_TEST_CASE_SIG(
	"type_list::concat_t appends all additional list into the first.",
	"[type_list]",
	((bool dummy, class Expected, class... Lists), dummy, Expected, Lists...),
	(true, tl::TypeList<>, tl::TypeList<>),
	(true, tl::TypeList<int>, tl::TypeList<int>),
	(true, tl::TypeList<int, int&>, tl::TypeList<int>, tl::TypeList<int&>),
	(true, tl::TypeList<int, int&&, int&>, tl::TypeList<int, int&&>, tl::TypeList<int&>),
	(true, tl::TypeList<int, int&&, int&, const int&>, tl::TypeList<int, int&&>, tl::TypeList<int&>, tl::TypeList<const int&>)
)
{
	STATIC_REQUIRE(std::same_as<Expected, typename tl::concat<Lists...>::type>);
	STATIC_REQUIRE(std::same_as<Expected, tl::concat_t<Lists...>>);
}
