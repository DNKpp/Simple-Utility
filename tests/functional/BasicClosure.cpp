//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/functional/BasicClosure.hpp"

#include <catch2/catch_template_test_macros.hpp>

#include "Helper.hpp"
#include "../helper.hpp"

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
	"functional::BasicClosure is invocable in any qualification, regardless the applied InvokePolicy.",
	"[functional]",
	((bool dummy, template <class, class> class InvokePolicy), dummy, InvokePolicy),
	(true, sf::BasicInvokePolicy)
)
{
	GenericOperation1Mock<int, int> func{};

	SECTION("As const lvalue ref.")
	{
		REQUIRE_CALL(func, call_const_lvalue_ref(42))
			.RETURN(1337);

		sf::BasicClosure<GenericOperation1Mock<int, int>, InvokePolicy> closure{std::move(func)};

		REQUIRE(1337 == std::as_const(closure)(42));
	}

	SECTION("As lvalue ref.")
	{
		REQUIRE_CALL(func, call_lvalue_ref(42))
			.RETURN(1337);

		sf::BasicClosure<GenericOperation1Mock<int, int>, InvokePolicy> closure{std::move(func)};

		REQUIRE(1337 == closure(42));
	}

	SECTION("As const rvalue ref.")
	{
		REQUIRE_CALL(func, call_const_rvalue_ref(42))
			.RETURN(1337);

		sf::BasicClosure<GenericOperation1Mock<int, int>, InvokePolicy> closure{std::move(func)};

		REQUIRE(1337 == std::move(std::as_const(closure))(42));
	}

	SECTION("As rvalue ref.")
	{
		REQUIRE_CALL(func, call_rvalue_ref(42))
			.RETURN(1337);

		sf::BasicClosure<GenericOperation1Mock<int, int>, InvokePolicy> closure{std::move(func)};

		REQUIRE(1337 == std::move(closure)(42));
	}
}
