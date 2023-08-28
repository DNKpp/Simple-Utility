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

	struct custom_fun_vertex
	{
		MAKE_CONST_MOCK0(my_vertex, int());
	};

	struct member_rank
	{
		// ReSharper disable once CppDeclaratorNeverUsed
		int rank;
	};

	struct member_fun_rank
	{
		MAKE_CONST_MOCK0(rank, int());
	};

	struct free_fun_rank
	{
		MAKE_CONST_MOCK0(my_rank, int());

		// ReSharper disable once CppDeclaratorNeverUsed
		friend int rank(const free_fun_rank& v)
		{
			return v.my_rank();
		}
	};

	struct custom_fun_rank
	{
		MAKE_CONST_MOCK0(my_rank, int());
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

	struct generic_basic_graph
	{
		struct info
		{
			using vertex_type = int;
			vertex_type vertex;
		};

		[[nodiscard]]
		static std::vector<info> neighbor_infos(const sg::concepts::node auto&)
		{
			return {};
		}
	};
}

template <>
struct sg::customize::rank_fn<custom_fun_rank>
{
	[[nodiscard]]
	decltype(auto) operator ()(const custom_fun_rank& e) const
	{
		return e.my_rank();
	}
};

TEST_CASE("graph::node::rank serves as a customization point accessing the node rank.", "[graph][graph::node]")
{
	const int expected = GENERATE(take(5, random(0, std::numeric_limits<int>::max())));

	SECTION("Access via member.")
	{
		REQUIRE(expected == sg::node::rank(member_rank{expected}));
	}

	SECTION("Access via member function.")
	{
		member_fun_rank mock{};
		REQUIRE_CALL(mock, rank())
			.RETURN(expected);
		REQUIRE(expected == sg::node::rank(std::as_const(mock)));
	}

	SECTION("Access via free function.")
	{
		free_fun_rank mock{};
		REQUIRE_CALL(mock, my_rank())
			.RETURN(expected);
		REQUIRE(expected == sg::node::rank(std::as_const(mock)));
	}

	SECTION("Access via custom function.")
	{
		custom_fun_rank mock{};
		REQUIRE_CALL(mock, my_rank())
			.RETURN(expected);
		REQUIRE(expected == sg::node::rank(std::as_const(mock)));
	}
}

TEMPLATE_TEST_CASE_SIG(
	"concepts::node determines, whether the given type satisfies the node requirements.",
	"[graph][graph::concepts]",
	((bool expected, class T), expected, T),
	(false, member_vertex),
	(false, member_fun_vertex),
	(false, free_fun_vertex),
	(true, minimal_node),
	(true, BasicTestNode<int>),
	(true, generic_basic_graph::info),
	(true, BasicGraph<minimal_node>::info)
)
{
	STATIC_REQUIRE(expected == sg::concepts::node<T>);
}

TEMPLATE_TEST_CASE_SIG(
	"concepts::node_factory_for determines, whether the given type satisfies the requirements of a node_factory for the specified node type.",
	"[graph][graph::concepts]",
	((bool expected, class Factory, class Node), expected, Factory, Node),
	(false, minimal_node_factory, BasicTestNode<int>),
	(true, minimal_node_factory, minimal_node),
	(true, BasicNodeFactoryMock<minimal_node, generic_basic_graph::info>, minimal_node)
)
{
	STATIC_REQUIRE(expected == sg::concepts::node_factory_for<Factory, Node>);
}

TEMPLATE_TEST_CASE_SIG(
	"concepts::compatible_with determines, whether the other type is compatible with T.",
	"[graph][graph::concepts]",
	((bool expected, class T, class Other), expected, T, Other),
	(false, minimal_node, BasicTestNode<std::string>),
	(true, minimal_node, BasicTestNode<int>),
	(true, minimal_node, BasicGraph<minimal_node>::info),
	(true, BasicTestNode<int>, BasicGraph<minimal_node>::info),
	(false, BasicTestNode<std::string>, BasicGraph<minimal_node>::info),
	(true, minimal_node, generic_basic_graph::info),
	(true, BasicTestNode<int>, generic_basic_graph::info),
	(false, BasicTestNode<std::string>, generic_basic_graph::info)
)
{
	STATIC_REQUIRE(expected == sg::concepts::compatible_with<T, Other>);
}

TEMPLATE_TEST_CASE_SIG(
	"concepts::graph_for determines, whether the graph type satisfies the minimal requirements of the specified node.",
	"[graph][graph::concepts]",
	((bool expected, class Graph, class Node), expected, Graph, Node),
	(true, generic_basic_graph, minimal_node),
	(true, generic_basic_graph, BasicTestNode<int>),
	(false, generic_basic_graph, BasicTestNode<std::string>),
	(true, BasicGraph<minimal_node>, minimal_node),
	(false, BasicGraph<minimal_node>, BasicTestNode<int>),
	(false, BasicGraph<minimal_node>, BasicTestNode<std::string>)
)
{
	STATIC_REQUIRE(expected == sg::concepts::graph_for<Graph, Node>);
}
