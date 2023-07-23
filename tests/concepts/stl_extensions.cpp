//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>

#include "../helper.hpp"

#include <optional>

#include "Simple-Utility/concepts/stl_extensions.hpp"

using namespace sl::concepts;

namespace
{
	struct target_t
	{
		target_t() = default;

		// ReSharper disable once CppNonExplicitConvertingConstructor
		target_t(int)
		{ }

		target_t& operator =(int)
		{
			return *this;
		}
	};

	template <bool VNoexcept>
	struct comparable_type
	{
		constexpr bool operator ==(const comparable_type&) const noexcept(VNoexcept) = default;

		constexpr bool operator ==(const comparable_type<!VNoexcept>&) const noexcept(false) { return true; }
	};
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG(
	"initializes should behave as the symmetrical counterpart of std::constructible_from with one constructor argument",
	"[concepts][stl_ext]",
	((class TSource, class TTarget, bool VExpected), TSource, TTarget, VExpected),
	(int, int, true),
	(int, target_t, true),
	(target_t, int, false)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(initializes<TSource, TTarget> == VExpected);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG(
	"assignable_to should behave as the symmetrical counterpart of std::assignable_from.",
	"[concepts][stl_ext]",
	((class TSource, class TTarget, bool VExpected), TSource, TTarget, VExpected),
	(int, int&, true),
	(int, target_t&, true),
	(int, target_t, false),
	(target_t, int&, false)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(assignable_to<TSource, TTarget> == VExpected);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG(
	"not_same_as should behave as the inverted counterpart of std::same_as.",
	"[concepts][stl_ext]",
	((class TSource, class TTarget, bool VExpected), TSource, TTarget, VExpected),
	(int, int, false),
	(int, target_t, true),
	(target_t&, target_t, true),
	(const target_t&, const target_t&, false)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(not_same_as<TSource, TTarget> == VExpected);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG(
	"weakly_equality_comparable_with Checks whether a symmetrical set of operators == and != to compare both types with each other exists.",
	"[concepts][stl_ext]",
	((class T1, class T2, bool VExpected), T1, T2, VExpected),
	(target_t, int, false),
	(int, int, true)
)
#pragma warning(default: 26444)
{
	STATIC_REQUIRE(weakly_equality_comparable_with<T1, T2> == VExpected);
	STATIC_REQUIRE(weakly_equality_comparable_with<T2, T1> == VExpected);
}

TEMPLATE_TEST_CASE_SIG(
	"nothrow_weakly_equality_comparable_with checks whether a symmetrical set of operators == and != to compare both types with each other exists and has noexcept specifier.",
	"[concepts][stl_ext]",
	((class T1, class T2, bool VExpected), T1, T2, VExpected),
	(int, int, true),
	(target_t, int, false),
	// simply not comparable
	(comparable_type<true>, comparable_type<true>, true),
	(comparable_type<false>, comparable_type<true>, false),
	(comparable_type<false>, comparable_type<false>, false)
)
{
	STATIC_REQUIRE(nothrow_weakly_equality_comparable_with<T1, T2> == VExpected);
	STATIC_REQUIRE(nothrow_weakly_equality_comparable_with<T2, T1> == VExpected);
}

struct unary_constructible
{
	explicit unary_constructible([[maybe_unused]] auto&& x) noexcept  // NOLINT(bugprone-forwarding-reference-overload)
	{}
};

struct throwing_unary_constructible
{
	explicit throwing_unary_constructible([[maybe_unused]] auto&& x)  // NOLINT(bugprone-forwarding-reference-overload)
	{}
};

TEMPLATE_TEST_CASE_SIG(
	"explicitly_convertible_to checks whether a type is static_cast convertible to another type.",
	"[concepts][stl_ext]",
	((class TFrom, class TTo, bool VExpected), TFrom, TTo, VExpected),
	(int, int, true),
	(const int&, int, true),
	(int, float, true),
	(const int&, const float&, true),
	(int, std::optional<int>, true),
	(int, unary_constructible, true),
	(int, throwing_unary_constructible, true),
	(int, std::optional<std::string>, false),
	(const std::optional<int>&, int, false)
)
{
	STATIC_REQUIRE(explicitly_convertible_to<TFrom, TTo> == VExpected);
}

TEMPLATE_TEST_CASE_SIG(
	"nothrow_explicitly_convertible_to checks whether a type is static_cast convertible to another type and does not throw.",
	"[concepts][stl_ext]",
	((class TFrom, class TTo, bool VExpected), TFrom, TTo, VExpected),
	(int, int, true),
	(const int&, int, true),
	(int, float, true),
	(const int&, const float&, true),
	(int, unary_constructible, true),
	(int, throwing_unary_constructible, false)
)
{
	STATIC_REQUIRE(nothrow_explicitly_convertible_to<TFrom, TTo> == VExpected);
}

TEMPLATE_TEST_CASE_SIG(
	"pointer behaves exactly as std::is_pointer.",
	"[concepts][stl_ext]",
	((bool expected, class T), expected, T),
	(true, int*),
	(true, int**),
	(true, const int * volatile),
	(true, void*),
	(false, int),
	(false, int&),
	(false, int&&),
	(false, int[10]),
	(false, std::nullptr_t)
)
{
	STATIC_REQUIRE(expected == pointer<T>);
}

TEMPLATE_TEST_CASE_SIG(
	"reference behaves exactly as std::is_reference.",
	"[concepts][stl_ext]",
	((bool expected, class T), expected, T),
	(true, int&),
	(true, int&&),
	(true, const int&),
	(true, const int&&),
	(true, double*&),
	(true, double*&&),
	(false, int),
	(false, int*)
)
{
	STATIC_REQUIRE(expected == reference<T>);
}

TEMPLATE_TEST_CASE_SIG(
	"decayed determines whether T is a decayed type.",
	"[concepts][stl_ext]",
	((bool expected, class T), expected, T),
	(true, int),
	(true, int*),
	(true, int**),
	(true, int(*)(int)),
	(false, int&),
	(false, int&&),
	(false, const int),
	(false, volatile int),
	(false, int[5]),
	(false, int(int))
)
{
	STATIC_REQUIRE(expected == decayed<T>);
}

TEMPLATE_TEST_CASE_SIG(
	"unqualified determines whether T denotes a unqualified type.",
	"[concepts][stl_ext]",
	((bool expected, class T), expected, T),
	(true, int),
	(true, int*),
	(true, int**),
	(true, int(*)(int)),
	(true, int(int)),
	(true, int[5]),
	(false, int&),
	(false, int&&),
	(false, const int),
	(false, volatile int)
)
{
	STATIC_REQUIRE(expected == unqualified<T>);
}

TEMPLATE_TEST_CASE_SIG(
	"comparison_category determines whether the provided type denotes a stl comparison category.",
	"[concepts][stl_ext]",
	((bool expected, class T), expected, T),
	(true, std::strong_ordering),
	(true, std::weak_ordering),
	(true, std::partial_ordering),
	(false, const std::strong_ordering),
	(false, std::strong_ordering&),
	(false, int)
)
{
	STATIC_REQUIRE(expected == comparison_category<T>);
}
