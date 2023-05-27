//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/functional/BasicClosure.hpp"

#include <catch2/catch_template_test_macros.hpp>

#include "Helper.hpp"
#include "../helper.hpp"

#include "Simple-Utility/functional/mixins/InvokePolicies.hpp"

namespace sf = sl::functional;

struct TestFun
{
	constexpr auto operator ()(auto&&...) const noexcept
	{
	}
};

TEMPLATE_TEST_CASE_SIG(
	"functional::unwrap_functional unwraps functionals if stored in a BasicClosure, or yields type as-is.",
	"[functional]",
	((bool dummy, class Expected, class Fun), dummy, Expected, Fun),
	(true, TestFun&, TestFun&),
	(true, const TestFun&, const TestFun&),
	(true, TestFun&&, TestFun&&),
	(true, const TestFun&&, const TestFun&&),
	(true, const TestFun&, const sf::BasicClosure<TestFun, sf::BasicInvokePolicy>&),
	(true, TestFun&, sf::BasicClosure<TestFun, sf::BasicInvokePolicy>&),
	(true, const TestFun&&, const sf::BasicClosure<TestFun, sf::BasicInvokePolicy>&&),
	(true, TestFun&&, sf::BasicClosure<TestFun, sf::BasicInvokePolicy>&&)
)
{
	STATIC_REQUIRE(std::same_as<Expected, typename sf::unwrap_functional<Fun>::type>);
	STATIC_REQUIRE(std::same_as<Expected, sf::unwrap_functional_t<Fun>>);
}

TEMPLATE_TEST_CASE_SIG(
	"functional::forward_unwrapped unwraps functionals if stored in a BasicClosure and forwards it.",
	"[functional]",
	((bool dummy, class Expected, class Fun), dummy, Expected, Fun),
	(true, TestFun&, TestFun&),
	(true, const TestFun&, const TestFun&),
	(true, TestFun&&, TestFun&&),
	(true, const TestFun&&, const TestFun&&),
	(true, const TestFun&, const sf::BasicClosure<TestFun, sf::BasicInvokePolicy>&),
	(true, TestFun&, sf::BasicClosure<TestFun, sf::BasicInvokePolicy>&),
	(true, const TestFun&&, const sf::BasicClosure<TestFun, sf::BasicInvokePolicy>&&),
	(true, TestFun&&, sf::BasicClosure<TestFun, sf::BasicInvokePolicy>&&)
)
{
	STATIC_REQUIRE(std::same_as<Expected, decltype(sf::forward_unwrapped<Fun>(std::declval<Fun&>()))>);
}

template <template <class, class> class InvokablePolicy, template <class> class... OperatorPolicies>
struct TemplatedClosure
{
	template <class Fn>
	using type = sf::BasicClosure<Fn, InvokablePolicy, OperatorPolicies...>;
};

template <class>
struct TestPolicy
{
};

TEMPLATE_TEST_CASE_SIG(
	"functional::BasicClosure is invocable in any qualification, regardless the applied InvokePolicy.",
	"[functional]",
	((bool dummy, template <class, class> class InvokePolicy, auto expected, auto arg), dummy, InvokePolicy, expected, arg),
	(true, sf::BasicInvokePolicy, 1337, 42),
	(true, sf::NodiscardInvokePolicy, 1337, 42),
	(true, sf::PredicateInvokePolicy, true, 42)
)
{
	GenericOperation1Mock<decltype(expected), decltype(arg)> func{};
	using Closure = TemplatedClosure<InvokePolicy>;

	SECTION("As const lvalue ref.")
	{
		REQUIRE_CALL(func, call_const_lvalue_ref(arg))
			.RETURN(expected);

		sf::BasicClosure closure = sf::enclose_functional<Closure::template type>(std::move(func));

		REQUIRE(expected == std::invoke(std::as_const(closure), arg));
	}

	SECTION("As lvalue ref.")
	{
		REQUIRE_CALL(func, call_lvalue_ref(arg))
			.RETURN(expected);

		sf::BasicClosure closure = sf::enclose_functional<Closure::template type>(std::move(func));

		REQUIRE(expected == std::invoke(closure, arg));
	}

	SECTION("As const rvalue ref.")
	{
		REQUIRE_CALL(func, call_const_rvalue_ref(arg))
			.RETURN(expected);

		sf::BasicClosure closure = sf::enclose_functional<Closure::template type>(std::move(func));

		REQUIRE(expected == std::invoke(std::move(std::as_const(closure)), arg));
	}

	SECTION("As rvalue ref.")
	{
		REQUIRE_CALL(func, call_rvalue_ref(arg))
			.RETURN(expected);

		sf::BasicClosure closure = sf::enclose_functional<Closure::template type>(std::move(func));

		REQUIRE(expected == std::invoke(std::move(closure), arg));
	}
}

TEMPLATE_TEST_CASE_SIG(
	"functional::closure_template yields the given closure template.",
	"[functional]",
	((bool dummy, template <class> class Closure), dummy, Closure),
	(true, TemplatedClosure<sf::BasicInvokePolicy>::type),
	(true, (TemplatedClosure<sf::BasicInvokePolicy, TestPolicy>::type))
)
{
	using Fn = Closure<TestFun>;

	STATIC_REQUIRE(std::same_as<Fn, typename sf::closure_template<Fn>::template type<TestFun>>);
}

TEMPLATE_TEST_CASE_SIG(
	"functional::enclose_functional wraps the given functional in the provided closure.",
	"[functional]",
	((bool dummy, template <class> class Closure), dummy, Closure),
	(true, TemplatedClosure<sf::BasicInvokePolicy>::type),
	(true, (TemplatedClosure<sf::BasicInvokePolicy, TestPolicy>::type))
)
{
	TestFun fn{};

	SECTION("Copy fn.")
	{
		auto enclosed = sf::enclose_functional<Closure>(fn);

		STATIC_REQUIRE(std::same_as<Closure<TestFun>, decltype(enclosed)>);
	}

	SECTION("Move fn.")
	{
		auto enclosed = sf::enclose_functional<Closure>(std::move(fn));

		STATIC_REQUIRE(std::same_as<Closure<TestFun>, decltype(enclosed)>);
	}

}
