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
	((bool dummy, class Expected, class T, class... Others), dummy, Expected, T, Others...),
	(true, sg::basic_feature_category, sg::basic_feature_category),
	(true, sg::ranked_feature_category, sg::ranked_feature_category),
	(true, sg::basic_feature_category, sg::ranked_feature_category, sg::basic_feature_category),
	(true, sg::basic_feature_category, sg::ranked_feature_category, sg::basic_feature_category, sg::ranked_feature_category)
)
{
	STATIC_REQUIRE(std::same_as<Expected, typename sg::common_feature_category<T, Others...>::type>);
	STATIC_REQUIRE(std::same_as<Expected, sg::common_feature_category_t<T, Others...>>);
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

	struct ranked_feature_category_type
	{
		using vertex_type = valid_vertex;
		using rank_type = valid_rank;
	};

	struct type_with_custom_trait
	{
	};
}

template <>
struct sg::feature_traits<type_with_custom_trait>
{
	using category_type = ranked_feature_category;
	using vertex_type = int;
	using rank_type = float;
};

template <>
struct sg::customize::vertex_fn<custom_fun_vertex>
{
	[[nodiscard]]
	decltype(auto) operator ()(const custom_fun_vertex& e) const
	{
		return e.my_vertex();
	}
};


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

TEST_CASE(
	"graph::feature_traits categorizes T as basic_feature_category, if just vertex_type is readable.",
	"[graph][graph::traits]"
)
{
	using TestType = readable_vertex_type;

	STATIC_REQUIRE(std::same_as<sg::basic_feature_category, sg::feature_traits<TestType>::category_type>);
	STATIC_REQUIRE(std::same_as<sg::basic_feature_category, sg::feature_category_t<TestType>>);

	STATIC_REQUIRE(std::same_as<valid_vertex, sg::feature_traits<TestType>::vertex_type>);
	STATIC_REQUIRE(std::same_as<valid_vertex, sg::feature_vertex_t<TestType>>);
}

TEST_CASE(
	"graph::feature_traits categorizes T as ranked_feature_category, if vertex_type and rank_type are readable.",
	"[graph][graph::traits]"
)
{
	using TestType = ranked_feature_category_type;

	STATIC_REQUIRE(std::same_as<sg::ranked_feature_category, sg::feature_traits<TestType>::category_type>);
	STATIC_REQUIRE(std::same_as<sg::ranked_feature_category, sg::feature_category_t<TestType>>);

	STATIC_REQUIRE(std::same_as<valid_vertex, sg::feature_traits<TestType>::vertex_type>);
	STATIC_REQUIRE(std::same_as<valid_vertex, sg::feature_vertex_t<TestType>>);

	STATIC_REQUIRE(std::same_as<valid_rank, sg::feature_traits<TestType>::rank_type>);
	STATIC_REQUIRE(std::same_as<valid_rank, sg::feature_rank_t<TestType>>);
}

TEST_CASE(
	"graph::feature_traits can be specialized.",
	"[graph][graph::traits]"
)
{
	using TestType = type_with_custom_trait;

	STATIC_REQUIRE(std::same_as<sg::ranked_feature_category, sg::feature_traits<TestType>::category_type>);
	STATIC_REQUIRE(std::same_as<sg::ranked_feature_category, sg::feature_category_t<TestType>>);

	STATIC_REQUIRE(std::same_as<int, sg::feature_traits<TestType>::vertex_type>);
	STATIC_REQUIRE(std::same_as<int, sg::feature_vertex_t<TestType>>);

	STATIC_REQUIRE(std::same_as<float, sg::feature_traits<TestType>::rank_type>);
	STATIC_REQUIRE(std::same_as<float, sg::feature_rank_t<TestType>>);
}

TEMPLATE_TEST_CASE(
	"Listed types yield at least basic_feature_category.",
	"[graph][graph::traits]",
	BasicTestNode<std::string>,
	(RankedTestNode<std::string, int>),
	(BasicGraph<BasicTestNode<std::string>>::info)
)
{
	STATIC_REQUIRE(std::same_as<
		sg::basic_feature_category,
		sg::common_feature_category_t<sg::basic_feature_category, sg::feature_category_t<TestType>>>);
}

TEMPLATE_TEST_CASE(
	"Listed types yield at least ranked_feature_category.",
	"[graph][graph::traits]",
	(RankedTestNode<std::string, int>)
)
{
	STATIC_REQUIRE(std::same_as<
		sg::ranked_feature_category,
		sg::common_feature_category_t<sg::ranked_feature_category, sg::feature_category_t<TestType>>>);
}

TEST_CASE("graph::details::vertex serves as a customization point accessing the vertex.", "[graph][detail]")
{
	constexpr sg::detail::vertex_fn fun{};

	const int expected = GENERATE(take(5, random(0, std::numeric_limits<int>::max())));

	SECTION("Access via the vertex member.")
	{
		REQUIRE(expected == fun(member_vertex{expected}));
	}

	SECTION("Access via the vertex member function.")
	{
		member_fun_vertex mock{};
		REQUIRE_CALL(mock, vertex())
			.RETURN(expected);
		REQUIRE(expected == fun(std::as_const(mock)));
	}

	SECTION("Access via the vertex free function.")
	{
		free_fun_vertex mock{};
		REQUIRE_CALL(mock, my_vertex())
			.RETURN(expected);
		REQUIRE(expected == fun(std::as_const(mock)));
	}

	SECTION("Access via custom function.")
	{
		custom_fun_vertex mock{};
		REQUIRE_CALL(mock, my_vertex())
			.RETURN(expected);
		REQUIRE(expected == fun(std::as_const(mock)));
	}
}
