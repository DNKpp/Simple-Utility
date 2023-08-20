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

// ReSharper disable CppDeclaratorNeverUsed

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

	struct TestNode
	{
		// ReSharper disable once CppTypeAliasNeverUsed
		using vertex_type = int;
		int vertex{};

		friend bool operator==(const TestNode&, const TestNode&) = default;
	};

	struct member_fun_insert
	{
		MAKE_MOCK1(insert, void(std::vector<TestNode>));
	};

	struct free_fun_insert
	{
		MAKE_MOCK1(do_insert, void(std::vector<TestNode>));

		friend void insert(free_fun_insert& obj, std::vector<TestNode> input)
		{
			obj.do_insert(std::move(input));
		}
	};

	struct member_fun_next
	{
		MAKE_MOCK0(next, TestNode());
	};

	struct free_fun_next
	{
		MAKE_MOCK0(get_next, TestNode());

		friend TestNode next(free_fun_next& obj)
		{
			return obj.get_next();
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

TEST_CASE("graph::queue::insert serves as a customization point, inserting the range elements.", "[graph][graph::queue]")
{
	const auto& expected = GENERATE(
		std::vector<TestNode>{},
		(std::vector<TestNode>{{42}, {1337} }));

	SECTION("Access via the member function.")
	{
		member_fun_insert mock{};
		REQUIRE_CALL(mock, insert(expected));

		sg::queue::insert(mock, expected);
	}

	SECTION("Access via the free function.")
	{
		free_fun_insert mock{};
		REQUIRE_CALL(mock, do_insert(expected));

		sg::queue::insert(mock, expected);
	}
}

TEST_CASE("graph::queue::next serves as a customization point, retrieving the next node.", "[graph][graph::queue]")
{
	const auto expected = GENERATE(TestNode{42}, TestNode{1337});

	SECTION("Access via the member function.")
	{
		member_fun_next mock{};
		REQUIRE_CALL(mock, next())
			.RETURN(expected);

		REQUIRE(expected == sg::queue::next(mock));
	}

	SECTION("Access via the free function.")
	{
		free_fun_next mock{};
		REQUIRE_CALL(mock, get_next())
			.RETURN(expected);

		REQUIRE(expected == sg::queue::next(mock));
	}
}

TEMPLATE_TEST_CASE_SIG(
	"concepts::queue_for determines, whether the given type satisfies the requirements of a queue for the specified node type.",
	"[graph][graph::concepts]",
	((bool expected, class T), expected, T),
	(false, member_fun_empty),
	(false, free_fun_empty),
	(false, member_fun_insert),
	(false, free_fun_insert),
	(false, member_fun_next),
	(false, free_fun_next),
	(false, QueueMock<BasicTestNode<int>>),
	(true, QueueMock<TestNode>)
)
{
	STATIC_REQUIRE(expected == sg::concepts::queue_for<T, TestNode>);
}
