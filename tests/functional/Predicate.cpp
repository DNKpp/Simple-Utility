//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include "Helper.hpp"

#include "Simple-Utility/functional/Predicate.hpp"

#include "Simple-Utility/functional/bind_back.hpp"

namespace sf = sl::functional;

struct PredicateMock
{
	static constexpr bool trompeloeil_movable_mock = true;

	bool operator ()(int param) { return call(param); }

	MAKE_MOCK1(call, bool(int));
};

TEST_CASE(
	"functional::Predicate can be composed with several operators.",
	"[functional][functional::Predicate]"
)
{
	std::array<PredicateMock, 5> mocks{};

	trompeloeil::sequence seq{};

	REQUIRE_CALL(mocks[0], call(42))
		.RETURN(false)
		.IN_SEQUENCE(seq);

	REQUIRE_CALL(mocks[1], call(42))
		.RETURN(true)
		.IN_SEQUENCE(seq);

	REQUIRE_CALL(mocks[2], call(42))
		.RETURN(true)
		.IN_SEQUENCE(seq);

	REQUIRE_CALL(mocks[3], call(42))
		.RETURN(false)
		.IN_SEQUENCE(seq);

	REQUIRE_CALL(mocks[4], call(true))
		.RETURN(false)
		.IN_SEQUENCE(seq);

	auto composition = !((sf::envelop<sf::Predicate>(std::move(mocks[0])) || std::move(mocks[1]))
						&& std::move(mocks[2]))
						== std::move(mocks[3])
						| std::move(mocks[4]);

	REQUIRE(!composition(42));
}

//! [predicate is_even definition]
inline constexpr auto is_even = sf::envelop<sf::Predicate>([](const std::integral auto& x) { return x % 2 == 0; });
//! [predicate is_even definition]

TEST_CASE(
	"functional::Predicate can be used to create more complex predicates.",
	"[functional][functional::Predicate][example]"
)
{
	//! [predicate more complex]
	std::vector<int> values{};
	std::ranges::copy_if(
		std::array{42, 43, 44, 45, 46, 47},
		std::back_inserter(values),
		is_even && sf::bind_back(std::less{}, 44)
		|| !is_even && sf::bind_back(std::greater{}, 45));

	REQUIRE_THAT(
		values,
		Catch::Matchers::RangeEquals(std::array{42, 47}));
	//! [predicate more complex]
}
