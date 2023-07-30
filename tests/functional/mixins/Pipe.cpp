//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/functional/mixins/Pipe.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "../Helper.hpp"

#include <optional>
#include <string>

#include "Simple-Utility/functional/Composition.hpp"
#include "Simple-Utility/functional/mixins/InvokePolicies.hpp"

namespace sf = sl::functional;

// ReSharper disable CppClangTidyBugproneUseAfterMove

TEST_CASE(
	"functional::PipeStrategy invokes a single function with the input params.",
	"[functional][functional::Pipe]"
)
{
	constexpr sf::PipeStrategy strategy{};

	GenericOperation2Mock<int, float, const std::string&> fun{};
	std::optional<int> result{};

	SECTION("Functional provided as const lvalue ref.")
	{
		REQUIRE_CALL(fun, call_const_lvalue_ref(1337.f, "1337"))
			.RETURN(42);

		result = strategy(std::forward_as_tuple(std::as_const(fun)), 1337.f, "1337");
	}

	SECTION("Functional provided as lvalue ref.")
	{
		REQUIRE_CALL(fun, call_lvalue_ref(1337.f, "1337"))
			.RETURN(42);

		result = strategy(std::forward_as_tuple(fun), 1337.f, "1337");
	}

	SECTION("Functional provided as const rvalue ref.")
	{
		REQUIRE_CALL(fun, call_const_rvalue_ref(1337.f, "1337"))
			.RETURN(42);

		result = strategy(std::forward_as_tuple(std::move(std::as_const(fun))), 1337.f, "1337");
	}

	SECTION("Functional provided as rvalue ref.")
	{
		REQUIRE_CALL(fun, call_rvalue_ref(1337.f, "1337"))
			.RETURN(42);

		result = strategy(std::forward_as_tuple(std::move(fun)), 1337.f, "1337");
	}

	REQUIRE(result == 42);
}

TEST_CASE(
	"functional::PipeStrategy with two functions invokes the second one with the result of the first one.",
	"[functional][functional::Pipe]"
)
{
	constexpr sf::PipeStrategy strategy{};

	GenericOperation2Mock<int, float, const std::string&> first{};
	GenericOperation1Mock<int, int> second{};

	trompeloeil::sequence seq{};
	std::optional<int> result{};

	SECTION("Functionals provided as const lvalue ref.")
	{
		REQUIRE_CALL(first, call_const_lvalue_ref(1337.f, "1337"))
		   .RETURN(42)
		   .IN_SEQUENCE(seq);

		REQUIRE_CALL(second, call_const_lvalue_ref(42))
			.RETURN(-1)
			.IN_SEQUENCE(seq);

		result = strategy(std::forward_as_tuple(std::as_const(first), std::as_const(second)), 1337.f, "1337");
	}

	SECTION("Functionals provided as lvalue ref.")
	{
		REQUIRE_CALL(first, call_lvalue_ref(1337.f, "1337"))
		   .RETURN(42)
		   .IN_SEQUENCE(seq);

		REQUIRE_CALL(second, call_lvalue_ref(42))
			.RETURN(-1)
			.IN_SEQUENCE(seq);

		result = strategy(std::forward_as_tuple(first, second), 1337.f, "1337");
	}

	SECTION("Functionals provided as const rvalue ref.")
	{
		REQUIRE_CALL(first, call_const_rvalue_ref(1337.f, "1337"))
		   .RETURN(42)
		   .IN_SEQUENCE(seq);

		REQUIRE_CALL(second, call_const_rvalue_ref(42))
			.RETURN(-1)
			.IN_SEQUENCE(seq);

		result = strategy(
			std::forward_as_tuple(
				std::move(std::as_const(first)),
				std::move(std::as_const(second))),
			1337.f,
			"1337");
	}

	SECTION("Functionals provided as rvalue ref.")
	{
		REQUIRE_CALL(first, call_rvalue_ref(1337.f, "1337"))
		   .RETURN(42)
		   .IN_SEQUENCE(seq);

		REQUIRE_CALL(second, call_rvalue_ref(42))
			.RETURN(-1)
			.IN_SEQUENCE(seq);

		result = strategy(std::forward_as_tuple(std::move(first), std::move(second)), 1337.f, "1337");
	}

	REQUIRE(result == -1);
}

