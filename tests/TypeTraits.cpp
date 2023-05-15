#include "Simple-Utility/TypeTraits.hpp"

#include <catch2/catch_template_test_macros.hpp>

TEMPLATE_TEST_CASE_SIG(
	"add_type_const applies the const qualification in any value category.",
	"[type_traits]",
	((bool dummy, class Expected, class T), dummy, Expected, T),
	(true, const int, int),
	(true, const int, const int),
	(true, const volatile int, const volatile int),
	(true, const int*, int*),
	(true, const int*, const int*),
	(true, const int* const, int* const),
	(true, const int* const, const int* const),
	(true, const int&, int&),
	(true, const int&, const int&),
	(true, const int&&, int&&),
	(true, const int&&, const int&&)
)
{
	STATIC_REQUIRE(std::same_as<Expected, typename sl::add_type_const<T>::type>);
	STATIC_REQUIRE(std::same_as<Expected, sl::add_type_const_t<T>>);
}
