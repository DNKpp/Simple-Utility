//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/graph/Edge.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>

#include "Defines.hpp"

namespace
{
	struct member_weight
	{
		// ReSharper disable once CppDeclaratorNeverUsed
		int weight;
	};

	struct member_fun_weight
	{
		MAKE_CONST_MOCK0(weight, int());
	};

	struct free_fun_weight
	{
		MAKE_CONST_MOCK0(my_weight, int());

		// ReSharper disable once CppDeclaratorNeverUsed
		friend int weight(const free_fun_weight& v)
		{
			return v.my_weight();
		}
	};

	struct custom_fun_weight
	{
		MAKE_CONST_MOCK0(my_weight, int());
	};
}

template <>
struct sg::customize::weight_fn<custom_fun_weight>
{
	[[nodiscard]]
	decltype(auto) operator ()(const custom_fun_weight& e) const
	{
		return e.my_weight();
	}
};

TEST_CASE("graph::edge::weight serves as a customization point accessing the edge weight.", "[graph][graph::edge]")
{
	const int expected = GENERATE(take(5, random(0, std::numeric_limits<int>::max())));

	SECTION("Access via member.")
	{
		REQUIRE(expected == sg::edge::weight(member_weight{expected}));
	}

	SECTION("Access via member function.")
	{
		member_fun_weight mock{};
		REQUIRE_CALL(mock, weight())
			.RETURN(expected);
		REQUIRE(expected == sg::edge::weight(std::as_const(mock)));
	}

	SECTION("Access via free function.")
	{
		free_fun_weight mock{};
		REQUIRE_CALL(mock, my_weight())
			.RETURN(expected);
		REQUIRE(expected == sg::edge::weight(std::as_const(mock)));
	}

	SECTION("Access via custom function.")
	{
		custom_fun_weight mock{};
		REQUIRE_CALL(mock, my_weight())
			.RETURN(expected);
		REQUIRE(expected == sg::edge::weight(std::as_const(mock)));
	}
}

TEST_CASE(
	"Default graph::edge::traits exposes vertex_type, if readable.",
	"[graph][graph::edge]"
)
{
	using TestType = GenericBasicEdge<std::string>;

	STATIC_REQUIRE(std::same_as<std::string, sg::edge::traits<TestType>::vertex_type>);
	STATIC_REQUIRE(std::same_as<std::string, sg::edge::vertex_t<TestType>>);
}

TEST_CASE(
	"Default graph::edge::traits exposes vertex_type and weight_type, if readable.",
	"[graph][graph::edge]"
)
{
	using TestType = GenericWeightedEdge<std::string, int>;

	STATIC_REQUIRE(std::same_as<std::string, sg::edge::traits<TestType>::vertex_type>);
	STATIC_REQUIRE(std::same_as<std::string, sg::edge::vertex_t<TestType>>);

	STATIC_REQUIRE(std::same_as<int, sg::edge::traits<TestType>::weight_type>);
	STATIC_REQUIRE(std::same_as<int, sg::edge::weight_t<TestType>>);
}

TEMPLATE_TEST_CASE_SIG(
	"concepts::edge determines, whether the given type satisfies the requirements.",
	"[graph][graph::concepts]",
	((bool expected, class T), expected, T),
	(false, member_weight),
	(false, member_fun_weight),
	(false, free_fun_weight),
	(false, custom_fun_weight),
	(true, GenericBasicEdge<std::string>),
	(true, GenericWeightedEdge<std::string, int>)
)
{
	STATIC_REQUIRE(expected == sg::concepts::edge<T>);
}

TEMPLATE_TEST_CASE_SIG(
	"concepts::weighted_edge determines, whether the given type satisfies the requirements.",
	"[graph][graph::concepts]",
	((bool expected, class T), expected, T),
	(false, member_weight),
	(false, member_fun_weight),
	(false, free_fun_weight),
	(false, custom_fun_weight),
	(false, GenericBasicEdge<std::string>),
	(true, GenericWeightedEdge<std::string, int>)
)
{
	STATIC_REQUIRE(expected == sg::concepts::weighted_edge<T>);
}

TEMPLATE_TEST_CASE_SIG(
	"concepts::edge_for determines, whether the Edge type satisfies the minimal requirements of the Node type.",
	"[graph][graph::concepts]",
	((bool expected, class Edge, class Node), expected, Edge, Node),
	(false, GenericBasicEdge<std::string>, GenericBasicNode<int>),
	(true, GenericBasicEdge<std::string>, GenericBasicNode<std::string>),
	(true, (GenericWeightedEdge<std::string, int>), GenericBasicNode<std::string>),

	(false, GenericBasicEdge<std::string>, (GenericRankedNode<std::string, int>)),
	(true, (GenericWeightedEdge<std::string, int>), (GenericRankedNode<std::string, int>))
)
{
	STATIC_REQUIRE(expected == sg::concepts::edge_for<Edge, Node>);
}
