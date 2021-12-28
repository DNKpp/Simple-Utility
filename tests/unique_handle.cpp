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

	STATIC_REQUIRE(!handle.is_valid());
	STATIC_REQUIRE(!handle);
}

TEST_CASE("unique_handle should be explicitly null constructible by nullhandle.", "[unique_handle]")
{
	constexpr unique_handle<int> handle{ nullhandle };

	STATIC_REQUIRE(!handle.is_valid());
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

	REQUIRE(!handle.is_valid());
	REQUIRE(!handle);
}

TEST_CASE("unique_handle should be constructible by value.", "[unique_handle]")
{
	constexpr unique_handle<int> handle{ 42 };

	STATIC_REQUIRE(handle.is_valid());
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

	STATIC_REQUIRE(handle.is_valid());
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

TEST_CASE("unique_handle's operator * should expose a const reference of its value.", "[unique_handle]")
{
	constexpr unique_handle<int> handle{ 42 };

	constexpr const int& ref{ *handle };

	STATIC_REQUIRE(ref == 42);
}

TEST_CASE("unique_handle's operator -> const overload should expose a const pointer to its value.", "[unique_handle]")
{
	constexpr auto access_via_ptr = []<template<class> class THandle>()
	{
		THandle<int> handle{ 1337 };
		auto* ptr = handle.operator ->();
		return *ptr;
	};

	constexpr int non_const_result = access_via_ptr.operator()<unique_handle>();
	constexpr int const_result = access_via_ptr.operator()<const unique_handle>();

	STATIC_REQUIRE(non_const_result == 1337);
	STATIC_REQUIRE(const_result == 1337);
}

TEST_CASE("unique_handle::reset should reset to a nullhandle.", "[unique_handle]")
{
	SL_UNIQUE_HANDLE_FULL_CONSTEXPR
	const unique_handle<int> handle = []
	{
		unique_handle<int> temp{ 42 };
		temp.reset();
		return temp;
	}();

	REQUIRE(!handle);
}
