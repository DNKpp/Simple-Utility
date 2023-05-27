//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/functional/Composition.hpp"

#include <catch2/catch_template_test_macros.hpp>

#include "Helper.hpp"

#include <optional>
#include <string>

#include "Simple-Utility/functional/mixins/Pipe.hpp"
#include "Simple-Utility/functional/mixins/InvokePolicies.hpp"

namespace sf = sl::functional;

// ReSharper disable CppClangTidyBugproneUseAfterMove

TEST_CASE("functional::Composition composes two functions in accordance to its strategy.", "[functional]")
{
	GenericOperation2Mock<int, float, const std::string&> first{};
	GenericOperation1Mock<int, int> second{};

	trompeloeil::sequence seq{};
	std::optional<int> result{};

	SECTION("as lvalue reference.")
	{
		REQUIRE_CALL(first, call_lvalue_ref(1337.f, "1337"))
			.RETURN(42)
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(second, call_lvalue_ref(42))
			.RETURN(-1)
			.IN_SEQUENCE(seq);

		sf::Composition composition = sf::make_composition<sf::PipeStrategy>(std::move(first), std::move(second));

		result = composition(1337.f, "1337");
	}

	SECTION("as const lvalue reference.")
	{
		REQUIRE_CALL(first, call_const_lvalue_ref(1337.f, "1337"))
			.RETURN(42)
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(second, call_const_lvalue_ref(42))
			.RETURN(-1)
			.IN_SEQUENCE(seq);

		const sf::Composition composition = sf::make_composition<sf::PipeStrategy>(std::move(first), std::move(second));

		result = composition(1337.f, "1337");
	}

	SECTION("as rvalue reference.")
	{
		REQUIRE_CALL(first, call_rvalue_ref(1337.f, "1337"))
			.RETURN(42)
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(second, call_rvalue_ref(42))
			.RETURN(-1)
			.IN_SEQUENCE(seq);

		sf::Composition composition = sf::make_composition<sf::PipeStrategy>(std::move(first), std::move(second));

		result = std::move(composition)(1337.f, "1337");
	}

	SECTION("as const rvalue reference.")
	{
		REQUIRE_CALL(first, call_const_rvalue_ref(1337.f, "1337"))
			.RETURN(42)
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(second, call_const_rvalue_ref(42))
			.RETURN(-1)
			.IN_SEQUENCE(seq);

		const sf::Composition composition = sf::make_composition<sf::PipeStrategy>(std::move(first), std::move(second));

		result = std::move(std::as_const(composition))(1337.f, "1337");
	}

	REQUIRE(-1 == result);
}

template <class Fun>
using DefaultClosure = sf::BasicClosure<Fun, sf::BasicInvokePolicy>;

TEST_CASE("functional::make_composition unwrapps functionals stored in functional::BasicClosure.", "[functional]")
{
	GenericOperation2Mock<int, float, const std::string&> first{};
	GenericOperation1Mock<int, int> second{};

	trompeloeil::sequence seq{};
	std::optional<int> result{};

	REQUIRE_CALL(first, call_lvalue_ref(1337.f, "1337"))
			.RETURN(42)
			.IN_SEQUENCE(seq);

	REQUIRE_CALL(second, call_lvalue_ref(42))
			.RETURN(-1)
			.IN_SEQUENCE(seq);

	SECTION("First is wrapped.")
	{
		sf::Composition composition = sf::make_composition<sf::PipeStrategy>(
			DefaultClosure<GenericOperation2Mock<int, float, const std::string&>>{std::move(first)},
			std::move(second));

		result = composition(1337.f, "1337");
	}

	SECTION("Second is wrapped.")
	{
		sf::Composition composition = sf::make_composition<sf::PipeStrategy>(
			std::move(first),
			DefaultClosure<GenericOperation1Mock<int, int>>{std::move(second)});

		result = composition(1337.f, "1337");
	}

	REQUIRE(-1 == result);
}

TEST_CASE("functional::make_composition flattens Composition if appropriate.", "[functional]")
{
	GenericOperation2Mock<int, float, const std::string&> first{};
	GenericOperation1Mock<int, int> second{};
	GenericOperation1Mock<int, int> third{};
	GenericOperation1Mock<int, int> fourth{};

	using Expected = sf::Composition<
		sf::PipeStrategy,
		GenericOperation2Mock<int, float, const std::string&>,
		GenericOperation1Mock<int, int>,
		GenericOperation1Mock<int, int>,
		GenericOperation1Mock<int, int>>;

	trompeloeil::sequence seq{};
	std::optional<int> result{};

	REQUIRE_CALL(first, call_lvalue_ref(1337.f, "1337"))
			.RETURN(42)
			.IN_SEQUENCE(seq);

	REQUIRE_CALL(second, call_lvalue_ref(42))
			.RETURN(1337)
			.IN_SEQUENCE(seq);

	REQUIRE_CALL(third, call_lvalue_ref(1337))
			.RETURN(-1)
			.IN_SEQUENCE(seq);

	REQUIRE_CALL(fourth, call_lvalue_ref(-1))
			.RETURN(-1337)
			.IN_SEQUENCE(seq);

	SECTION("FirstFn is composition.")
	{
		Expected composition = sf::make_composition<sf::PipeStrategy>(
			sf::make_composition<sf::PipeStrategy>(std::move(first), std::move(second)),
			std::move(third),
			std::move(fourth));

		result = composition(1337.f, "1337");
	}

	SECTION("SecondFn is composition.")
	{
		Expected composition = sf::make_composition<sf::PipeStrategy>(
			std::move(first),
			sf::make_composition<sf::PipeStrategy>(std::move(second), std::move(third)),
			std::move(fourth));

		result = composition(1337.f, "1337");
	}

	SECTION("ThirdFn is composition.")
	{
		Expected composition = sf::make_composition<sf::PipeStrategy>(
			std::move(first),
			std::move(second),
			sf::make_composition<sf::PipeStrategy>(std::move(third), std::move(fourth)));

		result = composition(1337.f, "1337");
	}

	REQUIRE(-1337 == result);
}
