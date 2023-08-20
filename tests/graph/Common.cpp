//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>

#include "Defines.hpp"

#include "Simple-Utility/graph/Common.hpp"

// ReSharper disable CppDeclaratorNeverUsed
// ReSharper disable CppTypeAliasNeverUsed
// ReSharper disable CppClangTidyClangDiagnosticUnusedMemberFunction
// ReSharper disable CppClangTidyClangDiagnosticUnneededMemberFunction

namespace
{
	struct non_equality_comparable
	{
		bool operator ==(const non_equality_comparable&) const = delete;
		auto operator <=>(const non_equality_comparable&) const = default;
	};

	struct non_copyable  // NOLINT(cppcoreguidelines-special-member-functions)
	{
		non_copyable(const non_copyable&) = delete;
		non_copyable& operator =(const non_copyable&) = delete;
		auto operator <=>(const non_copyable&) const = default;
	};

	struct valid_vertex
	{
		friend bool operator==(const valid_vertex&, const valid_vertex&) = default;
		// NOLINT(clang-diagnostic-unneeded-internal-declaration)
	};

	struct non_totally_ordered
	{
		bool operator ==(const non_totally_ordered&) const = default;
	};

	struct non_mutable_plus_rank
	{
		auto operator <=>(const non_mutable_plus_rank&) const = default;
		non_mutable_plus_rank operator +([[maybe_unused]] const non_mutable_plus_rank&) const;
	};

	struct non_immutable_plus_rank
	{
		auto operator <=>(const non_immutable_plus_rank&) const = default;

		non_immutable_plus_rank& operator +=([[maybe_unused]] const non_immutable_plus_rank&);
	};

	struct non_mutable_minus_rank
	{
		auto operator <=>(const non_mutable_minus_rank&) const = default;

		non_mutable_minus_rank operator +([[maybe_unused]] const non_mutable_minus_rank&) const;
	};

	struct non_immutable_minus_rank
	{
		auto operator <=>(const non_immutable_minus_rank&) const = default;
		non_immutable_minus_rank& operator +=([[maybe_unused]] const non_immutable_minus_rank&);
	};

	struct valid_rank
	{
		auto operator <=>(const valid_rank&) const = default;
		valid_rank& operator +=([[maybe_unused]] const valid_rank&);
		valid_rank operator +([[maybe_unused]] const valid_rank&) const;
		valid_rank& operator -=([[maybe_unused]] const valid_rank&);
		valid_rank operator -([[maybe_unused]] const valid_rank&) const;
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
	"graph::concepts::rank determines whether the given type can be used as rank type.",
	"[graph][graph::concepts]",
	((bool expected, class T), expected, T),
	(true, int),
	(true, float),
	(true, valid_rank),
	(false, const int),
	(false, int&),
	(false, non_equality_comparable),
	(false, non_copyable),
	(false, non_totally_ordered),
	(false, non_mutable_plus_rank),
	(false, non_immutable_plus_rank),
	(false, non_mutable_minus_rank),
	(false, non_immutable_minus_rank)
)
{
	STATIC_REQUIRE(expected == sg::concepts::rank<T>);
}

TEMPLATE_TEST_CASE_SIG(
	"graph::concepts::feature_category determines whether the given type denotes a feature category.",
	"[graph][graph::concepts]",
	((bool expected, class T), expected, T),
	(false, int),
	(true, sg::basic_feature_category),
	(true, sg::ranked_feature_category)
)
{
	STATIC_REQUIRE(expected == sg::concepts::feature_category<T>);
}

TEMPLATE_TEST_CASE_SIG(
	"graph::common_feature_category trait yields the strictest category.",
	"[graph][graph::traits]",
	((bool dummy, class Expected, class... Ts), dummy, Expected, Ts...),
	(true, sg::basic_feature_category, sg::basic_feature_category),
	(true, sg::ranked_feature_category, sg::ranked_feature_category),
	(true, sg::basic_feature_category, sg::ranked_feature_category, sg::basic_feature_category),
	(true, sg::basic_feature_category, sg::ranked_feature_category, sg::basic_feature_category, sg::ranked_feature_category)
)
{
	STATIC_REQUIRE(std::same_as<Expected, typename sg::common_feature_category<Ts...>::type>);
	STATIC_REQUIRE(std::same_as<Expected, sg::common_feature_category_t<Ts...>>);
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
