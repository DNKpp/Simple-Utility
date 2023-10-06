//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/graph/View.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include "Defines.hpp"

namespace
{
	struct member_fun_get_edges
	{
		using edge_type = GenericBasicEdge<std::string>;

		MAKE_CONST_MOCK1(edges, std::vector<edge_type>(const GenericBasicNode<std::string>&));
	};

	struct free_fun_get_edges
	{
		using edge_type = GenericBasicEdge<std::string>;

		MAKE_CONST_MOCK1(get_edges, std::vector<edge_type>(const GenericBasicNode<std::string>&));

		friend std::vector<edge_type> edges(const free_fun_get_edges& obj, const GenericBasicNode<std::string>& node)
		{
			return obj.get_edges(node);
		}
	};

	struct customized_get_edges
	{
		using edge_type = GenericBasicEdge<std::string>;

		MAKE_CONST_MOCK1(get_edges, std::vector<edge_type>(const GenericBasicNode<std::string>&));
	};

	template <sg::concepts::vertex Vertex>
	struct GenericBasicView
	{
		using edge_type = GenericBasicEdge<Vertex>;

		template <sg::concepts::basic_node Node>
			requires sg::concepts::edge_for<edge_type, Node>
		static std::vector<edge_type> edges(const Node&)
		{
			return {};
		}
	};

	static_assert(sg::concepts::view_for<GenericBasicView<std::string>, GenericBasicNode<std::string>>);

	template <sg::concepts::vertex Vertex, sg::concepts::weight Weight>
	struct GenericWeightedView
	{
		using edge_type = GenericWeightedEdge<Vertex, Weight>;

		template <sg::concepts::basic_node Node>
			requires sg::concepts::edge_for<edge_type, Node>
		static std::vector<edge_type> edges(const Node&)
		{
			return {};
		}
	};
}

template <>
struct sl::graph::customize::edges_fn<customized_get_edges>
{
	[[nodiscard]]
	auto operator ()(const customized_get_edges& e, const GenericBasicNode<std::string>& node) const
	{
		return e.get_edges(node);
	}
};

TEST_CASE(
	"graph::view::edges serves as a customization point, returning the outgoing edges of the given node.",
	"[graph][graph::view]")
{
	const GenericBasicNode<std::string> node{"Hello, World!"};
	const std::vector<GenericBasicEdge<std::string>> expected{
		{"Edge0"},
		{"Edge1"},
		{"Edge2"}
	};

	SECTION("Access via the member function.")
	{
		const member_fun_get_edges mock{};
		REQUIRE_CALL(mock, edges(node))
			.RETURN(expected);
		REQUIRE_THAT(sg::view::edges(mock, node), Catch::Matchers::RangeEquals(expected));
	}

	SECTION("Access via the free function.")
	{
		const free_fun_get_edges mock{};
		REQUIRE_CALL(mock, get_edges(node))
			.RETURN(expected);
		REQUIRE_THAT(sg::view::edges(mock, node), Catch::Matchers::RangeEquals(expected));
	}

	SECTION("Access via customized function.")
	{
		const customized_get_edges mock{};
		REQUIRE_CALL(mock, get_edges(node))
			.RETURN(expected);
		REQUIRE_THAT(sg::view::edges(mock, node), Catch::Matchers::RangeEquals(expected));
	}
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
