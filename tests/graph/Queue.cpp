//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/graph/Queue.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>

#include "Defines.hpp"

namespace
{
	struct member_fun_empty
	{
		MAKE_CONST_MOCK0(empty, bool());
	};

	struct free_fun_empty
	{
		MAKE_CONST_MOCK0(is_empty, bool());

		friend bool empty(const free_fun_empty& obj)
		{
			return obj.is_empty();
		}
	};
}

TEST_CASE("graph::queue::empty serves as a customization point, detmerining whether the queue is empty.", "[graph][graph::queue]")
{
	const bool expected = GENERATE(false, true);

	SECTION("Access via the member function.")
	{
		member_fun_empty mock{};
		REQUIRE_CALL(mock, empty())
			.RETURN(expected);
		REQUIRE(expected == sg::queue::empty(std::as_const(mock)));
	}

	SECTION("Access via the free function.")
	{
		free_fun_empty mock{};
		REQUIRE_CALL(mock, is_empty())
			.RETURN(expected);
		REQUIRE(expected == sg::queue::empty(std::as_const(mock)));
	}
}
