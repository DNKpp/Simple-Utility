//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/functional/mixins/Conjunction.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "../Helper.hpp"

#include "Simple-Utility/functional/mixins/InvokePolicies.hpp"

namespace sf = sl::functional;

// ReSharper disable CppClangTidyBugproneUseAfterMove

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

template <class Fn>
using TestClosure = sf::BasicClosure<Fn, sf::BasicInvokePolicy, sf::ConjunctionOperator>;

TEST_CASE(
	"functional::ConjunctionOperator enables && composing for functional::BasicClosure.",
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

		const sf::BasicClosure composedFun = sf::envelop<TestClosure>(std::move(firstFn))
											&& std::move(secondFn);

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

		sf::BasicClosure composedFun = sf::envelop<TestClosure>(std::move(firstFn))
										&& std::move(secondFn);

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

		const sf::BasicClosure composedFun = sf::envelop<TestClosure>(std::move(firstFn))
											&& std::move(secondFn);

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

		sf::BasicClosure composedFun = sf::envelop<TestClosure>(std::move(firstFn))
										&& std::move(secondFn);

		const bool result = std::invoke(std::move(composedFun), 42);

		REQUIRE(!result);
	}
}

TEST_CASE(
	"functional::ConjunctionOperator enables && composing for functional::BasicClosure in arbitrary depth.",
	"[functional][functional::Conjunction]"
)
{
	SECTION("Three functions.")
	{
		GenericOperation1Mock<bool, int> firstFn{};
		GenericOperation1Mock<bool, int> secondFn{};
		GenericOperation1Mock<bool, int> thirdFn{};

		using Expected = sf::BasicClosure<
					sf::Composition<
						sf::ConjunctionStrategy,
						GenericOperation1Mock<bool, int>,
						GenericOperation1Mock<bool, int>,
						GenericOperation1Mock<bool, int>>,
					sf::BasicInvokePolicy,
					sf::ConjunctionOperator>;

		trompeloeil::sequence seq{};
		REQUIRE_CALL(firstFn, call_const_lvalue_ref(trompeloeil::_))
			.RETURN(_1 < 45)
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(secondFn, call_const_lvalue_ref(trompeloeil::_))
			.RETURN(41 < _1)
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(thirdFn, call_const_lvalue_ref(trompeloeil::_))
			.RETURN(44 != _1)
			.IN_SEQUENCE(seq);

		const sf::BasicClosure composedFn = sf::envelop<TestClosure>(std::move(firstFn))
								&& std::move(secondFn)
								&& std::move(thirdFn);

		const auto [expected, input] = GENERATE(
			(table<bool, int>)({
				{true, 42},
				{true, 43},
				{false, 44}	// third predicate fails
			}));

		const bool result = std::invoke(composedFn, input);

		REQUIRE(expected == result);
		STATIC_REQUIRE(std::same_as<std::remove_const_t<decltype(composedFn)>, Expected>);
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
	using LhsClosure = TestClosure<FirstFun>;

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
