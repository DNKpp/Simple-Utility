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

// ReSharper disable CppDeclaratorNeverUsed

namespace
{
	struct member_vertex
	{
		int vertex;
	};

	struct member_fun_vertex
	{
		MAKE_CONST_MOCK0(vertex, int());
	};

	struct free_fun_vertex
	{
		MAKE_CONST_MOCK0(my_vertex, int());

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
		int rank;
	};

	struct member_fun_rank
	{
		MAKE_CONST_MOCK0(rank, int());
	};

	struct free_fun_rank
	{
		MAKE_CONST_MOCK0(my_rank, int());

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

	struct node_with_custom_trait
	{
		int vertex;
		float rank;
	};
}

template <>
struct sg::customize::vertex_fn<custom_fun_vertex>
{
	[[nodiscard]]
	decltype(auto) operator ()(const custom_fun_vertex& e) const
	{
		return e.my_vertex();
	}
};

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

TEST_CASE("graph::node::vertex serves as a customization point accessing the vertex.", "[graph][detail]")
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

	SECTION("Access via custom function.")
	{
		custom_fun_vertex mock{};
		REQUIRE_CALL(mock, my_vertex())
			.RETURN(expected);
		REQUIRE(expected == sg::node::vertex(std::as_const(mock)));
	}
}

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
	(true, GenericBasicNode<std::string>),
	(true, GenericRankedNode<std::string, int>),
	(true, sg::CommonBasicNode<std::string>),
	(true, sg::CommonRankedNode<std::string, int>),

	(true, sg::decorator::PredecessorNode<sg::CommonBasicNode<std::string>>),
	(true, sg::decorator::PredecessorNode<sg::CommonRankedNode<std::string, int>>),
	(true, sg::decorator::DepthNode<sg::CommonBasicNode<std::string>>),
	(true, sg::decorator::DepthNode<sg::CommonRankedNode<std::string, int>>),
	(true, sg::decorator::PredecessorNode<sg::decorator::DepthNode<sg::CommonBasicNode<std::string>>>),
	(true, sg::decorator::PredecessorNode<sg::decorator::DepthNode<sg::CommonRankedNode<std::string, int>>>),
	(true, sg::decorator::DepthNode<sg::decorator::PredecessorNode<sg::CommonBasicNode<std::string>>>),
	(true, sg::decorator::DepthNode<sg::decorator::PredecessorNode<sg::CommonRankedNode<std::string, int>>>)
)
{
	STATIC_REQUIRE(expected == sg::concepts::basic_node<T>);
}

TEMPLATE_TEST_CASE_SIG(
	"concepts::ranked_node determines, whether the given type satisfies the requirements.",
	"[graph][graph::concepts]",
	((bool expected, class T), expected, T),
	(false, member_rank),
	(false, member_fun_rank),
	(false, free_fun_rank),
	(false, minimal_node),
	(true, ranked_node),
	(false, GenericBasicNode<std::string>),
	(true, GenericRankedNode<std::string, int>),
	(false, sg::CommonBasicNode<std::string>),
	(true, sg::CommonRankedNode<std::string, int>),

	(false, sg::decorator::PredecessorNode<sg::CommonBasicNode<std::string>>),
	(true, sg::decorator::PredecessorNode<sg::CommonRankedNode<std::string, int>>),
	(false, sg::decorator::DepthNode<sg::CommonBasicNode<std::string>>),
	(true, sg::decorator::DepthNode<sg::CommonRankedNode<std::string, int>>),
	(false, sg::decorator::PredecessorNode<sg::decorator::DepthNode<sg::CommonBasicNode<std::string>>>),
	(true, sg::decorator::PredecessorNode<sg::decorator::DepthNode<sg::CommonRankedNode<std::string, int>>>),
	(false, sg::decorator::DepthNode<sg::decorator::PredecessorNode<sg::CommonBasicNode<std::string>>>),
	(true, sg::decorator::DepthNode<sg::decorator::PredecessorNode<sg::CommonRankedNode<std::string, int>>>)
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
	"concepts::edge_for determines, whether the Edge type satisfies the minimal requirements of the Node type.",
	"[graph][graph::concepts]",
	((bool expected, class Edge, class Node), expected, Edge, Node),
	(false, GenericBasicEdge<std::string>, GenericBasicNode<int>),
	(true, GenericBasicEdge<std::string>, GenericBasicNode<std::string>),
	(true, (GenericWeightedEdge<std::string, int>), GenericBasicNode<std::string>),

	(false, GenericBasicEdge<std::string>, (GenericRankedNode<std::string, int>)),
	(true, (GenericWeightedEdge<std::string, int>), (GenericRankedNode<std::string, int>)),

	(true, sg::CommonBasicEdge<std::string>, sg::CommonBasicNode<std::string>),
	(true, sg::CommonWeightedEdge<std::string, int>, sg::CommonRankedNode<std::string, int>)
)
{
	STATIC_REQUIRE(expected == sg::concepts::edge_for<Edge, Node>);
}
