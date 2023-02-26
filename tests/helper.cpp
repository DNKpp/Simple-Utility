//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "helper.hpp"

#include <catch2/catch_template_test_macros.hpp>

TEMPLATE_TEST_CASE_SIG(
	"reference mod traits work as expected.",
	"[helper]",
	((bool VDummy, template <class> class TMod, class TExpected), VDummy, TMod, TExpected),
	(true, as_lvalue_ref_t, int&),
	(true, as_const_lvalue_ref_t, const int&),
	(true, as_rvalue_ref_t, int&&),
	(true, as_const_rvalue_ref_t, const int&&)
)
{
	STATIC_REQUIRE(std::same_as<TExpected, TMod<int>>);

	using type_mode_t = type_mod<TMod>;
	STATIC_REQUIRE(std::same_as<TExpected, typename type_mode_t::template type<int>>);

	int value{42};
	TExpected ref = type_mode_t::cast(value);

	REQUIRE(&value == &ref);
}
