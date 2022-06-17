//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>

#include "../helper.hpp"

#include "Simple-Utility/concepts/stl_extensions.hpp"

using namespace sl::concepts;

namespace
{
	struct target_t
	{
		target_t() = default;

		// ReSharper disable once CppNonExplicitConvertingConstructor
		target_t(int)
		{
		}

		target_t& operator =(int)
		{
			return *this;
		}
	};
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"initializes should behave as the symmetrical counterpart of std::constructible_from with one constructor argument",
	"[concepts][stl_ext]",
	((class TSource, class TTarget, bool VExpected), TSource, TTarget, VExpected),
	(int, int, true),
	(int, target_t, true),
	(target_t, int, false)
)
#pragma warning(default: 26444)
{
	REQUIRE(initializes<TSource, TTarget> == VExpected);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
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
	REQUIRE(assignable_to<TSource, TTarget> == VExpected);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
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
	REQUIRE(not_same_as<TSource, TTarget> == VExpected);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"weakly_equality_comparable_with Checks whether a symmetrical set of operators == and != to compare both types with each other exists.",
	"[concepts][stl_ext]",
	((class T1, class T2, bool VExpected), T1, T2, VExpected),
	(target_t, int, false),
	(int, int, true)
)
#pragma warning(default: 26444)
{
	REQUIRE(weakly_equality_comparable_with<T1, T2> == VExpected);
	REQUIRE(weakly_equality_comparable_with<T2, T1> == VExpected);
}
