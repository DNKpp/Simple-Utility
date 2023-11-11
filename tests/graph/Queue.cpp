//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/graph/Queue.hpp"
#include "Simple-Utility/graph/mixins/queue/std_priority_queue.hpp"
#include "Simple-Utility/graph/mixins/queue/std_queue.hpp"
#include "Simple-Utility/graph/mixins/queue/std_stack.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>

#include "Defines.hpp"

#include <array>

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

	struct customized_fun_empty
	{
		MAKE_CONST_MOCK0(is_empty, bool());
	};

	using TestNode = GenericBasicNode<int>;
	using TestRankedNode = GenericRankedNode<std::string, int>;

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

	struct customized_fun_insert
	{
		MAKE_MOCK1(do_insert, void(std::vector<TestNode>));
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

	struct customized_fun_next
	{
		MAKE_MOCK0(get_next, TestNode());
	};
}

template <>
struct sl::graph::customize::empty_fn<customized_fun_empty>
{
	[[nodiscard]]
	bool operator ()(const customized_fun_empty& e) const
	{
		return e.is_empty();
	}
};

template <>
struct sl::graph::customize::insert_fn<customized_fun_insert>
{
	void operator ()(customized_fun_insert& e, std::vector<TestNode> v) const
	{
		e.do_insert(std::move(v));
	}
};

template <>
struct sl::graph::customize::next_fn<customized_fun_next>
{
	[[nodiscard]]
	decltype(auto) operator ()(customized_fun_next& e) const
	{
		return e.get_next();
	}
};

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

	SECTION("Access via the customizization point.")
	{
		customized_fun_empty mock{};
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

	SECTION("Access via the customization point.")
	{
		customized_fun_insert mock{};
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

	SECTION("Access via the customization point.")
	{
		customized_fun_next mock{};
		REQUIRE_CALL(mock, get_next())
			.RETURN(expected);

		REQUIRE(expected == sg::queue::next(mock));
	}
}

TEMPLATE_TEST_CASE_SIG(
	"concepts::queue_for determines, whether the given type satisfies the requirements of a queue for the specified node type.",
	"[graph][graph::concepts]",
	((bool expected, class Queue, class Node), expected, Queue, Node),
	(false, member_fun_empty, TestNode),
	(false, free_fun_empty, TestNode),
	(false, member_fun_insert, TestNode),
	(false, free_fun_insert, TestNode),
	(false, member_fun_next, TestNode),
	(false, free_fun_next, TestNode),
	(true, QueueMock<TestNode>, TestNode),
	(true, sg::queue::CommonStack<TestNode>, TestNode),
	(true, sg::queue::CommonQueue<TestNode>, TestNode),
	(true, sg::queue::CommonPriorityQueue<TestRankedNode>, TestRankedNode)
)
{
	STATIC_REQUIRE(expected == sg::concepts::queue_for<Queue, Node>);
}

TEST_CASE("graph::queue::CommonStack follows the queue protocol.", "[graph][graph::queue]")
{
	sg::queue::CommonStack<TestNode> queue{};

	REQUIRE(sg::queue::empty(queue));

	TestNode node{.vertex = 42};

	SECTION("When a single node is inserted.")
	{
		sg::queue::insert(queue, std::array{node});
	}

	SECTION("When multiple nodes are inserted.")
	{
		sg::queue::insert(queue, std::array{TestNode{44}, node});
	}

	SECTION("When multiple nodes are inserted during multiple insertions.")
	{
		sg::queue::insert(queue, std::array{TestNode{41}});
		sg::queue::insert(queue, std::array{TestNode{44}, node});
	}

	REQUIRE(!sg::queue::empty(queue));
	REQUIRE(node == sg::queue::next(queue));
}

TEST_CASE("graph::queue::CommonQueue follows the queue protocol.", "[graph][graph::queue]")
{
	sg::queue::CommonQueue<TestNode> queue{};

	REQUIRE(sg::queue::empty(queue));

	TestNode node{.vertex = 42};

	SECTION("When a single node is inserted.")
	{
		sg::queue::insert(queue, std::array{node});
	}

	SECTION("When multiple nodes are inserted.")
	{
		sg::queue::insert(queue, std::array{node, TestNode{44}});
	}

	SECTION("When multiple nodes are inserted during multiple insertions.")
	{
		sg::queue::insert(queue, std::array{node, TestNode{44}});
		sg::queue::insert(queue, std::array{TestNode{41}});
	}

	REQUIRE(!sg::queue::empty(queue));
	REQUIRE(node == sg::queue::next(queue));
}

TEST_CASE("graph::queue::CommonPriorityQueue follows the queue protocol.", "[graph][graph::queue]")
{
	sg::queue::CommonPriorityQueue<TestRankedNode> queue{};

	REQUIRE(sg::queue::empty(queue));

	TestRankedNode node{.vertex = "V42", .rank = 1};

	SECTION("When a single node is inserted.")
	{
		sg::queue::insert(queue, std::array{node});
	}

	SECTION("When multiple nodes are inserted.")
	{
		sg::queue::insert(queue, std::array{node, TestRankedNode{"V44", 2}});
	}

	SECTION("When multiple nodes are inserted during multiple insertions.")
	{
		sg::queue::insert(queue, std::array{node, TestRankedNode{"V44", 2}});
		sg::queue::insert(queue, std::array{TestRankedNode{"V41", 3}});
	}

	REQUIRE(!sg::queue::empty(queue));
	REQUIRE(node == sg::queue::next(queue));
}
