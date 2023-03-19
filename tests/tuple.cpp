//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>

#include "Simple-Utility/tuple.hpp"

#include <array>

namespace matchers = Catch::Matchers;

using namespace sl;
using namespace tuple;

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

	STATIC_REQUIRE(concepts::apply_invocable<function_t, TTuple> == VExpected);
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
	[[maybe_unused]] static constexpr auto func = []<class T>(T&&) noexcept(std::same_as<int, T>)
	{
	};
	using function_t = decltype(func);

	STATIC_REQUIRE(concepts::nothrow_apply_invocable<function_t, TTuple> == VExpected);
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
	STATIC_REQUIRE(concepts::tuple_like<TTuple> == VExpected);
}

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
	[[maybe_unused]] static constexpr auto getFront = []<class TFront, class... TIgnored>(TFront&& front, TIgnored&&...) -> TFront
	{
		return std::forward<TFront>(front);
	};
	using get_front_t = decltype(getFront);

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
	[[maybe_unused]] static constexpr auto func = []<class T>(T&&) noexcept(std::same_as<int, T>)
	{
	};
	using function_t = decltype(func);

	STATIC_REQUIRE(is_nothrow_apply_invocable<function_t, TTuple>::value == VExpected);
	STATIC_REQUIRE(is_nothrow_apply_invocable_v<function_t, TTuple> == VExpected);
}

TEMPLATE_TEST_CASE_SIG(
	"envelop_elements_result_t yields return type of envelop_elements algorithm.",
	"[tuple][trait]",
	((bool VDummy, class TResult, class TTuple), VDummy, TResult, TTuple),
	(true, std::tuple<>, std::tuple<>),
	(true, std::tuple<std::tuple<int>>, std::tuple<int>),
	(true, std::tuple<std::tuple<int>, std::tuple<float>>, std::tuple<int, float>),
	(true, std::tuple<std::tuple<std::tuple<int>>, std::tuple<int>>, std::tuple<std::tuple<int>, int>)
)
{
	STATIC_REQUIRE(std::same_as<TResult, envelop_elements_result_t<TTuple>>);
}

TEST_CASE("envelop_elements creates new tuple from source.", "[tuple][algorithm]")
{
	std::tuple<std::string, std::string> tuple{"Hello, World!", "Test"};

	using result_t = std::tuple<
		std::tuple<std::string>,
		std::tuple<std::string>
	>;
	const result_t expectedResult{{"Hello, World!"}, {"Test"}};
	result_t result{};

	SECTION("copy from source")
	{
		result = envelop_elements(tuple);

		REQUIRE_THAT(std::get<0>(tuple), matchers::Equals("Hello, World!"));
		REQUIRE_THAT(std::get<1>(tuple), matchers::Equals("Test"));
	}

	SECTION("move from source")
	{
		result = envelop_elements(std::move(tuple));

		REQUIRE_THAT(std::get<0>(tuple), matchers::IsEmpty());
		REQUIRE_THAT(std::get<1>(tuple), matchers::IsEmpty());
	}

	REQUIRE(result == expectedResult);
}

TEMPLATE_TEST_CASE_SIG(
	"zip_result_t yields return type of zip algorithm.",
	"[tuple][trait]",
	((bool VDummy, class TResult, class... TTuples), VDummy, TResult, TTuples...),
	(true, std::tuple<>, std::tuple<>, std::tuple<int>),
	(true, std::tuple<std::tuple<int, int>>, std::tuple<int>, std::tuple<int>),
	(true, std::tuple<std::tuple<int, int>>, std::tuple<int, float>, std::tuple<int>),
	(true, std::tuple<std::tuple<int, int>, std::tuple<float, int>>, std::tuple<int, float>, std::tuple<int, int>),
	(true, std::tuple<std::tuple<int, float, std::string>>, std::tuple<int>, std::tuple<float>, std::tuple<std::string, short>)
)
{
	STATIC_REQUIRE(std::same_as<TResult, zip_result_t<TTuples...>>);
}

TEST_CASE("zip_result_t yields return type when combined with types other than std::tuple.", "[tuple][trait]")
{
	using expected_result_t = std::tuple<std::tuple<int, std::string>, std::tuple<float, short>>;
	using result_t = zip_result_t<std::pair<int, float>, std::tuple<std::string, short>>;

	STATIC_REQUIRE(std::same_as<result_t, expected_result_t>);
}

