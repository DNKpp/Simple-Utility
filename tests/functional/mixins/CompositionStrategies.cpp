//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/functional/mixins/Conjunction.hpp"
#include "Simple-Utility/functional/mixins/Disjunction.hpp"
#include "Simple-Utility/functional/mixins/Equality.hpp"

#include <catch2/catch_template_test_macros.hpp>

#include "../Helper.hpp"

#include "Simple-Utility/functional/mixins/InvokePolicies.hpp"

namespace sf = sl::functional;

// ReSharper disable CppClangTidyBugproneUseAfterMove

TEMPLATE_TEST_CASE_SIG(
	"Composition strategies invoke a single function with the input params.",
	"[functional][functional::CompositionStrategy]",
	((bool expected, class Strategy, bool fnResult), expected, Strategy, fnResult),
	(true, sf::ConjunctionStrategy, true),
	(false, sf::ConjunctionStrategy, false),
	(true, sf::DisjunctionStrategy, true),
	(false, sf::DisjunctionStrategy, false),
	(true, sf::EqualityStrategy, true),
	(true, sf::EqualityStrategy, false)
)
{
	constexpr Strategy strategy{};

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
	"Composition strategies invoke two functions with the given arguments.",
	"[functional][functional::CompositionStrategy]",
	((bool expected, class Strategy, bool firstResult, bool secondResult), expected, Strategy, firstResult, secondResult),
	(true, sf::ConjunctionStrategy, true, true),
	(false, sf::ConjunctionStrategy, true, false),
	/*,
	These tests will return early, thus make it a separate case.
	(false, sf::ConjunctionStrategy, false, true),
	(false, sf::ConjunctionStrategy, false, false),
	*/
	(false, sf::DisjunctionStrategy, false, false),
	(true, sf::DisjunctionStrategy, false, true),
	/*,
	These tests will return early, thus make it a separate case.
	(true, sf::DisjunctionStrategy, true, true),
	(true, sf::DisjunctionStrategy, true, false),
	*/	
	(true, sf::EqualityStrategy, true, true),
	(false, sf::EqualityStrategy, true, false),
	(false, sf::EqualityStrategy, false, true),
	(true, sf::EqualityStrategy, false, false)
)
{
	constexpr Strategy strategy{};

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

		const bool result = strategy(std::tie(std::as_const(first), std::as_const(second)), 42);

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

		const bool result = strategy(std::tie(first, second), 42);

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

		const bool result = strategy(std::forward_as_tuple(std::move(std::as_const(first)), std::move(std::as_const(second))), 42);

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

TEMPLATE_TEST_CASE(
	"Composition strategies propagate noexcept specification for invocation.",
	"[functional][functional::CompositionStrategy]",
	sf::ConjunctionStrategy,
	sf::DisjunctionStrategy,
	sf::EqualityStrategy
)
{
	STATIC_REQUIRE(std::is_nothrow_invocable_v<TestType, std::tuple<NoThrowInvokable<true>>, int>);
	STATIC_REQUIRE(!std::is_nothrow_invocable_v<TestType, std::tuple<NoThrowInvokable<false>>, int>);

	STATIC_REQUIRE(std::is_nothrow_invocable_v<TestType, std::tuple<NoThrowInvokable<true>, NoThrowInvokable<true>>, int>);
	STATIC_REQUIRE(!std::is_nothrow_invocable_v<TestType, std::tuple<NoThrowInvokable<false>, NoThrowInvokable<true>>, int>);
	STATIC_REQUIRE(!std::is_nothrow_invocable_v<TestType, std::tuple<NoThrowInvokable<true>, NoThrowInvokable<false>>, int>);
	STATIC_REQUIRE(!std::is_nothrow_invocable_v<TestType, std::tuple<NoThrowInvokable<false>, NoThrowInvokable<false>>, int>);

	STATIC_REQUIRE(
		std::is_nothrow_invocable_v<TestType,
		std::tuple<NoThrowInvokable<true>,
		NoThrowInvokable<true>,
		NoThrowInvokable<true>>,
		int>);
	STATIC_REQUIRE(
		!std::is_nothrow_invocable_v<TestType,
		std::tuple<NoThrowInvokable<false>,
		NoThrowInvokable<true>,
		NoThrowInvokable<true>>,
		int>);
	STATIC_REQUIRE(
		!std::is_nothrow_invocable_v<TestType,
		std::tuple<NoThrowInvokable<true>,
		NoThrowInvokable<false>,
		NoThrowInvokable<true>>,
		int>);
	STATIC_REQUIRE(
		!std::is_nothrow_invocable_v<TestType,
		std::tuple<NoThrowInvokable<false>,
		NoThrowInvokable<false>,
		NoThrowInvokable<true>>,
		int>);
	STATIC_REQUIRE(
		!std::is_nothrow_invocable_v<TestType,
		std::tuple<NoThrowInvokable<true>,
		NoThrowInvokable<true>,
		NoThrowInvokable<false>>,
		int>);
	STATIC_REQUIRE(
		!std::is_nothrow_invocable_v<TestType,
		std::tuple<NoThrowInvokable<false>,
		NoThrowInvokable<true>,
		NoThrowInvokable<false>>,
		int>);
	STATIC_REQUIRE(
		!std::is_nothrow_invocable_v<TestType,
		std::tuple<NoThrowInvokable<true>,
		NoThrowInvokable<false>,
		NoThrowInvokable<false>>,
		int>);
	STATIC_REQUIRE(
		!std::is_nothrow_invocable_v<TestType,
		std::tuple<NoThrowInvokable<false>,
		NoThrowInvokable<false>,
		NoThrowInvokable<false>>,
		int>);
}
