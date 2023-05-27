//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/functional/mixins/Pipe.hpp"

#include <catch2/catch_template_test_macros.hpp>

#include "../Helper.hpp"

#include <optional>
#include <string>

#include "Simple-Utility/functional/Composition.hpp"
#include "Simple-Utility/functional/mixins/InvokePolicies.hpp"

namespace sf = sl::functional;

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
			std::forward_as_tuple(
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
			std::forward_as_tuple(
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

template <bool isNoexcept>
struct NoThrowInvokable
{
	template <class First>
	constexpr First operator ()(First&& first, [[maybe_unused]] auto&&...) const noexcept(isNoexcept)
	{
		return std::forward<First>(first);
	}
};

TEMPLATE_TEST_CASE_SIG(
	"functional::PipeStrategy propagates noexcept specification.",
	"[functional][functional::Pipe]",
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
	STATIC_REQUIRE(expected == std::is_nothrow_invocable_v<sf::PipeStrategy, std::tuple<Funs...>, int>);
}

struct TestFun
{
	constexpr auto operator ()(auto&&...) const noexcept
	{
	}
};

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

		sf::BasicClosure<decltype(firstFn), sf::BasicInvokePolicy, sf::PipeOperator> fun{std::move(firstFn)};
		const sf::Composition composedFun = std::move(fun) | std::move(secondFn);

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

		sf::BasicClosure<decltype(firstFn), sf::BasicInvokePolicy, sf::PipeOperator> fun{std::move(firstFn)}; // NOLINT(bugprone-use-after-move)
		sf::Composition composedFun = std::move(fun) | std::move(secondFn);            // NOLINT(bugprone-use-after-move)

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

		sf::BasicClosure<decltype(firstFn), sf::BasicInvokePolicy, sf::PipeOperator> fun{std::move(firstFn)}; // NOLINT(bugprone-use-after-move)
		const sf::Composition composedFun = std::move(fun) | std::move(secondFn);      // NOLINT(bugprone-use-after-move)

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

		sf::BasicClosure<decltype(firstFn), sf::BasicInvokePolicy, sf::PipeOperator> fun{std::move(firstFn)}; // NOLINT(bugprone-use-after-move)
		sf::Composition composedFun = std::move(fun) | std::move(secondFn);            // NOLINT(bugprone-use-after-move)

		const int result = std::invoke(std::move(composedFun), 42);

		REQUIRE(result == 42);
	}
}

template <bool isNoexceptCopyable, bool isNoexceptMovable>
struct NoThrowConstructible
{
	constexpr NoThrowConstructible() = default;

	constexpr NoThrowConstructible(const NoThrowConstructible&) noexcept(isNoexceptCopyable) = default;
	constexpr NoThrowConstructible& operator =(const NoThrowConstructible&) noexcept(isNoexceptCopyable) = default;

	constexpr NoThrowConstructible(NoThrowConstructible&&) noexcept(isNoexceptMovable) = default;
	constexpr NoThrowConstructible& operator =(NoThrowConstructible&&) noexcept(isNoexceptMovable) = default;
};

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

	STATIC_REQUIRE((lhsNothrowCopyable && rhsNothrowCopyable) == noexcept(std::declval<const LhsClosure&>() | std::declval<const SecondFun&>()));
	STATIC_REQUIRE((lhsNothrowCopyable && rhsNothrowMovable) == noexcept(std::declval<const LhsClosure&>() | std::declval<SecondFun&&>()));
	STATIC_REQUIRE((lhsNothrowMovable && rhsNothrowCopyable) == noexcept(std::declval<LhsClosure&&>() | std::declval<const SecondFun&>()));
	STATIC_REQUIRE((lhsNothrowMovable && rhsNothrowMovable) == noexcept(std::declval<LhsClosure&&>() | std::declval<SecondFun&&>()));
}
