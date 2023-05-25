//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/CRTPBase.hpp"

#include <catch2/catch_template_test_macros.hpp>

#include "helper.hpp"

struct CRTPTest
	: public sl::CRTPBase<CRTPTest>
{
	constexpr CRTPTest() = default;

	constexpr decltype(auto) access() const &
	{
		return cast();
	}

	constexpr decltype(auto) access() &
	{
		return cast();
	}

	constexpr decltype(auto) access() const &&
	{
		return std::move(*this).cast();  // NOLINT(performance-move-const-arg)
	}

	constexpr decltype(auto) access() &&
	{
		return std::move(*this).cast();  // NOLINT(performance-move-const-arg)
	}
};

TEMPLATE_LIST_TEST_CASE(
	"CRTPBase offers cast() functions for all value and reference combination.",
	"[CRTPBase]",
	all_ref_mods_list
)
{
	CRTPTest instance{};

	typename TestType::template type<CRTPTest> ref = TestType::cast(instance).access();

	REQUIRE(&instance == &ref);
}