TEST_CASE("zip zips multiple tuples into one.", "[tuple][algorithm]")
{
	std::tuple first{1337, 42};
	std::tuple<std::string, int> second{"Hello, World!", -42};
	std::tuple<short, int, std::string> third{8, -1337, "World, Hello!"};

	using result_t = std::tuple<
		std::tuple<int, std::string, short>,
		std::tuple<int, int, int>
	>;
	const result_t expectedResult{{1337, "Hello, World!", 8}, {42, -42, -1337}};

	result_t result = zip(std::move(first), std::move(second), std::move(third));

	REQUIRE(result == expectedResult);
	REQUIRE_THAT(std::get<0>(second), matchers::IsEmpty());
}

TEMPLATE_TEST_CASE_SIG(
	"cartesian_product_result_t yields return type of cartesian_product algorithm.",
	"[tuple][trait]",
	((bool VDummy, class TResult, class... TTuples), VDummy, TResult, TTuples...),
	(true, std::tuple<>, std::tuple<>, std::tuple<>),
	(true, std::tuple<>, std::tuple<>, std::tuple<int>),
	(true, std::tuple<std::tuple<float, int>>, std::tuple<float>, std::tuple<int>),
	(true, std::tuple<std::tuple<float, int>, std::tuple<short, int>>, std::tuple<float, short>, std::tuple<int>),
	(true, (std::tuple<std::tuple<float, int>, std::tuple<float, short>, std::tuple<double, int>, std::tuple<double, short>>),
		std::tuple<float, double>, std::tuple<int, short>),
	(true, (std::tuple<std::tuple<float, int, short>, std::tuple<float, int, unsigned>,
			std::tuple<double, int, short>, std::tuple<double, int, unsigned>>),
		std::tuple<float, double>, std::tuple<int>, std::tuple<short, unsigned>)
)
{
	STATIC_REQUIRE(std::same_as<TResult, cartesian_product_result_t<TTuples...>>);
}

TEST_CASE("cartesian_product creates cartesian product of two tuples.", "[tuple][algorithm]")
{
	const std::tuple<std::string, short> first{"Hello, World!", 42};
	const std::tuple<unsigned, int, std::string> second{1337, -42, "Test"};

	using result_t = std::tuple<
		std::tuple<std::string, unsigned>,
		std::tuple<std::string, int>,
		std::tuple<std::string, std::string>,
		std::tuple<short, unsigned>,
		std::tuple<short, int>,
		std::tuple<short, std::string>
	>;

	// LCOV_EXCL_START
	const result_t expectedResult{
		{"Hello, World!", 1337},
		{"Hello, World!", -42},
		{"Hello, World!", "Test"},
		{42, 1337},
		{42, -42},
		{42, "Test"}
	};
	// LCOV_EXCL_STOP

	result_t result = cartesian_product(first, second);

	REQUIRE(result == expectedResult);
}

TEST_CASE("cartesian_product creates cartesian product of three tuples.", "[tuple][algorithm]")
{
	const std::tuple<std::string, short> first{"Hello, World!", 42};
	const std::tuple<unsigned, int, std::string> second{1337, -42, "Test"};
	const std::tuple<int, std::string> third{-1337, "Test2"};

	using result_t = std::tuple<
		std::tuple<std::string, unsigned, int>,
		std::tuple<std::string, unsigned, std::string>,
		std::tuple<std::string, int, int>,
		std::tuple<std::string, int, std::string>,
		std::tuple<std::string, std::string, int>,
		std::tuple<std::string, std::string, std::string>,

		std::tuple<short, unsigned, int>,
		std::tuple<short, unsigned, std::string>,
		std::tuple<short, int, int>,
		std::tuple<short, int, std::string>,
		std::tuple<short, std::string, int>,
		std::tuple<short, std::string, std::string>
	>;

	// LCOV_EXCL_START
	const result_t expectedResult{
		{"Hello, World!", 1337, -1337},
		{"Hello, World!", 1337, "Test2"},
		{"Hello, World!", -42, -1337},
		{"Hello, World!", -42, "Test2"},
		{"Hello, World!", "Test", -1337},
		{"Hello, World!", "Test", "Test2"},

		{42, 1337, -1337},
		{42, 1337, "Test2"},
		{42, -42, -1337},
		{42, -42, "Test2"},
		{42, "Test", -1337},
		{42, "Test", "Test2"}
	};
	// LCOV_EXCL_STOP

	result_t result = cartesian_product(first, second, third);

	REQUIRE(result == expectedResult);
}
