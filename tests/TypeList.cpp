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

TEMPLATE_PRODUCT_TEST_CASE(
	"type_list::tail trims the first element from the container.",
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

TEMPLATE_TEST_CASE_SIG(
	"type_list::index_of yields index of the queried type.",
	"[type_list]",
	((std::size_t expected, class Query, class T), expected, Query, T),
	(0, int, tl::TypeList<int>),
	(0, int, tl::TypeList<int, int&>),
	(1, int&, tl::TypeList<int, int&, int&, float>),
	(3, float, tl::TypeList<int, int&, int&, float>),
	(1, int&, std::tuple<int, int&, int&, float>)
)
{
	STATIC_REQUIRE(expected == tl::index_of<T, Query>::value);
	STATIC_REQUIRE(expected == tl::index_of_v<T, Query>);
}

TEMPLATE_TEST_CASE_SIG(
	"type_list::contains determines whether the queried type is contained inside the type-list.",
	"[type_list]",
	((bool expected, class Query, class T), expected, Query, T),
	(false, int, tl::TypeList<>),
	(false, int&&, tl::TypeList<int, int&>),
	(true, int, tl::TypeList<int>),
	(true, int, tl::TypeList<int, int&>),
	(true, int&, tl::TypeList<int, int&, int&, float>),
	(true, float, tl::TypeList<int, int&, int&, float>),
	(true, int&, std::tuple<int, int&, int&, float>)
)
{
	STATIC_REQUIRE(expected == tl::contains<T, Query>::value);
	STATIC_REQUIRE(expected == tl::contains_v<T, Query>);
}

TEMPLATE_TEST_CASE_SIG(
	"type_list::count determines how often the queried type is contained inside the type-list.",
	"[type_list]",
	((std::size_t expected, class Query, class T), expected, Query, T),
	(0, int, tl::TypeList<>),
	(0, int&&, tl::TypeList<int, int&>),
	(1, int, tl::TypeList<int>),
	(1, int, tl::TypeList<int, int&>),
	(2, int&, tl::TypeList<int, int&, int&, float>),
	(1, float, tl::TypeList<int, int&, int&, float>),
	(2, int&, std::tuple<int, int&, int&, float>)
)
{
	STATIC_REQUIRE(expected == tl::count<Query, T>::value);
	STATIC_REQUIRE(expected == tl::count_v<Query, T>);
}

TEMPLATE_TEST_CASE_SIG(
	"type_list::equal determines whether all given type-lists contain the same elements in equal order.",
	"[type_list]",
	((bool expected, class... Lists), expected, Lists...),
	(true, tl::TypeList<>),
	(true, tl::TypeList<int, int&>),
	(true, tl::TypeList<int>, tl::TypeList<int>),
	(true, tl::TypeList<int>, tl::TypeList<int>, tl::TypeList<int>),
	(true, std::tuple<int>, tl::TypeList<int>, tl::TypeList<int>),
	(false, tl::TypeList<int>, tl::TypeList<int&>),
	(false, tl::TypeList<int>, tl::TypeList<int, int>),
	(false, tl::TypeList<int, int&>, tl::TypeList<int&, int>),
	(false, std::tuple<float>, tl::TypeList<int>, tl::TypeList<int>),
	(false, std::tuple<int>, tl::TypeList<float>, tl::TypeList<int>)
)
{
	STATIC_REQUIRE(expected == tl::equal<Lists...>::value);
	STATIC_REQUIRE(expected == tl::equal_v<Lists...>);

	STATIC_REQUIRE(expected == tl::populated_from_t<tl::equal, tl::reverse_t<tl::TypeList<Lists...>>>::value);
}

TEMPLATE_TEST_CASE_SIG(
	"type_list::unordered_equal determines whether all given type-lists contain the same elements.",
	"[type_list]",
	((bool expected, class... Lists), expected, Lists...),
	(true, tl::TypeList<>),
	(true, tl::TypeList<int, int&>),
	(true, tl::TypeList<int>, tl::TypeList<int>),
	(true, tl::TypeList<int>, tl::TypeList<int>, tl::TypeList<int>),
	(true, std::tuple<int>, tl::TypeList<int>, tl::TypeList<int>),
	(false, tl::TypeList<int>, tl::TypeList<int&>),
	(false, tl::TypeList<int>, tl::TypeList<int, int>),
	(true, tl::TypeList<int, int&>, tl::TypeList<int&, int>),
	(false, std::tuple<float>, tl::TypeList<int>, tl::TypeList<int>),
	(false, std::tuple<int>, tl::TypeList<float>, tl::TypeList<int>)
)
{
	STATIC_REQUIRE(expected == tl::unordered_equal<Lists...>::value);
	STATIC_REQUIRE(expected == tl::unordered_equal_v<Lists...>);

	STATIC_REQUIRE(expected == tl::populated_from_t<tl::unordered_equal, tl::reverse_t<tl::TypeList<Lists...>>>::value);
}

TEMPLATE_TEST_CASE_SIG(
	"concepts::type_list checks for valid types.",
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
	STATIC_REQUIRE(sl::concepts::type_list<T> == expected);
}

TEMPLATE_TEST_CASE_SIG(
	"concepts::populated_type_list checks whether a type-list contains elements.",
	"[type_list][concept]",
	((bool expected, class T), expected, T),
	(false, std::vector<int>),
	(false, tl::TypeList<>),
	(false, std::tuple<>),
	(false, std::array<int, 0>),
	(true, tl::TypeList<int>),
	(true, std::tuple<int>),
	(true, std::pair<int, int>),
	(true, std::array<int, 1>)
)
{
	STATIC_REQUIRE(sl::concepts::populated_type_list<T> == expected);
}

//! [transformation definition]
template <class T>
struct test_transformation
{
	using type = tl::TypeList<T>;
};

//! [transformation definition]

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

template <class T>
struct lvalue_reference_filter
	: public std::bool_constant<std::is_lvalue_reference_v<T>>
{
};

TEMPLATE_TEST_CASE_SIG(
	"type_list::filter removes all types from the type-list for which the predicate yields false.",
	"[type_list]",
	((bool dummy, class Expected, class Input), dummy, Expected, Input),
	(true, tl::TypeList<>, tl::TypeList<>),
	(true, tl::TypeList<>, tl::TypeList<int>),
	(true, tl::TypeList<>, tl::TypeList<int, int>),
	(true, tl::TypeList<float&, int&>, tl::TypeList<float&, int, int&>),
	(true, tl::TypeList<int&, float&>, tl::TypeList<float, int&, float&, int>),
	(true, std::tuple<int&, float&>, std::tuple<float, int&, float&, int>)
)
{
	STATIC_REQUIRE(std::same_as<Expected, typename tl::filter<Input, std::is_lvalue_reference>::type>);
	STATIC_REQUIRE(std::same_as<Expected, tl::filter_t<Input, std::is_lvalue_reference>>);
}

TEMPLATE_TEST_CASE_SIG(
	"type_list::remove removes all appearances of a given type from the type-list.",
	"[type_list]",
	((bool dummy, class Expected, class Input, class Type), dummy, Expected, Input, Type),
	(true, tl::TypeList<>, tl::TypeList<>, int),
	(true, tl::TypeList<>, tl::TypeList<int>, int),
	(true, tl::TypeList<>, tl::TypeList<int, int>, int),
	(true, tl::TypeList<float>, tl::TypeList<float, int, int>, int),
	(true, tl::TypeList<float, float&>, tl::TypeList<float, int, float&, int>, int),
	(true, std::tuple<float, float&>, std::tuple<float, int, float&, int>, int)
)
{
	STATIC_REQUIRE(std::same_as<Expected, typename tl::remove<Input, Type>::type>);
	STATIC_REQUIRE(std::same_as<Expected, tl::remove_t<Input, Type>>);
}

TEMPLATE_TEST_CASE_SIG(
	"type_list::remove_at removes the given index from the source type-list.",
	"[type_list]",
	((class Expected, class SourceList, std::size_t index), Expected, SourceList, index),
	(tl::TypeList<>, tl::TypeList<int>, 0),
	(tl::TypeList<int&>, tl::TypeList<int, int&>, 0),
	(tl::TypeList<int>, tl::TypeList<int, int&>, 1),
	(std::tuple<int>, std::tuple<int, int&>, 1)
)
{
	STATIC_REQUIRE(std::same_as<Expected, typename tl::remove_at<SourceList, index>::type>);
	STATIC_REQUIRE(std::same_as<Expected, tl::remove_at_t<SourceList, index>>);
}

TEMPLATE_PRODUCT_TEST_CASE(
	"type_list::pop_front trims the first element from the container.",
	"[type_list]",
	trait_test,
	(
		(tl::TypeList<int>, tl::TypeList<>),
		(tl::TypeList<float, int>, tl::TypeList<int>),
		(tl::TypeList<float, tl::TypeList<int>>, tl::TypeList<tl::TypeList<int>>),
		(std::tuple<float, int>, std::tuple<int>)
	)
)
{
	STATIC_REQUIRE(std::same_as<expected_t<TestType>, typename tl::pop_front<input_t<TestType>>::type>);
	STATIC_REQUIRE(std::same_as<expected_t<TestType>, tl::pop_front_t<input_t<TestType>>>);
}

TEMPLATE_PRODUCT_TEST_CASE(
	"type_list::pop_front trims the last element from the container.",
	"[type_list]",
	trait_test,
	(
		(tl::TypeList<int>, tl::TypeList<>),
		(tl::TypeList<float, int>, tl::TypeList<float>),
		(tl::TypeList<float, tl::TypeList<int>>, tl::TypeList<float>),
		(std::tuple<float, int>, std::tuple<float>)
	)
)
{
	STATIC_REQUIRE(std::same_as<expected_t<TestType>, typename tl::pop_back<input_t<TestType>>::type>);
	STATIC_REQUIRE(std::same_as<expected_t<TestType>, tl::pop_back_t<input_t<TestType>>>);
}

TEMPLATE_TEST_CASE_SIG(
	"type_list::append adds all given types at the end of the source type-list.",
	"[type_list]",
	((bool dummy, class Expected, class SourceList, class... Types), dummy, Expected, SourceList, Types...),
	(true, tl::TypeList<>, tl::TypeList<>),
	(true, tl::TypeList<int>, tl::TypeList<>, int),
	(true, tl::TypeList<int, float>, tl::TypeList<int>, float),
	(true, tl::TypeList<int, float, tl::TypeList<float&>>, tl::TypeList<int, float>, tl::TypeList<float&>),
	(true, std::tuple<int, float>, std::tuple<int>, float),
	(true, std::tuple<int, float, tl::TypeList<float&>>, std::tuple<int, float>, tl::TypeList<float&>)
)
{
	STATIC_REQUIRE(std::same_as<Expected, typename tl::append<SourceList, Types...>::type>);
	STATIC_REQUIRE(std::same_as<Expected, tl::append_t<SourceList, Types...>>);
}

TEMPLATE_TEST_CASE_SIG(
	"type_list::prepend adds all given types at the begin of the source type-list.",
	"[type_list]",
	((bool dummy, class Expected, class SourceList, class... Types), dummy, Expected, SourceList, Types...),
	(true, tl::TypeList<>, tl::TypeList<>),
	(true, tl::TypeList<int>, tl::TypeList<>, int),
	(true, tl::TypeList<float, int>, tl::TypeList<int>, float),
	(true, tl::TypeList<tl::TypeList<float&>, int, float>, tl::TypeList<int, float>, tl::TypeList<float&>),
	(true, std::tuple<float, int>, std::tuple<int>, float),
	(true, std::tuple<tl::TypeList<float&>, int, float>, std::tuple<int, float>, tl::TypeList<float&>)
)
{
	STATIC_REQUIRE(std::same_as<Expected, typename tl::prepend<SourceList, Types...>::type>);
	STATIC_REQUIRE(std::same_as<Expected, tl::prepend_t<SourceList, Types...>>);
}

TEMPLATE_PRODUCT_TEST_CASE(
	"type_list::reverse reverses the order of all elements of the source type-list.",
	"[type_list]",
	trait_test,
	(
		(tl::TypeList<>, tl::TypeList<>),
		(tl::TypeList<int>, tl::TypeList<int>),
		(tl::TypeList<float, int>, tl::TypeList<int, float>),
		(tl::TypeList<float, tl::TypeList<int>>, tl::TypeList<tl::TypeList<int>, float>),
		(std::tuple<float, int>, std::tuple<int, float>)
	)
)
{
	STATIC_REQUIRE(std::same_as<expected_t<TestType>, typename tl::reverse<input_t<TestType>>::type>);
	STATIC_REQUIRE(std::same_as<expected_t<TestType>, tl::reverse_t<input_t<TestType>>>);
}

TEMPLATE_PRODUCT_TEST_CASE(
	"type_list::unique removes all type duplicates.",
	"[type_list]",
	trait_test,
	(
		(tl::TypeList<>, tl::TypeList<>),
		(tl::TypeList<int>, tl::TypeList<int>),
		(tl::TypeList<int, int&>, tl::TypeList<int, int&>),
		(tl::TypeList<int, int>, tl::TypeList<int>),
		(tl::TypeList<int, int&, int>, tl::TypeList<int, int&>),
		(tl::TypeList<int, int, int>, tl::TypeList<int>),
		(std::tuple<int, int, int>, std::tuple<int>)
	)
)
{
	STATIC_REQUIRE(std::same_as<expected_t<TestType>, typename tl::unique<input_t<TestType>>::type>);
	STATIC_REQUIRE(std::same_as<expected_t<TestType>, tl::unique_t<input_t<TestType>>>);
}

/*
 * This test case was very tricky, as the initial approach, just comparing the two templates via the usual specialization technique,
 * was flawed and lead me to a language defect. It is perfectly answered here:
 * https://stackoverflow.com/questions/76048362/whats-the-correct-outcome-of-that-template-template-type-specialization
 *
 * Even if the current approach is merely hack, it should do fine (hopefully).
 */
template <class T, class... Others>
using AliasTypeList = tl::TypeList<T, Others...>;

TEMPLATE_TEST_CASE_SIG(
	"type_list::common_container_t yields a templated alias.",
	"[type_list]",
	((bool dummy, template <class...> class Expected, class... Lists), dummy, Expected, Lists...),
	(true, std::tuple, std::tuple<int>),
	(true, tl::TypeList, tl::TypeList<>),
	(true, tl::TypeList, tl::TypeList<int>, tl::TypeList<>),
	(true, tl::TypeList, tl::TypeList<int>, AliasTypeList<float, int&>)
)
{
	STATIC_REQUIRE(std::same_as<Expected<>, typename tl::common_container<Lists...>::template type<>>);
}

TEMPLATE_TEST_CASE_SIG(
	"type_list::concat_t appends all additional list into the first.",
	"[type_list]",
	((bool dummy, class Expected, class... Lists), dummy, Expected, Lists...),
	(true, tl::TypeList<>, tl::TypeList<>),
	(true, tl::TypeList<int>, tl::TypeList<int>),
	(true, tl::TypeList<int>, tl::TypeList<int>, tl::TypeList<>),
	(true, tl::TypeList<int, int&>, tl::TypeList<int>, tl::TypeList<int&>),
	(true, tl::TypeList<int, int&&, int&>, tl::TypeList<int, int&&>, tl::TypeList<int&>),
	(true, tl::TypeList<int, int&&, int&, const int&>, tl::TypeList<int, int&&>, tl::TypeList<int&>, tl::TypeList<const int&>)
)
{
	STATIC_REQUIRE(std::same_as<Expected, typename tl::concat<Lists...>::type>);
	STATIC_REQUIRE(std::same_as<Expected, tl::concat_t<Lists...>>);
}

TEMPLATE_TEST_CASE_SIG(
	"type_list::zip_nth_elements_as creates a TargetContainer with elements at a specific index of the provides lists.",
	"[type_list]",
	((class Expected, template <class...> class TargetContainer, std::size_t index, class... Lists),
		Expected, TargetContainer, index, Lists...),
	(tl::TypeList<>, tl::TypeList, 0),
	(tl::TypeList<int>, tl::TypeList, 0, tl::TypeList<int>),
	(tl::TypeList<int, int>, tl::TypeList, 0, tl::TypeList<int>, tl::TypeList<int>),
	(tl::TypeList<int, int&>, tl::TypeList, 0, tl::TypeList<int>, tl::TypeList<int&>),
	(tl::TypeList<int&&, const int&>, tl::TypeList, 1, tl::TypeList<int, int&&>, tl::TypeList<int&, const int&>),
	(tl::TypeList<int&&, const int&>, tl::TypeList, 1, std::tuple<int, int&&>, std::tuple<int&, const int&>),
	(std::tuple<int&&, const int&>, std::tuple, 1, std::tuple<int, int&&>, std::tuple<int&, const int&>)
)
{
	STATIC_REQUIRE(std::same_as<Expected, typename tl::zip_nth_elements_as<TargetContainer, index, Lists...>::type>);
	STATIC_REQUIRE(std::same_as<Expected, tl::zip_nth_elements_as_t<TargetContainer, index, Lists...>>);
}

TEMPLATE_TEST_CASE_SIG(
	"type_list::zip_nth_elements creates a common_container type with elements at a specific index of the provides lists.",
	"[type_list]",
	((class Expected, std::size_t index, class... Lists), Expected, index, Lists...),
	//(tl::TypeList<>, 0), common_container isn't possible with zero lists
	(tl::TypeList<int>, 0, tl::TypeList<int>),
	(tl::TypeList<int, int>, 0, tl::TypeList<int>, tl::TypeList<int>),
	(tl::TypeList<int, int&>, 0, tl::TypeList<int>, tl::TypeList<int&>),
	(tl::TypeList<int&&, const int&>, 1, tl::TypeList<int, int&&>, tl::TypeList<int&, const int&>),
	(std::tuple<int&&, const int&>, 1, std::tuple<int, int&&>, std::tuple<int&, const int&>)
)
{
	STATIC_REQUIRE(std::same_as<Expected, typename tl::zip_nth_elements<index, Lists...>::type>);
	STATIC_REQUIRE(std::same_as<Expected, tl::zip_nth_elements_t<index, Lists...>>);
}

TEMPLATE_TEST_CASE_SIG(
	"type_list::zip_as creates a TargetContainer of TargetContainers with elements of the provides lists.",
	"[type_list]",
	((bool dummy, class Expected, template <class...> class TargetContainer, class... Lists), dummy, Expected, TargetContainer, Lists
		...),
	(true, tl::TypeList<>, tl::TypeList),
	(true, tl::TypeList<>, tl::TypeList, tl::TypeList<>),
	(true, tl::TypeList<tl::TypeList<int>>, tl::TypeList, tl::TypeList<int>),
	(true, tl::TypeList<tl::TypeList<int>, tl::TypeList<int&>>, tl::TypeList, tl::TypeList<int, int&>)
)
{
	STATIC_REQUIRE(std::same_as<Expected, typename tl::zip_as<TargetContainer, Lists...>::type>);
	STATIC_REQUIRE(std::same_as<Expected, tl::zip_as_t<TargetContainer, Lists...>>);
}

TEMPLATE_TEST_CASE_SIG(
	"type_list::zip creates a common_container of common_container with elements of the provides lists.",
	"[type_list]",
	((bool dummy, class Expected, class... Lists), dummy, Expected, Lists...),
	// (true, tl::TypeList<>),
	(true, tl::TypeList<>, tl::TypeList<>),
	(true, tl::TypeList<tl::TypeList<int>>, tl::TypeList<int>),
	(true, tl::TypeList<tl::TypeList<int>, tl::TypeList<int&>>, tl::TypeList<int, int&>)
)
{
	STATIC_REQUIRE(std::same_as<Expected, typename tl::zip<Lists...>::type>);
	STATIC_REQUIRE(std::same_as<Expected, tl::zip_t<Lists...>>);
}

TEMPLATE_TEST_CASE_SIG(
	"type_list::cartesian_product_as creates a TargetContainer of TargetContainer with elements of the provides lists.",
	"[type_list]",
	((bool dummy, class Expected, template <class...> class TargetContainer, class... Lists),
		dummy, Expected, TargetContainer, Lists...),
	//(true, tl::TypeList<>, tl::TypeList),
	(true, tl::TypeList<>, tl::TypeList, tl::TypeList<>),
	(true, tl::TypeList<tl::TypeList<int>>, tl::TypeList, tl::TypeList<int>),
	(true, tl::TypeList<tl::TypeList<int>, tl::TypeList<float>>, tl::TypeList, tl::TypeList<int, float>),
	(true, tl::TypeList<>, tl::TypeList, tl::TypeList<int, float>, tl::TypeList<>),
	(true, tl::TypeList<tl::TypeList<int>, tl::TypeList<tl::TypeList<float>>>, tl::TypeList, tl::TypeList<int, tl::TypeList<float>>),
	(true, tl::TypeList<
		tl::TypeList<int, double>, tl::TypeList<tl::TypeList<float>, double>
		>, tl::TypeList, tl::TypeList<int, tl::TypeList<float>>, tl::TypeList<double>),
	(true, tl::TypeList<
		tl::TypeList<int, float>, tl::TypeList<int&, float>
		>, tl::TypeList, tl::TypeList<int, int&>, tl::TypeList<float>),
	(true, (tl::TypeList<
		tl::TypeList<int, float>, tl::TypeList<int, float&>, tl::TypeList<int&, float>, tl::TypeList<int&, float&>
		>), tl::TypeList, tl::TypeList<int, int&>, tl::TypeList<float, float&>),
	(true, (tl::TypeList<
		tl::TypeList<int, double, float>,
		tl::TypeList<int, double, float&>,
		tl::TypeList<int&, double, float>,
		tl::TypeList<int&, double, float&>
		>), tl::TypeList, tl::TypeList<int, int&>, tl::TypeList<double>, tl::TypeList<float, float&>),
	(true, (std::tuple<
		std::tuple<int, float>, std::tuple<int, float&>, std::tuple<int&, float>, std::tuple<int&, float&>
		>), std::tuple, tl::TypeList<int, int&>, std::tuple<float, float&>)
)
{
	STATIC_REQUIRE(std::same_as<Expected, typename tl::cartesian_product_as<TargetContainer, Lists...>::type>);
	STATIC_REQUIRE(std::same_as<Expected, tl::cartesian_product_as_t<TargetContainer, Lists...>>);
}

TEMPLATE_TEST_CASE_SIG(
	"type_list::cartesian_product a common_container of common_container with elements with elements of the provides lists.",
	"[type_list]",
	((bool dummy, class Expected, class... Lists), dummy, Expected, Lists...),
	(true, tl::TypeList<tl::TypeList<int, std::tuple<int&>>>, tl::TypeList<int>, tl::TypeList<std::tuple<int&>>),
	(true, std::tuple<std::tuple<int, tl::TypeList<int&>>>, std::tuple<int>, std::tuple<tl::TypeList<int&>>)
)
{
	STATIC_REQUIRE(std::same_as<Expected, typename tl::cartesian_product<Lists...>::type>);
	STATIC_REQUIRE(std::same_as<Expected, tl::cartesian_product_t<Lists...>>);
}

TEMPLATE_TEST_CASE_SIG(
	"type_list::difference_as determines which elements of first are not contained in second and yields the result as TargetContainer.",
	"[type_list]",
	((bool dummy, class Expected, template <class...> class TargetContainer, class FirstList, class SecondList),
		dummy, Expected, TargetContainer, FirstList, SecondList),
	(true, tl::TypeList<>, tl::TypeList, tl::TypeList<>, tl::TypeList<>),
	(true, tl::TypeList<>, tl::TypeList, tl::TypeList<>, tl::TypeList<int>),
	(true, tl::TypeList<int>, tl::TypeList, tl::TypeList<int>, tl::TypeList<>),
	(true, tl::TypeList<>, tl::TypeList, tl::TypeList<int>, tl::TypeList<int>),
	(true, tl::TypeList<>, tl::TypeList, tl::TypeList<double, int>, tl::TypeList<int, double>),
	(true, std::tuple<int&>, std::tuple, tl::TypeList<double, int&, int>, std::tuple<int, double>),
	(true, tl::TypeList<int&>, tl::TypeList, tl::TypeList<double, int&, int>, std::tuple<int, double>)
)
{
	STATIC_REQUIRE(tl::unordered_equal_v<Expected, typename tl::difference_as<TargetContainer, FirstList, SecondList>::type>);
	STATIC_REQUIRE(tl::unordered_equal_v<Expected, tl::difference_as_t<TargetContainer, FirstList, SecondList>>);

	using ResultContainer = tl::common_container<tl::difference_as_t<TargetContainer, FirstList, SecondList>>;
	STATIC_REQUIRE(std::same_as<TargetContainer<>, typename ResultContainer::template type<>>);
}

TEMPLATE_TEST_CASE_SIG(
	"type_list::difference determines which elements of first are not contained in second and yields the result as common_container.",
	"[type_list]",
	((bool dummy, class Expected, class FirstList, class SecondList), dummy, Expected, FirstList, SecondList),
	(true, tl::TypeList<>, tl::TypeList<>, tl::TypeList<>),
	(true, tl::TypeList<>, tl::TypeList<>, tl::TypeList<int>),
	(true, tl::TypeList<int>, tl::TypeList<int>, tl::TypeList<>),
	(true, tl::TypeList<>, tl::TypeList<int>, tl::TypeList<int>),
	(true, tl::TypeList<>, tl::TypeList<double, int>, tl::TypeList<int, double>),
	(true, tl::TypeList<int&>, tl::TypeList<double, int&, int>, tl::TypeList<int, double>),
	(true, std::tuple<int&>, std::tuple<double, int&, int>, std::tuple<int, double>)
)
{
	STATIC_REQUIRE(tl::unordered_equal_v<Expected, typename tl::difference<FirstList, SecondList>::type>);
	STATIC_REQUIRE(tl::unordered_equal_v<Expected, tl::difference_t<FirstList, SecondList>>);

	using ResultContainer = tl::common_container<tl::difference_t<FirstList, SecondList>>;
	STATIC_REQUIRE(std::same_as<typename tl::common_container<Expected>::template type<>, typename ResultContainer::template type<>>);
}

TEMPLATE_TEST_CASE_SIG(
	"type_list::symmetric_difference_as determines which elements are contained in either type-list but not in both and yields the result as TargetContainer.",
	"[type_list]",
	((bool dummy, class Expected, template <class...> class TargetContainer, class FirstList, class SecondList),
		dummy, Expected, TargetContainer, FirstList, SecondList),
	(true, tl::TypeList<>, tl::TypeList, tl::TypeList<>, tl::TypeList<>),
	(true, tl::TypeList<int>, tl::TypeList, tl::TypeList<>, tl::TypeList<int>),
	(true, tl::TypeList<>, tl::TypeList, tl::TypeList<int>, tl::TypeList<int>),
	(true, tl::TypeList<>, tl::TypeList, tl::TypeList<double, int>, tl::TypeList<int, double>),
	(true, std::tuple<int&>, std::tuple, tl::TypeList<double, int&, int>, std::tuple<int, double>),
	(true, tl::TypeList<int&>, tl::TypeList, tl::TypeList<double, int&, int>, std::tuple<int, double>)
)
{
	STATIC_REQUIRE(
		tl::unordered_equal_v<Expected,
		typename tl::symmetric_difference_as<TargetContainer,
		FirstList,
		SecondList>::type>
	);
	STATIC_REQUIRE(tl::unordered_equal_v<Expected, tl::symmetric_difference_as_t<TargetContainer, FirstList, SecondList>>);

	STATIC_REQUIRE(
		tl::unordered_equal_v<Expected,
		typename tl::symmetric_difference_as<TargetContainer,
		SecondList,
		FirstList>::type>
	);
	STATIC_REQUIRE(tl::unordered_equal_v<Expected, tl::symmetric_difference_as_t<TargetContainer, SecondList, FirstList>>);

	using ResultContainer = tl::common_container<tl::symmetric_difference_as_t<TargetContainer, FirstList, SecondList>>;
	STATIC_REQUIRE(std::same_as<TargetContainer<>, typename ResultContainer::template type<>>);
}

TEMPLATE_TEST_CASE_SIG(
	"type_list::symmetric_difference determines which elements are contained in either type-list but not in both and yields the result as common_container.",
	"[type_list]",
	((bool dummy, class Expected, class FirstList, class SecondList), dummy, Expected, FirstList, SecondList),
	(true, tl::TypeList<>, tl::TypeList<>, tl::TypeList<>),
	(true, tl::TypeList<int>, tl::TypeList<>, tl::TypeList<int>),
	(true, tl::TypeList<>, tl::TypeList<int>, tl::TypeList<int>),
	(true, tl::TypeList<>, tl::TypeList<double, int>, tl::TypeList<int, double>),
	(true, tl::TypeList<int&>, tl::TypeList<double, int&, int>, tl::TypeList<int, double>),
	(true, std::tuple<int&>, std::tuple<double, int&, int>, std::tuple<int, double>)
)
{
	STATIC_REQUIRE(tl::unordered_equal_v<Expected, typename tl::symmetric_difference<FirstList, SecondList>::type>);
	STATIC_REQUIRE(tl::unordered_equal_v<Expected, tl::symmetric_difference_t<FirstList, SecondList>>);

	STATIC_REQUIRE(tl::unordered_equal_v<Expected, typename tl::symmetric_difference<SecondList, FirstList>::type>);
	STATIC_REQUIRE(tl::unordered_equal_v<Expected, tl::symmetric_difference_t<SecondList, FirstList>>);

	using ResultContainer = tl::common_container<tl::symmetric_difference_t<FirstList, SecondList>>;
	STATIC_REQUIRE(std::same_as<typename tl::common_container<Expected>::template type<>, typename ResultContainer::template type<>>);
}

TEMPLATE_TEST_CASE_SIG(
	"type_list::intersection_as determines which types are contained in both type-lists and yields the result in the given TargetContainer.",
	"[type_list]",
	((bool dummy, class Expected, template <class...> class TargetContainer, class FirstList, class SecondList),
		dummy, Expected, TargetContainer, FirstList, SecondList),
	(true, tl::TypeList<>, tl::TypeList, tl::TypeList<>, tl::TypeList<>),
	(true, tl::TypeList<>, tl::TypeList, tl::TypeList<>, tl::TypeList<int>),
	(true, tl::TypeList<int>, tl::TypeList, tl::TypeList<int>, tl::TypeList<int>),
	(true, tl::TypeList<double, int>, tl::TypeList, tl::TypeList<double, int>, tl::TypeList<int, double>),
	(true, std::tuple<double, int>, std::tuple, tl::TypeList<double, int&, int>, std::tuple<int, double>),
	(true, tl::TypeList<double, int>, tl::TypeList, tl::TypeList<double, int&, int>, std::tuple<int, double>)
)
{
	STATIC_REQUIRE(tl::unordered_equal_v<Expected, typename tl::intersection_as<TargetContainer, FirstList, SecondList>::type>);
	STATIC_REQUIRE(tl::unordered_equal_v<Expected, tl::intersection_as_t<TargetContainer, FirstList, SecondList>>);

	STATIC_REQUIRE(tl::unordered_equal_v<Expected, typename tl::intersection_as<TargetContainer, SecondList, FirstList>::type>);
	STATIC_REQUIRE(tl::unordered_equal_v<Expected, tl::intersection_as_t<TargetContainer, SecondList, FirstList>>);

	using ResultContainer = tl::common_container<tl::intersection_as_t<TargetContainer, FirstList, SecondList>>;
	STATIC_REQUIRE(std::same_as<TargetContainer<>, typename ResultContainer::template type<>>);
}

TEMPLATE_TEST_CASE_SIG(
	"type_list::intersection determines which types are contained in both type-lists and yields the result in common_container.",
	"[type_list]",
	((bool dummy, class Expected, class FirstList, class SecondList), dummy, Expected, FirstList, SecondList),
	(true, tl::TypeList<>, tl::TypeList<>, tl::TypeList<>),
	(true, tl::TypeList<>, tl::TypeList<>, tl::TypeList<int>),
	(true, tl::TypeList<int>, tl::TypeList<int>, tl::TypeList<int>),
	(true, tl::TypeList<double, int>, tl::TypeList<double, int>, tl::TypeList<int, double>),
	(true, tl::TypeList<double, int>, tl::TypeList<double, int&, int>, tl::TypeList<int, double>),
	(true, std::tuple<double, int>, std::tuple<double, int&, int>, std::tuple<int, double>)
)
{
	STATIC_REQUIRE(tl::unordered_equal_v<Expected, typename tl::intersection<FirstList, SecondList>::type>);
	STATIC_REQUIRE(tl::unordered_equal_v<Expected, tl::intersection_t<FirstList, SecondList>>);

	STATIC_REQUIRE(tl::unordered_equal_v<Expected, typename tl::intersection<SecondList, FirstList>::type>);
	STATIC_REQUIRE(tl::unordered_equal_v<Expected, tl::intersection_t<SecondList, FirstList>>);

	using ResultContainer = tl::common_container<tl::intersection_t<FirstList, SecondList>>;
	STATIC_REQUIRE(std::same_as<typename tl::common_container<Expected>::template type<>, typename ResultContainer::template type<>>);
}