TEST_CASE(
	"functional::PipeStrategy with three (or more) functions invokes the next one with the result from the previous one.",
	"[functional][functional::Pipe]"
)
{
	constexpr sf::PipeStrategy strategy{};

	GenericOperation2Mock<int, float, const std::string&> first{};
	GenericOperation1Mock<int, int> second{};
	GenericOperation1Mock<std::string, int> third{};

	trompeloeil::sequence seq{};
	std::optional<std::string> result{};

	SECTION("Functionals provided as const lvalue ref.")
	{
		REQUIRE_CALL(first, call_const_lvalue_ref(1337.f, "1337"))
		   .RETURN(42)
		   .IN_SEQUENCE(seq);

		REQUIRE_CALL(second, call_const_lvalue_ref(42))
			.RETURN(-1)
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(third, call_const_lvalue_ref(-1))
			.RETURN(std::to_string(_1))
			.IN_SEQUENCE(seq);

		result = strategy(
			std::tie(
				std::as_const(first),
				std::as_const(second),
				std::as_const(third)),
			1337.f,
			"1337");
	}

	SECTION("Functionals provided as lvalue ref.")
	{
		REQUIRE_CALL(first, call_lvalue_ref(1337.f, "1337"))
		   .RETURN(42)
		   .IN_SEQUENCE(seq);

		REQUIRE_CALL(second, call_lvalue_ref(42))
			.RETURN(-1)
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(third, call_lvalue_ref(-1))
			.RETURN(std::to_string(_1))
			.IN_SEQUENCE(seq);

		result = strategy(
			std::tie(
				first,
				second,
				third),
			1337.f,
			"1337");
	}

	SECTION("Functionals provided as const rvalue ref.")
	{
		REQUIRE_CALL(first, call_const_rvalue_ref(1337.f, "1337"))
		   .RETURN(42)
		   .IN_SEQUENCE(seq);

		REQUIRE_CALL(second, call_const_rvalue_ref(42))
			.RETURN(-1)
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(third, call_const_rvalue_ref(-1))
			.RETURN(std::to_string(_1))
			.IN_SEQUENCE(seq);

		result = strategy(
			std::forward_as_tuple(
				std::move(std::as_const(first)),
				std::move(std::as_const(second)),
				std::move(std::as_const(third))),
			1337.f,
			"1337");
	}

	SECTION("Functionals provided as rvalue ref.")
	{
		REQUIRE_CALL(first, call_rvalue_ref(1337.f, "1337"))
		   .RETURN(42)
		   .IN_SEQUENCE(seq);

		REQUIRE_CALL(second, call_rvalue_ref(42))
			.RETURN(-1)
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(third, call_rvalue_ref(-1))
			.RETURN(std::to_string(_1))
			.IN_SEQUENCE(seq);

		result = strategy(
			std::forward_as_tuple(
				std::move(first),
				std::move(second),
				std::move(third)),
			1337.f,
			"1337");
	}

	REQUIRE(result == "-1");
}

struct TestFun
{
	constexpr auto operator ()(auto&&...) const noexcept
	{
	}
};

template <class Fn>
using TestClosure = sf::BasicClosure<Fn, sf::BasicInvokePolicy, sf::PipeOperator>;

TEST_CASE("functional::PipeOperator enables enables pipe composing for functional::BasicClosure.", "[functional][functional::Pipe]")
{
	GenericOperation1Mock<std::string, int> firstFn{};
	GenericOperation1Mock<int, const std::string&> secondFn{};

	trompeloeil::sequence seq{};

	SECTION("Invoking as const lvalue ref.")
	{
		REQUIRE_CALL(firstFn, call_const_lvalue_ref(trompeloeil::_))
			.RETURN(std::to_string(_1))
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(secondFn, call_const_lvalue_ref(trompeloeil::_))
			.RETURN(std::stoi(_1))
			.IN_SEQUENCE(seq);

		const sf::BasicClosure composedFun = sf::envelop<TestClosure>(std::move(firstFn))
											| std::move(secondFn);

		const int result = std::invoke(composedFun, 42);

		REQUIRE(result == 42);
	}

	SECTION("Invoking as lvalue ref.")
	{
		REQUIRE_CALL(firstFn, call_lvalue_ref(trompeloeil::_))
			.RETURN(std::to_string(_1))
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(secondFn, call_lvalue_ref(trompeloeil::_))
			.RETURN(std::stoi(_1))
			.IN_SEQUENCE(seq);

		sf::BasicClosure composedFun = sf::envelop<TestClosure>(std::move(firstFn))
										| std::move(secondFn);

		const int result = std::invoke(composedFun, 42);

		REQUIRE(result == 42);
	}

	SECTION("Invoking as const rvalue ref.")
	{
		REQUIRE_CALL(firstFn, call_const_rvalue_ref(trompeloeil::_))
			.RETURN(std::to_string(_1))
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(secondFn, call_const_rvalue_ref(trompeloeil::_))
			.RETURN(std::stoi(_1))
			.IN_SEQUENCE(seq);

		const sf::BasicClosure composedFun = sf::envelop<TestClosure>(std::move(firstFn))
											| std::move(secondFn);

		const int result = std::invoke(std::move(composedFun), 42);

		REQUIRE(result == 42);
	}

	SECTION("Invoking as rvalue ref.")
	{
		REQUIRE_CALL(firstFn, call_rvalue_ref(trompeloeil::_))
			.RETURN(std::to_string(_1))
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(secondFn, call_rvalue_ref(trompeloeil::_))
			.RETURN(std::stoi(_1))
			.IN_SEQUENCE(seq);

		sf::BasicClosure composedFun = sf::envelop<TestClosure>(std::move(firstFn))
										| std::move(secondFn);

		const int result = std::invoke(std::move(composedFun), 42);

		REQUIRE(result == 42);
	}
}

