//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/CRTPBase.hpp"

#include <catch2/catch_template_test_macros.hpp>

#include "helper.hpp"

TEMPLATE_LIST_TEST_CASE(
	"CRTPBase offers cast() functions for all value and reference combination.",
	"[CRTPBase]",
	all_ref_mods_list
)
{
	struct CRTPTest
		: public sl::CRTPBase<CRTPTest>
	{
	};

	CRTPTest instance{};

	typename TestType::template type<CRTPTest> ref = TestType::cast(instance).cast();

	REQUIRE(&instance == &ref);
}
