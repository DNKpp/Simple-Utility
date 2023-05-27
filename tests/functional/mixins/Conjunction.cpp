//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/functional/mixins/Conjunction.hpp"

#include <catch2/catch_template_test_macros.hpp>

#include "../Helper.hpp"

#include "Simple-Utility/functional/mixins/InvokePolicies.hpp"

namespace sf = sl::functional;

// ReSharper disable CppClangTidyBugproneUseAfterMove

TEMPLATE_TEST_CASE_SIG(
	"functional::ConjunctionStrategy invokes a single function with the input params.",
	"[functional][functional::Conjunction]",
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
	"[functional][functional::Conjunction]",
	((bool firstResult, bool secondResult), firstResult, secondResult),
	(true, true),
	(true, false) /*,
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

TEST_CASE(
	"functional::ConjunctionStrategy with multiple functions invokes each function until false was returned.",
	"[functional][functional::Conjunction]"
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

TEMPLATE_TEST_CASE_SIG(
	"functional::ConjunctionStrategy propagates noexcept specification.",
	"[functional][functional::Conjunction]",
	((bool expected, class... Funs), expected, Funs...),
	(true, NoThrowInvokable<true>),
	(true, NoThrowInvokable<true>, NoThrowInvokable<true>),
	(true, NoThrowInvokable<true>, NoThrowInvokable<true>, NoThrowInvokable<true>),
	(false, NoThrowInvokable<false>),
	(false, NoThrowInvokable<false>, NoThrowInvokable<true>),
	(false, NoThrowInvokable<true>, NoThrowInvokable<false>),
	(false, NoThrowInvokable<false>, NoThrowInvokable<false>),
	(false, NoThrowInvokable<false>, NoThrowInvokable<true>, NoThrowInvokable<true>),
	(false, NoThrowInvokable<true>, NoThrowInvokable<false>, NoThrowInvokable<true>),
	(false, NoThrowInvokable<true>, NoThrowInvokable<true>, NoThrowInvokable<false>)
)
{
	STATIC_REQUIRE(expected == std::is_nothrow_invocable_v<sf::ConjunctionStrategy, std::tuple<Funs...>, int>);
}

TEST_CASE(
	"functional::ConjunctionOperator enables enables && composing for functional::BasicClosure.",
	"[functional][functional::Conjunction]"
)
{
	GenericOperation1Mock<bool, int> firstFn{};
	GenericOperation1Mock<bool, int> secondFn{};

	trompeloeil::sequence seq{};

	SECTION("Invoking as const lvalue ref.")
	{
		REQUIRE_CALL(firstFn, call_const_lvalue_ref(42))
		   .RETURN(true)
		   .IN_SEQUENCE(seq);

		REQUIRE_CALL(secondFn, call_const_lvalue_ref(42))
			.RETURN(false)
			.IN_SEQUENCE(seq);

		sf::BasicClosure<decltype(firstFn), sf::BasicInvokePolicy, sf::ConjunctionOperator> fun{std::move(firstFn)};
		const sf::Composition composedFun = std::move(fun) && std::move(secondFn);

		const bool result = std::invoke(composedFun, 42);

		REQUIRE(!result);
	}

	SECTION("Invoking as lvalue ref.")
	{
		REQUIRE_CALL(firstFn, call_lvalue_ref(42))
			.RETURN(true)
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(secondFn, call_lvalue_ref(42))
			.RETURN(false)
			.IN_SEQUENCE(seq);

		sf::BasicClosure<decltype(firstFn), sf::BasicInvokePolicy, sf::ConjunctionOperator> fun{std::move(firstFn)};
		sf::Composition composedFun = std::move(fun) && std::move(secondFn);

		const bool result = std::invoke(composedFun, 42);

		REQUIRE(!result);
	}

	SECTION("Invoking as const rvalue ref.")
	{
		REQUIRE_CALL(firstFn, call_const_rvalue_ref(42))
			.RETURN(true)
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(secondFn, call_const_rvalue_ref(42))
			.RETURN(false)
			.IN_SEQUENCE(seq);

		sf::BasicClosure<decltype(firstFn), sf::BasicInvokePolicy, sf::ConjunctionOperator> fun{std::move(firstFn)};
		const sf::Composition composedFun = std::move(fun) && std::move(secondFn);

		const bool result = std::invoke(std::move(composedFun), 42);

		REQUIRE(!result);
	}

	SECTION("Invoking as rvalue ref.")
	{
		REQUIRE_CALL(firstFn, call_rvalue_ref(42))
			.RETURN(true)
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(secondFn, call_rvalue_ref(42))
			.RETURN(false)
			.IN_SEQUENCE(seq);

		sf::BasicClosure<decltype(firstFn), sf::BasicInvokePolicy, sf::ConjunctionOperator> fun{std::move(firstFn)};
		sf::Composition composedFun = std::move(fun) && std::move(secondFn);

		const bool result = std::invoke(std::move(composedFun), 42);

		REQUIRE(!result);
	}
}

TEMPLATE_TEST_CASE_SIG(
	"functional::ConjunctionOperator propagates noexcept specification during composing.",
	"[functional][functional::Conjunction]",
	((bool lhsNothrowCopyable, bool lhsNothrowMovable, bool rhsNothrowCopyable, bool rhsNothrowMovable),
		lhsNothrowCopyable, lhsNothrowMovable, rhsNothrowCopyable, rhsNothrowMovable),
	(true, true, true, true),
	(false, true, true, true),
	(true, true, false, true),
	(true, false, true, true),
	(true, true, true, false)
)
{
	using FirstFun = NoThrowConstructible<lhsNothrowCopyable, lhsNothrowMovable>;
	using SecondFun = NoThrowConstructible<rhsNothrowCopyable, rhsNothrowMovable>;
	using LhsClosure = sf::BasicClosure<FirstFun, sf::BasicInvokePolicy, sf::ConjunctionOperator>;

	constexpr bool b = std::is_nothrow_constructible_v<LhsClosure, const LhsClosure&>;

	static_assert(lhsNothrowCopyable == b);

	STATIC_REQUIRE(
		(lhsNothrowCopyable && rhsNothrowCopyable)
		== noexcept(std::declval<const LhsClosure&>() && std::declval<const SecondFun&>()));
	STATIC_REQUIRE(
		(lhsNothrowCopyable && rhsNothrowMovable)
		== noexcept(std::declval<const LhsClosure&>() && std::declval<SecondFun&&>()));
	STATIC_REQUIRE(
		(lhsNothrowMovable && rhsNothrowCopyable)
		== noexcept(std::declval<LhsClosure&&>() && std::declval<const SecondFun&>()));
	STATIC_REQUIRE(
		(lhsNothrowMovable && rhsNothrowMovable)
		== noexcept(std::declval<LhsClosure&&>() && std::declval<SecondFun&&>()));
}
