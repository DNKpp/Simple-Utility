//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>

#include "Simple-Utility/functional/Tuple.hpp"

#include <optional>

using namespace sl::functional;

TEMPLATE_TEST_CASE_SIG(
	"get forwards tuple to the specific get implementation and returns as received.",
	"[functional][functional::tuple]",
	((class Query, class Expected, auto... values), Query, Expected, values...),
	(char, char&, 'c'),
	( int, int&, 'c', 42),
	(char, char&, 'c', 42)
)
{
	std::tuple tuple{values...};

	Expected e = tuple::get<Query>(tuple);

	REQUIRE(std::get<Query>(tuple) == e);
}

TEMPLATE_TEST_CASE_SIG(
	"get_at forwards tuple to the specific get implementation and returns as received.",
	"[functional][functional::tuple]",
	((std::size_t index, class Expected, auto... values), index, Expected, values...),
	(0, char&, 'c'),
	(1, int&, 'c', 42),
	(0, char&, 'c', 42)
)
{
	std::tuple tuple{values...};

	Expected e = tuple::get_at<index>(tuple);

	REQUIRE(std::get<index>(tuple) == e);
}

TEMPLATE_TEST_CASE_SIG(
	"reduce extracts elements from tuple",
	"[functional][functional::tuple]",
	((bool dummy, class Source, class Target, class... Args), dummy, Source, Target, Args...),
	(true, std::tuple<int, float, int&>, std::tuple<int&, float, int>, int&, float, int),
	(true, std::tuple<int, float, int&>, std::tuple<int&, float>, int&, float)
)
{
	STATIC_REQUIRE(std::same_as<Target, decltype(tuple::reduce<Args...>(std::declval<Source>()))>);
}

TEMPLATE_TEST_CASE_SIG(
	"concat combines elements from multiple tuples.",
	"[functional][functional::tuple]",
	((bool dummy, class Expected, class... Tuples), dummy, Expected, Tuples...),
	(true, std::tuple<int, float, int&>, const std::tuple<int, float, int&>&, std::tuple<>&),
	(true, std::tuple<int, float, int&>, std::tuple<>&&, const std::tuple<int, float, int&>&),
	(true, std::tuple<int, float>, const std::tuple<int, float>&)
)
{
	STATIC_REQUIRE(std::same_as<Expected, decltype(tuple::concat(std::declval<Tuples>()...))>);
}

TEMPLATE_TEST_CASE_SIG(
	"tie creates tuple from lvalue references.",
	"[functional][functional::tuple]",
	((bool dummy, class Expected, class... Args), dummy, Expected, Args...),
	(true, std::tuple<int&>, int&),
	(true, std::tuple<int&, float&, int&>, int&, float&, int&),
	(true, std::tuple<const int&>, const int&),
	(true, std::tuple<>)
)
{
	STATIC_REQUIRE(std::same_as<Expected, decltype(tuple::tie(std::declval<Args>()...))>);
}

TEST_CASE("make_from creates instances from provided tuples.", "[functional][functional::tuple]")
{
	std::tuple source{42};

	auto instance = std::invoke(tuple::make_from<std::optional<int>>, source);

	REQUIRE(instance == 42);
}

TEMPLATE_TEST_CASE_SIG(
	"Apply forwards tuple elements as given to std::apply.",
	"[functional][functional::tuple]",
	((auto... values), values...),
	('c'),
	(42, 'c')
)
{
	constexpr std::tuple tuple{values...};

	auto fn = envelop<Apply>(tuple::tie);

	REQUIRE(tuple == std::invoke(std::as_const(fn), tuple));
	REQUIRE(tuple == std::invoke(fn, tuple));
	REQUIRE(tuple == std::invoke(std::move(std::as_const(fn)), tuple));
	REQUIRE(tuple == std::invoke(std::move(fn), tuple));
}
