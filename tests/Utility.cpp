#include "Simple-Utility/Utility.hpp"

#include <catch2/catch_template_test_macros.hpp>

#include <concepts>

TEMPLATE_TEST_CASE_SIG(
	"Higher priority_tag always inherit from the lower ones.",
	"[utility]",
	((bool expected, class Derived, class Base), expected, Derived, Base),
	(true, sl::priority_tag<10>, sl::priority_tag<0>),	
	(true, sl::priority_tag<10>, sl::priority_tag<1>),	
	(true, sl::priority_tag<1>, sl::priority_tag<0>),
	(true, sl::priority_tag<1>, sl::priority_tag<1>),
	(false, sl::priority_tag<0>, sl::priority_tag<1>),
	(false, sl::priority_tag<1>, sl::priority_tag<10>)
)
{
	STATIC_REQUIRE(expected == std::derived_from<Derived, Base>);
}
