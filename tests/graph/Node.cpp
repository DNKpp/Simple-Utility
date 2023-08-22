//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/graph/Node.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>

#include "Defines.hpp"

namespace
{
	struct member_vertex
	{
		// ReSharper disable once CppDeclaratorNeverUsed
		int vertex;
	};

	struct member_fun_vertex
	{
		MAKE_CONST_MOCK0(vertex, int());
	};

	struct free_fun_vertex
	{
		MAKE_CONST_MOCK0(my_vertex, int());

		// ReSharper disable once CppDeclaratorNeverUsed
		friend int vertex(const free_fun_vertex& v)
		{
			return v.my_vertex();
		}
	};

	struct minimal_node
	{
		using vertex_type = int;

		vertex_type vertex;
	};

	struct minimal_node_factory
	{
		using node_type = minimal_node;
		using vertex_type = sg::feature_vertex_t<node_type>;

		static node_type make_init_node(const vertex_type& v)
		{
			return {.vertex = v};
		}

		static node_type make_successor_node([[maybe_unused]] const node_type& predecessor, const vertex_type& v)
		{
			return {.vertex = v};
		}
	};
}

TEST_CASE("graph::node::vertex serves as a customization point accessing the node vertex.", "[graph][graph::node]")
{
	const int expected = GENERATE(take(5, random(0, std::numeric_limits<int>::max())));

	SECTION("Access via the vertex member.")
	{
		REQUIRE(expected == sg::node::vertex(member_vertex{expected}));
	}

	SECTION("Access via the vertex member function.")
	{
		member_fun_vertex mock{};
		REQUIRE_CALL(mock, vertex())
			.RETURN(expected);
		REQUIRE(expected == sg::node::vertex(std::as_const(mock)));
	}

	SECTION("Access via the vertex free function.")
	{
		free_fun_vertex mock{};
		REQUIRE_CALL(mock, my_vertex())
			.RETURN(expected);
		REQUIRE(expected == sg::node::vertex(std::as_const(mock)));
	}
}

TEMPLATE_TEST_CASE_SIG(
	"concepts::node determines, whether the given type satisfies the node requirements.",
	"[graph][graph::concepts]",
	((bool expected, class T), expected, T),
	(false, member_vertex),
	(false, member_fun_vertex),
	(false, free_fun_vertex),
	(true, BasicTestNode<int>)
)
{
	STATIC_REQUIRE(expected == sg::concepts::node<T>);
}

TEMPLATE_TEST_CASE_SIG(
	"concepts::node_factory_for determines, whether the given type satisfies the requirements of a node_factory for the specified node type.",
	"[graph][graph::concepts]",
	((bool expected, class Factory, class Node), expected, Factory, Node),
	//(false, minimal_node_factory, BasicTestNode<int>),
	(true, minimal_node_factory, minimal_node)
)
{
	STATIC_REQUIRE(expected == sg::concepts::node_factory_for<Factory, Node>);
}

TEMPLATE_TEST_CASE_SIG(
	"concepts::compatible_with determines, whether the other type is compatible with T.",
	"[graph][graph::concepts]",
	((bool expected, class T, class Other), expected, T, Other),
	(false, minimal_node, BasicTestNode<std::string>),
	(true, minimal_node, BasicTestNode<int>)
)
{
	STATIC_REQUIRE(expected == sg::concepts::compatible_with<T, Other>);
}
