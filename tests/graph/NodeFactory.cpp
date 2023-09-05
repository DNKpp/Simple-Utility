//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/graph/NodeFactory.hpp"

#include <catch2/catch_template_test_macros.hpp>

#include "Defines.hpp"

TEMPLATE_TEST_CASE_SIG(
	"concepts::node_factory_for determines, whether the given type satisfies the requirements of a node_factory for the specified node type.",
	"[graph][graph::concepts]",
	((bool expected, class Factory, class Node, class Edge), expected, Factory, Node, Edge),
	(false, GenericBasicNodeFactoryMock<GenericBasicNode<std::string>>, GenericBasicNode<int>, GenericBasicEdge<int>),
	(true, GenericBasicNodeFactoryMock<GenericBasicNode<std::string>>, GenericBasicNode<std::string>, GenericBasicEdge<std::string>),

	(false, GenericRankedNodeFactoryMock<GenericRankedNode<std::string, int>>, GenericRankedNode<std::string, int>, GenericBasicEdge<std::string>),
	(true, GenericRankedNodeFactoryMock<GenericRankedNode<std::string, int>>, GenericRankedNode<std::string, int>, GenericWeightedEdge<std::string, int>)
)
{
	STATIC_CHECK(sg::concepts::node<Node>);
	STATIC_CHECK(sg::concepts::edge<Edge>);

	STATIC_REQUIRE(expected == sg::concepts::node_factory_for<Factory, Node, Edge>);
}
