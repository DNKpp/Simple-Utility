//          Copyright Dominic Koepke 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch.hpp>

#include "../helper.hpp"

#include "Simple-Utility/concepts/stl_symmetrical.hpp"

using namespace sl::concepts;

namespace
{
	struct target_t
	{
		target_t() = default;
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
	"constructs should behave as the symmetrical counterpart of std::constructible_from with one constructor argument",
	"[concepts][stl_ext]",
	((class TSource, class TTarget, bool VExpected), TSource, TTarget, VExpected),
	(int, int, true),
	(int, target_t, true),
	(target_t, int, false)
)
#pragma warning(default: 26444)
{
	REQUIRE(constructs<TSource, TTarget> == VExpected);
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

