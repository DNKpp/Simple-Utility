//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>

#include "Helper.hpp"

#include "Simple-Utility/functional/Transform.hpp"

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
