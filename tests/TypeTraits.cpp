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
	(true, const int* volatile, int* volatile),
	(true, const volatile int*, volatile int*),
	(true, const int*, const int*),
	(true, const int* const, int* const),
	(true, const int* const, const int* const),
	(true, const int* volatile, const int* volatile),
	(true, const int* const volatile, const int* const volatile)
)
{
	STATIC_REQUIRE(std::same_as<Expected, typename sl::add_type_const<T>::type>);
	STATIC_REQUIRE(std::same_as<Expected, sl::add_type_const_t<T>>);

	STATIC_REQUIRE(std::same_as<Expected&, typename sl::add_type_const<T&>::type>);
	STATIC_REQUIRE(std::same_as<Expected&, sl::add_type_const_t<T&>>);

	STATIC_REQUIRE(std::same_as<Expected&&, typename sl::add_type_const<T&&>::type>);
	STATIC_REQUIRE(std::same_as<Expected&&, sl::add_type_const_t<T&&>>);

	STATIC_REQUIRE(std::same_as<Expected*, typename sl::add_type_const<T*>::type>);
	STATIC_REQUIRE(std::same_as<Expected*, sl::add_type_const_t<T*>>);

	STATIC_REQUIRE(std::same_as<Expected* const, typename sl::add_type_const<T* const>::type>);
	STATIC_REQUIRE(std::same_as<Expected* const, sl::add_type_const_t<T* const>>);

	STATIC_REQUIRE(std::same_as<Expected* volatile, typename sl::add_type_const<T* volatile>::type>);
	STATIC_REQUIRE(std::same_as<Expected* volatile, sl::add_type_const_t<T* volatile>>);

	STATIC_REQUIRE(std::same_as<Expected* const volatile, typename sl::add_type_const<T* const volatile>::type>);
	STATIC_REQUIRE(std::same_as<Expected* const volatile, sl::add_type_const_t<T* const volatile>>);
}

TEST_CASE("Explicit examples for the documenation of add_type_const.", "[type_traits][example]")
{
	//! [add_type_const]
	STATIC_REQUIRE(std::same_as<int&, std::add_const_t<int&>>);
	STATIC_REQUIRE(std::same_as<const int&, sl::add_type_const_t<int&>>);

	STATIC_REQUIRE(std::same_as<int* const, std::add_const_t<int*>>);
	STATIC_REQUIRE(std::same_as<const int*, sl::add_type_const_t<int*>>);

	STATIC_REQUIRE(std::same_as<const int* const, std::add_const_t<const int*>>);
	STATIC_REQUIRE(std::same_as<const int*, sl::add_type_const_t<const int*>>);

	STATIC_REQUIRE(std::same_as<int* const, std::add_const_t<int* const>>);
	STATIC_REQUIRE(std::same_as<const int* const, sl::add_type_const_t<int* const>>);
	//! [add_type_const]
}