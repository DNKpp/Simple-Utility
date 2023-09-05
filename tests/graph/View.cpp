//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/graph/View.hpp"

#include <catch2/catch_template_test_macros.hpp>

#include "Defines.hpp"

namespace
{
	template <sg::concepts::vertex Vertex>
	struct GenericBasicView
	{
		using edge_type = GenericBasicEdge<Vertex>;

		template <sg::concepts::node Node>
			requires sg::concepts::edge_for<edge_type, Node>
		// ReSharper disable once CppFunctionIsNotImplemented
		static std::vector<edge_type> edges(const Node&);
	};

	template <sg::concepts::vertex Vertex, sg::concepts::weight Weight>
	struct GenericWeightedView
	{
		using edge_type = GenericWeightedEdge<Vertex, Weight>;

		template <sg::concepts::node Node>
			requires sg::concepts::edge_for<edge_type, Node>
		// ReSharper disable once CppFunctionIsNotImplemented
		static std::vector<edge_type> edges(const Node&);
	};
}

TEMPLATE_TEST_CASE_SIG(
	"view::traits extracts edge type.",
	"[graph][graph::view]",
	((bool dummy, class Expected, class Graph), dummy, Expected, Graph),
	(true, GenericBasicEdge<int>, GenericBasicView<int>),
	(true, GenericBasicEdge<std::string>, GenericBasicView<std::string>),
	(true, GenericWeightedEdge<std::string, int>, GenericWeightedView<std::string, int>)
)
{
	STATIC_REQUIRE(std::same_as<Expected, typename sg::view::traits<Graph>::edge_type>);
	STATIC_REQUIRE(std::same_as<Expected, sg::view::edge_t<Graph>>);
}

TEMPLATE_TEST_CASE_SIG(
	"concepts::view_for determines, whether the view type satisfies the minimal requirements of the specified node.",
	"[graph][graph::concepts]",
	((bool expected, class Graph, class Node), expected, Graph, Node),
	(false, GenericBasicView<std::string>, GenericBasicNode<int>),
	(true, GenericBasicView<std::string>, GenericBasicNode<std::string>),
	(true, GenericWeightedView<std::string, int>, GenericBasicNode<std::string>),

	(false, GenericBasicView<std::string>, GenericRankedNode<std::string, int>),
	(false, GenericWeightedView<int, int>, GenericRankedNode<std::string, int>),
	(true, GenericWeightedView<std::string, int>, GenericRankedNode<std::string, int>),

	(true, BasicViewMock<std::string>, GenericBasicNode<std::string>)
)
{
	STATIC_REQUIRE(expected == sg::concepts::view_for<Graph, Node>);
}
