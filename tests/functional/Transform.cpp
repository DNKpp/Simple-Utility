//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include "Helper.hpp"

#include "Simple-Utility/functional/Transform.hpp"

#include "Simple-Utility/functional/Arithmetic.hpp"
#include "Simple-Utility/functional/Predicate.hpp"

#include <ranges>

namespace sf = sl::functional;

TEST_CASE(
	"functional::Transform can be composed.",
	"[functional][functional::Transform]"
)
{
	GenericOperation2Mock<int, int, int> firstFn{};
	GenericOperation1Mock<int, int> secondFn{};
	GenericOperation1Mock<int, int> thirdFn{};

	trompeloeil::sequence seq{};

	REQUIRE_CALL(firstFn, call_const_lvalue_ref(trompeloeil::_, trompeloeil::_))
		.RETURN(_1 * _2)
		.IN_SEQUENCE(seq);

	REQUIRE_CALL(secondFn, call_const_lvalue_ref(trompeloeil::_))
		.RETURN(_1 + 42)
		.IN_SEQUENCE(seq);

	REQUIRE_CALL(thirdFn, call_const_lvalue_ref(trompeloeil::_))
		.RETURN((-1) * _1)
		.IN_SEQUENCE(seq);

	const auto composition = sf::envelop<sf::Transform>(std::move(firstFn))
							| std::move(secondFn)
							| std::move(thirdFn);

	REQUIRE(-(2 * 1337 + 42) == composition(2, 1337));
}

TEST_CASE(
	"functional::Transform can be used to project to different types.",
	"[functional][functional::Transform][example]"
)
{
	//! [transform piped]
	auto composition = sf::envelop<sf::Transform>([](const int x) { return x + 42; })
						| [](const int x) { return std::to_string(x); };

	REQUIRE(composition(1) == "43");
	//! [transform piped]
}

//! [transform to_string definition]
inline constexpr auto to_string = sf::envelop<sf::Transform>([](const auto& x) { return std::to_string(x); });
//! [transform to_string definition]

TEST_CASE(
	"functional::Transform can be used to project to different types, in a more complex pipeline.",
	"[functional][functional::Transform][example]"
)
{
	//! [transform more complex pipeline]
	std::vector<std::string> strings{};
	std::ranges::transform(
		std::array{42, 43, 44},
		std::back_inserter(strings),
		sf::arithmetic::negate
		| std::bind_front(std::multiplies{}, 2)
		| to_string
		| [i{0}](const std::string& str) mutable { return "value#" + std::to_string(++i) + " is = " + str; });

	REQUIRE_THAT(
		strings,
		Catch::Matchers::RangeEquals(std::array{
			"value#1 is = -84",
			"value#2 is = -86",
			"value#3 is = -88"
			}));
	//! [transform more complex pipeline]
}
