//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>

#include "Simple-Utility/nullables/base.hpp"

using namespace sl;

namespace
{
	struct test_nullable
	{
	};

	struct test_nullable_null_t
	{
	};
}

template <>
struct nullables::traits<test_nullable>
{
	using value_type = int;
	inline static constexpr test_nullable_null_t null{};
};

TEMPLATE_TEST_CASE_SIG
(
	"value_t should expose expected value type.",
	"[nullables][trait]",
	((bool VDummy, class TNullable, class TExpected), VDummy, TNullable, TExpected),
	(true, int*, int),
	(true, const int* const&, const int),
	(true, test_nullable&, int),
	(true, const test_nullable, int)
)
{
	STATIC_REQUIRE(std::same_as<nullables::value_t<TNullable>, TExpected>);
}

TEMPLATE_TEST_CASE_SIG
(
	"null_v should expose expected null object.",
	"[nullables][trait]",
	((bool VDummy, class TNullable, class TExpected), VDummy, TNullable, TExpected),
	(true, int*, std::nullptr_t),
	(true, const int*&, std::nullptr_t),
	(true, test_nullable&, test_nullable_null_t),
	(true, const test_nullable, test_nullable_null_t)
)
{

	STATIC_REQUIRE(std::same_as<std::remove_cvref_t<decltype(nullables::null_v<TNullable>)>, TExpected>);
}