TEST_CASE(
	"functional::PipeOperator enables pipe composing for functional::BasicClosure in arbitrary depth.",
	"[functional][functional::Pipe]"
)
{
	SECTION("Three functions.")
	{
		GenericOperation1Mock<std::string, int> firstFn{};
		GenericOperation1Mock<int, const std::string&> secondFn{};
		GenericOperation1Mock<int, int> thirdFn{};

		using Expected = sf::BasicClosure<
					sf::Composition<
						sf::PipeStrategy,
						GenericOperation1Mock<std::string, int>,
						GenericOperation1Mock<int, const std::string&>,
						GenericOperation1Mock<int, int>>,
					sf::BasicInvokePolicy,
					sf::PipeOperator>;

		trompeloeil::sequence seq{};
		REQUIRE_CALL(firstFn, call_const_lvalue_ref(trompeloeil::_))
			.RETURN(std::to_string(_1))
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(secondFn, call_const_lvalue_ref(trompeloeil::_))
			.RETURN(std::stoi(_1))
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(thirdFn, call_const_lvalue_ref(trompeloeil::_))
			.RETURN(2 * _1)
			.IN_SEQUENCE(seq);

		const auto composedFn = sf::envelop<TestClosure>(std::move(firstFn))
								| std::move(secondFn)
								| std::move(thirdFn);

		const auto [expected, input] = GENERATE(
			(table<int, int>)({
				{2 * 42, 42},
				{2 * 1337, 1337}
			}));

		const int result = std::invoke(composedFn, input);

		REQUIRE(expected == result);
		STATIC_REQUIRE(std::same_as<std::remove_const_t<decltype(composedFn)>, Expected>);
	}
}

template <class Fn>
using TestClosure2 = sf::BasicClosure<Fn, sf::BasicInvokePolicy>;
TEMPLATE_TEST_CASE_SIG(
	"functional::PipeOperator propagates different closure types, depending on the operands.",
	"[functional][functional::Pipe]",
	((bool dummy, template <class> class ExpectedClosure, class Rhs), dummy, ExpectedClosure, Rhs),
	(true, TestClosure, std::string(*)(int)),
	(true, TestClosure, TestClosure<std::string(*)(int)>),
	(true, TestClosure2, TestClosure2<std::string(*)(int)>)
)
{
	using Expected = ExpectedClosure<sf::Composition<sf::PipeStrategy, int(*)(bool), std::string(*)(int)>>;
	using Lhs = TestClosure<int(*)(bool)>;

	SECTION("Lhs as const lvalue-reference; Rhs as const lvalue-reference")
	{
		using Result = decltype(std::declval<const Lhs&>() | std::declval<const Rhs&>());
		STATIC_REQUIRE(std::same_as<Expected, Result>);
	}
		
	SECTION("Lhs as rvalue-reference; Rhs as const lvalue-reference")
	{
		using Result = decltype(std::declval<Lhs&&>() | std::declval<const Rhs&>());
		STATIC_REQUIRE(std::same_as<Expected, Result>);
	}

	SECTION("Lhs as const lvalue-reference; Rhs as rvalue-reference")
	{
		using Result = decltype(std::declval<const Lhs&>() | std::declval<Rhs&&>());
		STATIC_REQUIRE(std::same_as<Expected, Result>);
	}

	SECTION("Lhs as rvalue-reference; Rhs as rvalue-reference")
	{
		using Result = decltype(std::declval<Lhs&&>() | std::declval<Rhs&&>());
		STATIC_REQUIRE(std::same_as<Expected, Result>);
	}
}

TEMPLATE_TEST_CASE_SIG(
	"functional::PipeOperator propagates noexcept specification during composing.",
	"[functional][functional::Pipe]",
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
	using LhsClosure = sf::BasicClosure<FirstFun, sf::BasicInvokePolicy, sf::PipeOperator>;

	STATIC_REQUIRE(
		(lhsNothrowCopyable && rhsNothrowCopyable) == noexcept(std::declval<const LhsClosure&>() | std::declval<const SecondFun&>()));
	STATIC_REQUIRE(
		(lhsNothrowCopyable && rhsNothrowMovable) == noexcept(std::declval<const LhsClosure&>() | std::declval<SecondFun&&>()));
	STATIC_REQUIRE(
		(lhsNothrowMovable && rhsNothrowCopyable) == noexcept(std::declval<LhsClosure&&>() | std::declval<const SecondFun&>()));
	STATIC_REQUIRE((lhsNothrowMovable && rhsNothrowMovable) == noexcept(std::declval<LhsClosure&&>() | std::declval<SecondFun&&>()));
}
