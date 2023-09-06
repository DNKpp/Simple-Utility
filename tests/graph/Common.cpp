//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/graph/Common.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>

#include "Defines.hpp"

// ReSharper disable CppDeclaratorNeverUsed
// ReSharper disable CppTypeAliasNeverUsed
// ReSharper disable CppClangTidyClangDiagnosticUnusedMemberFunction
// ReSharper disable CppClangTidyClangDiagnosticUnneededMemberFunction
// ReSharper disable CppClangTidyClangDiagnosticUnneededInternalDeclaration

namespace
{
	struct non_equality_comparable
	{
		bool operator ==(const non_equality_comparable&) const = delete;
		auto operator <=>(const non_equality_comparable&) const = default;
	};

	struct non_copyable // NOLINT(cppcoreguidelines-special-member-functions)
	{
		non_copyable(const non_copyable&) = delete;
		non_copyable& operator =(const non_copyable&) = delete;
		auto operator <=>(const non_copyable&) const = default;
	};

	struct valid_vertex
	{
		friend bool operator==(const valid_vertex&, const valid_vertex&) = default;
	};

	struct non_totally_ordered
	{
		bool operator ==(const non_totally_ordered&) const = default;
	};

	struct non_mutable_plus
	{
		non_mutable_plus operator +([[maybe_unused]] const non_mutable_plus&) const;
	};

	struct non_immutable_plus
	{
		non_immutable_plus& operator +=([[maybe_unused]] const non_immutable_plus&);
	};

	struct non_mutable_minus
	{
		non_mutable_minus operator +([[maybe_unused]] const non_mutable_minus&) const;
	};

	struct non_immutable_minus
	{
		non_immutable_minus& operator +=([[maybe_unused]] const non_immutable_minus&);
	};

	struct valid_weight
	{
		valid_weight& operator +=([[maybe_unused]] const valid_weight&);
		valid_weight operator +([[maybe_unused]] const valid_weight&) const;
		valid_weight& operator -=([[maybe_unused]] const valid_weight&);
		valid_weight operator -([[maybe_unused]] const valid_weight&) const;
	};

	struct valid_rank
	{
		auto operator <=>(const valid_rank&) const = default;
	};
}

TEMPLATE_TEST_CASE_SIG(
	"graph::concepts::vertex determines whether the given type can be used as vertex type.",
	"[graph][graph::concepts]",
	((bool expected, class T), expected, T),
	(true, int),
	(true, float),
	(true, valid_vertex),
	(false, const int),
	(false, int&),
	(false, non_equality_comparable),
	(false, non_copyable)
)
{
	STATIC_REQUIRE(expected == sg::concepts::vertex<T>);
}

TEMPLATE_TEST_CASE_SIG(
	"graph::concepts::weight determines whether the given type can be used as weight type.",
	"[graph][graph::concepts]",
	((bool expected, class T), expected, T),
	(true, int),
	(true, float),
	(true, valid_weight),
	(false, const int),
	(false, int&),
	(false, non_copyable),
	(false, non_mutable_plus),
	(false, non_immutable_plus),
	(false, non_mutable_minus),
	(false, non_immutable_minus)
)
{
	STATIC_REQUIRE(expected == sg::concepts::weight<T>);
}

TEMPLATE_TEST_CASE_SIG(
	"graph::concepts::rank determines whether the given type can be used as rank type.",
	"[graph][graph::concepts]",
	((bool expected, class T), expected, T),
	(true, int),
	(true, float),
	(true, valid_rank),
	(false, const int),
	(false, int&),
	(false, non_equality_comparable),
	(false, non_copyable)
)
{
	STATIC_REQUIRE(expected == sg::concepts::rank<T>);
}

namespace
{
	struct non_readable_vertex_type
	{
	};

	struct readable_but_unsatisfied_vertex_type
	{
		using vertex_type = non_equality_comparable;
	};

	struct readable_vertex_type
	{
		using vertex_type = valid_vertex;
	};

	struct non_readable_weight_type
	{
	};

	struct readable_but_unsatisfied_weight_type
	{
		using weight_type = non_mutable_plus;
	};

	struct readable_weight_type
	{
		using weight_type = valid_weight;
	};

	struct non_readable_rank_type
	{
	};

	struct readable_but_unsatisfied_rank_type
	{
		using vertex_type = non_totally_ordered;
	};

	struct readable_rank_type
	{
		using rank_type = valid_rank;
	};
}

TEMPLATE_TEST_CASE_SIG(
	"graph::concepts::readable_vertex_type determines whether T contains a \"vertex_type\" member alias.",
	"[graph][graph::concepts]",
	((bool expected, class T), expected, T),
	(false, non_readable_vertex_type),
	(false, readable_but_unsatisfied_vertex_type),
	(true, readable_vertex_type)
)
{
	STATIC_REQUIRE(expected == sg::concepts::readable_vertex_type<T>);
}

TEMPLATE_TEST_CASE_SIG(
	"graph::concepts::readable_weight_type determines whether T contains a \"weight_type\" member alias.",
	"[graph][graph::concepts]",
	((bool expected, class T), expected, T),
	(false, non_readable_weight_type),
	(false, readable_but_unsatisfied_weight_type),
	(true, readable_weight_type)
)
{
	STATIC_REQUIRE(expected == sg::concepts::readable_weight_type<T>);
}

TEMPLATE_TEST_CASE_SIG(
	"graph::concepts::readable_rank_type determines whether T contains a \"rank_type\" member alias.",
	"[graph][graph::concepts]",
	((bool expected, class T), expected, T),
	(false, non_readable_rank_type),
	(false, readable_but_unsatisfied_rank_type),
	(true, readable_rank_type)
)
{
	STATIC_REQUIRE(expected == sg::concepts::readable_rank_type<T>);
}
