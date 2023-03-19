//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>

#include "Simple-Utility/functional/tuple.hpp"
#include "Simple-Utility/functional/compare.hpp"

using namespace sl::functional;

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG(
	"get forwards tuple to the specific get implementation and returns as received.",
	"[functional][tuple]",
	((class TQuery, class TExpected, auto... VValue), TQuery, TExpected, VValue...),
	(char, char&, 'c'),
	( int, int&, 'c', 42),
	(char, char&, 'c', 42)
)
#pragma warning(default: 26444)
{
	std::tuple tuple{ VValue... };

	TExpected e = tuple::get<TQuery>(tuple);

	REQUIRE(std::get<TQuery>(tuple) == e);
}

TEMPLATE_TEST_CASE_SIG(
	"get_at forwards tuple to the specific get implementation and returns as received.",
	"[functional][tuple]",
	((std::size_t VIndex, class TExpected, auto... VValue), VIndex, TExpected, VValue...),
	(0, char&, 'c'),
	(1, int&, 'c', 42),
	(0, char&, 'c', 42)
)
{
	std::tuple tuple{ VValue... };

	TExpected e = tuple::get_at<VIndex>(tuple);

	REQUIRE(std::get<VIndex>(tuple) == e);
}

TEMPLATE_TEST_CASE_SIG(
	"reduce extracts elements from tuple",
	"[functional][tuple]",
	((bool VDummy, class TSource, class TTarget, class... TArgs), VDummy, TSource, TTarget, TArgs...),
	(true, std::tuple<int, float, int&>, std::tuple<int&, float, int>, int&, float, int),
	(true, std::tuple<int, float, int&>, std::tuple<int&, float>, int&, float)
)
{
	STATIC_REQUIRE(std::same_as<TTarget, decltype(tuple::reduce<TArgs...>(std::declval<TSource>()))>);
}

TEMPLATE_TEST_CASE_SIG(
	"concat combines elements from multiple tuples.",
	"[functional][tuple]",
	((bool VDummy, class TExpected, class... TTuples), VDummy, TExpected, TTuples...),
	(true, std::tuple<int, float, int&>, const std::tuple<int, float, int&>&, std::tuple<>&),
	(true, std::tuple<int, float, int&>, std::tuple<>&&, const std::tuple<int, float, int&>&),
	(true, std::tuple<int, float>, const std::tuple<int, float>&)
)
{
	STATIC_REQUIRE(std::same_as<TExpected, decltype(tuple::concat(std::declval<TTuples>()...))>);
}

TEMPLATE_TEST_CASE_SIG(
	"tie creates tuple from lvalue references.",
	"[functional][tuple]",
	((bool VDummy, class TExpected, class... TArgs), VDummy, TExpected, TArgs...),
	(true, std::tuple<int&>, int&),
	(true, std::tuple<int&, float&, int&>, int&, float&, int&),
	(true, std::tuple<const int&>, const int&),
	(true, std::tuple<>)
)
{
	STATIC_REQUIRE(std::same_as<TExpected, decltype(tuple::tie(std::declval<TArgs>()...))>);
}

TEMPLATE_TEST_CASE_SIG(
	"apply forwards tuple elements as given to std::apply.",
	"[functional][tuple]",
	((auto... VValue), VValue...),
	('c'),
	(42, 'c')
)
{
	const std::tuple tuple{ VValue... };

	auto func = tuple::apply(tuple::tie | compare::equal << tuple);

	REQUIRE(func(tuple));
	REQUIRE(std::as_const(func)(tuple));
	REQUIRE(std::move(func)(tuple));
}
