//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/functional/mixins/Conjunction.hpp"

#include <catch2/catch_template_test_macros.hpp>

#include "../Helper.hpp"

namespace sf = sl::functional;

TEMPLATE_TEST_CASE_SIG(
	"functional::ConjunctionStrategy invokes a single function with the input params.",
	"[functional][functional::ConjunctionStrategy]",
	((bool fnResult), fnResult),
	(true),
	(false)
)
{
	constexpr bool expected = fnResult;
	constexpr sf::ConjunctionStrategy strategy{};

	GenericOperation2Mock<bool, float, const std::string&> fun{};

	SECTION("Functional provided as const lvalue ref.")
	{
		REQUIRE_CALL(fun, call_const_lvalue_ref(1337.f, "1337"))
			.RETURN(fnResult);

		const bool result = strategy(std::forward_as_tuple(std::as_const(fun)), 1337.f, "1337");

		REQUIRE(expected == result);
	}

	SECTION("Functional provided as lvalue ref.")
	{
		REQUIRE_CALL(fun, call_lvalue_ref(1337.f, "1337"))
			.RETURN(fnResult);

		const bool result = strategy(std::forward_as_tuple(fun), 1337.f, "1337");

		REQUIRE(expected == result);
	}

	SECTION("Functional provided as const rvalue ref.")
	{
		REQUIRE_CALL(fun, call_const_rvalue_ref(1337.f, "1337"))
			.RETURN(fnResult);

		const bool result = strategy(std::forward_as_tuple(std::move(std::as_const(fun))), 1337.f, "1337");

		REQUIRE(expected == result);
	}

	SECTION("Functional provided as rvalue ref.")
	{
		REQUIRE_CALL(fun, call_rvalue_ref(1337.f, "1337"))
			.RETURN(fnResult);

		const bool result = strategy(std::forward_as_tuple(std::move(fun)), 1337.f, "1337");

		REQUIRE(expected == result);
	}
}

TEMPLATE_TEST_CASE_SIG(
	"functional::ConjunctionStrategy with two functions invokes all functions with the given arguments.",
	"[functional][functional::ConjunctionStrategy]",
	((bool firstResult, bool secondResult), firstResult, secondResult),
	(true, true),
	(true, false)/*,
	let this test via a separate test
	(false, true),
	(false, false)*/
)
{
	constexpr bool expected = firstResult && secondResult;
	constexpr sf::ConjunctionStrategy strategy{};

	GenericOperation1Mock<bool, int> first{};
	GenericOperation1Mock<bool, int> second{};

	trompeloeil::sequence seq{};

	SECTION("Functionals provided as const lvalue ref.")
	{
		REQUIRE_CALL(first, call_const_lvalue_ref(42))
		   .RETURN(firstResult)
		   .IN_SEQUENCE(seq);

		REQUIRE_CALL(second, call_const_lvalue_ref(42))
			.RETURN(secondResult)
			.IN_SEQUENCE(seq);

		const bool result = strategy(std::forward_as_tuple(std::as_const(first), std::as_const(second)), 42);

		REQUIRE(expected == result);
	}

	SECTION("Functionals provided as lvalue ref.")
	{
		REQUIRE_CALL(first, call_lvalue_ref(42))
			.RETURN(firstResult)
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(second, call_lvalue_ref(42))
			.RETURN(secondResult)
			.IN_SEQUENCE(seq);

		const bool result = strategy(std::forward_as_tuple(first, second), 42);

		REQUIRE(expected == result);
	}

	SECTION("Functionals provided as const rvalue ref.")
	{
		REQUIRE_CALL(first, call_const_rvalue_ref(42))
			.RETURN(firstResult)
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(second, call_const_rvalue_ref(42))
			.RETURN(secondResult)
			.IN_SEQUENCE(seq);

		const bool result = strategy(std::forward_as_tuple(std::move(std::as_const(first)),std::move(std::as_const(second))), 42);

		REQUIRE(expected == result);
	}

	SECTION("Functionals provided as rvalue ref.")
	{
		REQUIRE_CALL(first, call_rvalue_ref(42))
			.RETURN(firstResult)
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(second, call_rvalue_ref(42))
			.RETURN(secondResult)
			.IN_SEQUENCE(seq);

		const bool result = strategy(std::forward_as_tuple(std::move(first), std::move(second)), 42);

		REQUIRE(expected == result);
	}
}

TEST_CASE(
	"functional::ConjunctionStrategy with multiple functions invokes each function until false was returned.",
	"[functional][functional::ConjunctionStrategy]"
)
{
	constexpr sf::ConjunctionStrategy strategy{};

	trompeloeil::sequence seq{};

	SECTION("Given two functions.")
	{
		GenericOperation1Mock<bool, int> first{};
		GenericOperation1Mock<bool, int> second{};

		REQUIRE_CALL(first, call_lvalue_ref(42))
			.RETURN(false)
			.IN_SEQUENCE(seq);

		const bool result = strategy(std::forward_as_tuple(first, second), 42);

		REQUIRE(!result);
	}

	SECTION("Given three functions.")
	{
		GenericOperation1Mock<bool, int> first{};
		GenericOperation1Mock<bool, int> second{};
		GenericOperation1Mock<bool, int> three{};

		REQUIRE_CALL(first, call_lvalue_ref(42))
			.RETURN(true)
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(second, call_lvalue_ref(42))
			.RETURN(false)
			.IN_SEQUENCE(seq);

		const bool result = strategy(std::forward_as_tuple(first, second, three), 42);

		REQUIRE(!result);
	}
}
