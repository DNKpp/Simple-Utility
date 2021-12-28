//          Copyright Dominic Koepke 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch.hpp>

#include "helper.hpp"

#include "unique_handle.hpp"

using namespace sl;

TEST_CASE("unique_handle should be default constructible.", "[unique_handle]")
{
	STATIC_REQUIRE(std::default_initializable<unique_handle<int>>);
}

TEST_CASE("unique_handle should neither be copy constructible nor assignable.", "[unique_handle]")
{
	STATIC_REQUIRE(!std::copy_constructible<unique_handle<int>>);
	STATIC_REQUIRE(!std::assignable_from<unique_handle<int>&, const unique_handle<int>&>);
}

TEST_CASE("default constructed unique_handle should not contain a value.", "[unique_handle]")
{
	constexpr unique_handle<int> handle{};

	STATIC_REQUIRE(!handle.has_value());
	STATIC_REQUIRE(!handle);
}

TEST_CASE("unique_handle should be explicitly null constructible by nullhandle.", "[unique_handle]")
{
	constexpr unique_handle<int> handle{ nullhandle };

	STATIC_REQUIRE(!handle.has_value());
	STATIC_REQUIRE(!handle);
}

TEST_CASE("unique_handle should be assignable by nullhandle.", "[unique_handle]")
{
	SL_UNIQUE_HANDLE_FULL_CONSTEXPR const unique_handle<int> handle = []
	{
		// ReSharper disable once CppInitializedValueIsAlwaysRewritten
		unique_handle<int> temp{};
		temp = nullhandle;
		return temp;
	}();

	REQUIRE(!handle.has_value());
	REQUIRE(!handle);
}

TEST_CASE("unique_handle should be constructible by value.", "[unique_handle]")
{
	constexpr unique_handle<int> handle{ 42 };

	STATIC_REQUIRE(handle.has_value());
	STATIC_REQUIRE(handle);
}

TEST_CASE("unique_handle should be assignable by value.", "[unique_handle]")
{
	constexpr unique_handle<int> handle = []
	{
		// ReSharper disable once CppInitializedValueIsAlwaysRewritten
		unique_handle<int> temp{};
		temp = 42;
		return temp;
	}();

	STATIC_REQUIRE(handle.has_value());
	STATIC_REQUIRE(handle);
}

TEST_CASE("unique_handle::raw should expose a const reference of its value.", "[unique_handle]")
{
	constexpr unique_handle<int> handle{ 1337 };

	constexpr const int& ref{ handle.raw() };

	STATIC_REQUIRE(ref == 1337);
}

TEST_CASE("unique_handle::raw should throw bad_handle_access if no value is hold.", "[unique_handle]")
{
	constexpr unique_handle<int> handle{};

	REQUIRE_THROWS_AS(handle.raw(), bad_handle_access);
}
