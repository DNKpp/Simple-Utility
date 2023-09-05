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

#include <string>

namespace
{
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

	struct ranked_node
	{
		using vertex_type = std::string;
		using rank_type = int;

		vertex_type vertex;
		rank_type rank;
	};

	struct minimal_node_factory
	{
		using node_type = minimal_node;
		using vertex_type = sg::node::vertex_t<node_type>;

		static node_type make_init_node(const vertex_type& v)
		{
			return {.vertex = v};
		}

		static node_type make_successor_node([[maybe_unused]] const node_type& predecessor, const node_type& v)
		{
			return {.vertex = sg::node::vertex(v)};
		}
	};
	struct node_with_custom_trait
	{
		int vertex;
		float rank;
	};
}

template <>
struct sg::node::traits<node_with_custom_trait>
{
	using vertex_type = int;
	using rank_type = float;
};

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
	"concepts::node determines, whether the given type satisfies the requirements.",
	"[graph][graph::concepts]",
	((bool expected, class T), expected, T),
	(true, minimal_node),
	(true, ranked_node),
	(true, BasicTestNode<int>)
)
{
	STATIC_REQUIRE(expected == sg::concepts::node<T>);
}

TEMPLATE_TEST_CASE_SIG(
	"concepts::ranked_node determines, whether the given type satisfies the requirements.",
	"[graph][graph::concepts]",
	((bool expected, class T), expected, T),
	(false, member_rank),
	(false, member_fun_rank),
	(false, free_fun_rank),
	(false, minimal_node),
	(true, ranked_node)
)
{
	STATIC_REQUIRE(expected == sg::concepts::ranked_node<T>);
}

TEST_CASE(
	"Default graph::node::traits exposes vertex_type if readable.",
	"[graph][graph::node]"
)
{
	using TestType = minimal_node;

	STATIC_REQUIRE(std::same_as<int, sg::node::traits<TestType>::vertex_type>);
	STATIC_REQUIRE(std::same_as<int, sg::node::vertex_t<TestType>>);
}

TEST_CASE(
	"Default graph::node::traits exposes vertex_type and rank_type if readable.",
	"[graph][graph::node]"
)
{
	using TestType = ranked_node;

	STATIC_REQUIRE(std::same_as<std::string, sg::node::traits<TestType>::vertex_type>);
	STATIC_REQUIRE(std::same_as<std::string, sg::node::vertex_t<TestType>>);

	STATIC_REQUIRE(std::same_as<int, sg::node::traits<TestType>::rank_type>);
	STATIC_REQUIRE(std::same_as<int, sg::node::rank_t<TestType>>);
}

TEST_CASE(
	"graph::node::traits can be specialized.",
	"[graph][graph::node]"
)
{
	using TestType = node_with_custom_trait;

	STATIC_REQUIRE(std::same_as<int, sg::node::traits<TestType>::vertex_type>);
	STATIC_REQUIRE(std::same_as<int, sg::node::vertex_t<TestType>>);

	STATIC_REQUIRE(std::same_as<float, sg::node::traits<TestType>::rank_type>);
	STATIC_REQUIRE(std::same_as<float, sg::node::rank_t<TestType>>);
}

TEMPLATE_TEST_CASE_SIG(
	"concepts::node_factory_for determines, whether the given type satisfies the requirements of a node_factory for the specified node type.",
	"[graph][graph::concepts]",
	((bool expected, class Factory, class Node), expected, Factory, Node),
	(false, minimal_node_factory, BasicTestNode<int>),
	(true, minimal_node_factory, minimal_node),
	(true, BasicNodeFactoryMock<minimal_node, generic_basic_graph_stub::edge_type>, minimal_node),
	(true, sg::NodeFactory<sg::BasicNode<std::string>>, sg::BasicNode<std::string>),
	(false, sg::NodeFactory<sg::RankedNode<std::string, int>>, sg::BasicNode<std::string>),
	(false, sg::NodeFactory<sg::BasicNode<std::string>>, sg::RankedNode<std::string, int>),
	(true, sg::NodeFactory<sg::RankedNode<std::string, int>>, sg::RankedNode<std::string, int>)
)
{
	STATIC_REQUIRE(expected == sg::concepts::node_factory_for<Factory, Node>);
}

