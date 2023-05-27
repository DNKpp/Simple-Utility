//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/functional/mixins/Negation.hpp"

#include <catch2/catch_template_test_macros.hpp>

#include "../Helper.hpp"

#include "Simple-Utility/functional/mixins/InvokePolicies.hpp"

namespace sf = sl::functional;

// ReSharper disable CppClangTidyBugproneUseAfterMove

template <class Fn>
using Closure = sf::BasicClosure<Fn, sf::BasicInvokePolicy, sf::NegationOperator>;

TEMPLATE_TEST_CASE_SIG(
	"functional::ConjunctionOperator enables negation via ! for functional::BasicClosure.",
	"[functional][functional::Negation]",
	((bool invokeResult), invokeResult),
	(true),
	(false)
)
{
	constexpr bool expected = !invokeResult;
	GenericOperation1Mock<bool, int> fn{};

	SECTION("Invoking as const lvalue ref.")
	{
		REQUIRE_CALL(fn, call_const_lvalue_ref(42))
			.RETURN(invokeResult);

		Closure<decltype(fn)> closureFn{std::move(fn)};
		sf::BasicClosure negation = !std::move(closureFn);

		const bool result = std::invoke(std::as_const(negation), 42);

		REQUIRE(expected == result);
	}

	SECTION("Invoking as lvalue ref.")
	{
		REQUIRE_CALL(fn, call_lvalue_ref(42))
			.RETURN(invokeResult);

		Closure<decltype(fn)> closureFn{std::move(fn)};
		sf::BasicClosure negation = !std::move(closureFn);

		const bool result = std::invoke(negation, 42);

		REQUIRE(expected == result);
	}

	SECTION("Invoking as const rvalue ref.")
	{
		REQUIRE_CALL(fn, call_const_rvalue_ref(42))
			.RETURN(invokeResult);

		Closure<decltype(fn)> closureFn{std::move(fn)};
		sf::BasicClosure negation = !std::move(closureFn);

		const bool result = std::invoke(std::move(std::as_const(negation)), 42);

		REQUIRE(expected == result);
	}

	SECTION("Invoking as rvalue ref.")
	{
		REQUIRE_CALL(fn, call_rvalue_ref(42))
			.RETURN(invokeResult);

		Closure<decltype(fn)> closureFn{std::move(fn)};
		sf::BasicClosure negation = !std::move(closureFn);

		const bool result = std::invoke(std::move(negation), 42);

		REQUIRE(expected == result);
	}
}

TEMPLATE_TEST_CASE_SIG(
	"Negating functional propagates noexcept specification during invocation.",
	"[functional][functional::Conjunction]",
	((bool expected), expected),
	(true),
	(false)
)
{
	using Fn = decltype(!std::declval<Closure<NoThrowInvokable<expected>>&&>());

	STATIC_REQUIRE(expected == std::is_nothrow_invocable_v<Fn, int>);
}

TEMPLATE_TEST_CASE_SIG(
	"functional::ConjunctionOperator propagates noexcept specification during composing.",
	"[functional][functional::Conjunction]",
	((bool nothrowCopyable, bool nothrowMovable), nothrowCopyable, nothrowMovable),
	(true, true),
	(false, true),
	(true, false),
	(false, false)
)
{
	using Fn = NoThrowConstructible<nothrowCopyable, nothrowMovable>;
	using Closure = sf::BasicClosure<Fn, sf::BasicInvokePolicy, sf::NegationOperator>;

	STATIC_REQUIRE(nothrowCopyable	== noexcept(!std::declval<const Closure&>()));
	STATIC_REQUIRE(nothrowMovable == noexcept(!std::declval<Closure&&>()));
}
