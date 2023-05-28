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
		return derived();
	}

	constexpr decltype(auto) access() &
	{
		return derived();
	}
};

TEMPLATE_TEST_CASE(
	"CRTPBase offers cast() functions for all value and reference combination.",
	"[CRTPBase]",
	CRTPTest&,
	const CRTPTest&
)
{
	CRTPTest instance{};

	TestType ref = static_cast<TestType&>(instance).access();

	REQUIRE(&instance == &ref);
}
