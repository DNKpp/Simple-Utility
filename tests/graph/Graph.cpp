//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/graph/Graph.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include "Defines.hpp"

namespace
{
	struct member_fun_out_edges
	{
		using vertex_type = std::string;
		using edge_type = GenericBasicEdge<vertex_type>;

		MAKE_CONST_MOCK1(out_edges, std::vector<edge_type>(const vertex_type&));
	};

	struct free_fun_out_edges
	{
		using vertex_type = std::string;
		using edge_type = GenericBasicEdge<std::string>;

		MAKE_CONST_MOCK1(get_out_edges, std::vector<edge_type>(const vertex_type&));

		friend std::vector<edge_type> out_edges(const free_fun_out_edges& obj, const vertex_type& vertex)
		{
			return obj.get_out_edges(vertex);
		}
	};

	struct customized_out_edges
	{
		using vertex_type = std::string;
		using edge_type = GenericBasicEdge<std::string>;

		MAKE_CONST_MOCK1(get_out_edges, std::vector<edge_type>(const vertex_type&));
	};
}

template <>
struct sl::graph::customize::out_edges_fn<customized_out_edges>
{
	[[nodiscard]]
	auto operator ()(const customized_out_edges& e, const std::string& vertex) const
	{
		return e.get_out_edges(vertex);
	}
};

TEMPLATE_TEST_CASE_SIG(
	"graph::traits extracts edge type.",
	"[graph][graph::graph]",
	((bool dummy, class Expected, class Graph), dummy, Expected, Graph),
	(true, GenericBasicEdge<int>, BasicGraphMock<int>),
	(true, GenericBasicEdge<std::string>, BasicGraphMock<std::string>),
	(true, GenericWeightedEdge<std::string, int>, WeightedGraphMock<std::string, int>)
)
{
	STATIC_REQUIRE(std::same_as<Expected, typename sg::graph::traits<Graph>::edge_type>);
	STATIC_REQUIRE(std::same_as<Expected, sg::graph::edge_t<Graph>>);
}

TEMPLATE_TEST_CASE_SIG(
	"graph::traits extracts vertex type.",
	"[graph][graph::graph]",
	((bool dummy, class Expected, class Graph), dummy, Expected, Graph),
	(true, int, BasicGraphMock<int>),
	(true, std::string, BasicGraphMock<std::string>),
	(true, std::string, WeightedGraphMock<std::string, int>)
)
{
	STATIC_REQUIRE(std::same_as<Expected, typename sg::graph::traits<Graph>::vertex_type>);
	STATIC_REQUIRE(std::same_as<Expected, sg::graph::vertex_t<Graph>>);
}

TEMPLATE_TEST_CASE_SIG(
	"concepts::basic_graph determines, whether the given type satisfies the minimal requirements.",
	"[graph][graph::concepts]",
	((bool expected, class Graph), expected, Graph),
	(true, BasicGraphMock<std::string>),
	(true, WeightedGraphMock<std::string, int>),
	(true, EmptyGraphStub<std::string>),
	(true, BasicGraphStub),
	(true, WeightedGraphStub)
)
{
	STATIC_REQUIRE(expected == sg::concepts::basic_graph<Graph>);
}

TEST_CASE(
	"graph::graph::out_edges serves as a customization point, returning the outgoing edges of the given vertex.",
	"[graph][graph::graph]")
{
	const std::string vertex{"Hello, World!"};
	const std::vector<GenericBasicEdge<std::string>> expected{
		{"Edge0"},
		{"Edge1"},
		{"Edge2"}
	};

	SECTION("Access via the member function.")
	{
		const member_fun_out_edges mock{};
		REQUIRE_CALL(mock, out_edges(vertex))
			.RETURN(expected);
		REQUIRE_THAT(sg::graph::out_edges(mock, vertex), Catch::Matchers::RangeEquals(expected));
	}

	SECTION("Access via the free function.")
	{
		const free_fun_out_edges mock{};
		REQUIRE_CALL(mock, get_out_edges(vertex))
			.RETURN(expected);
		REQUIRE_THAT(sg::graph::out_edges(mock, vertex), Catch::Matchers::RangeEquals(expected));
	}

	SECTION("Access via customized function.")
	{
		const customized_out_edges mock{};
		REQUIRE_CALL(mock, get_out_edges(vertex))
			.RETURN(expected);
		REQUIRE_THAT(sg::graph::out_edges(mock, vertex), Catch::Matchers::RangeEquals(expected));
	}
}
