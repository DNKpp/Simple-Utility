//          Copyright Dominic Koepke 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch.hpp>

#include "helper.hpp"

#include "Simple-Utility/unique_handle.hpp"

using namespace sl;

namespace
{
	struct delete_action_mock
	{
		int* invoke_counter{};

		constexpr void operator ()(auto&) noexcept
		{
			if (invoke_counter)
				++(*invoke_counter);
		}
	};

	using test_handle = unique_handle<int, delete_action_mock>;
}

TEST_CASE("unique_handle should be default constructible.", "[unique_handle]")
{
	STATIC_REQUIRE(std::default_initializable<test_handle>);
}

TEST_CASE("unique_handle should neither be copy constructible nor assignable.", "[unique_handle]")
{
	STATIC_REQUIRE(!std::copy_constructible<test_handle>);
	STATIC_REQUIRE(!std::assignable_from<test_handle&, const test_handle&>);
}

TEST_CASE("default constructed unique_handle should not contain a value.", "[unique_handle]")
{
	constexpr test_handle handle{};

	STATIC_REQUIRE(!handle.is_valid());
	STATIC_REQUIRE(!handle);
}

TEST_CASE("unique_handle should be explicitly null constructible by nullhandle.", "[unique_handle]")
{
	constexpr test_handle handle{ nullhandle };

	STATIC_REQUIRE(!handle.is_valid());
	STATIC_REQUIRE(!handle);
}

TEST_CASE("unique_handle should be assignable by nullhandle.", "[unique_handle]")
{
	SL_UNIQUE_HANDLE_FULL_CONSTEXPR
	const test_handle handle = []
	{
		// ReSharper disable once CppInitializedValueIsAlwaysRewritten
		test_handle temp{};
		temp = nullhandle;
		return temp;
	}();

	REQUIRE(!handle.is_valid());
	REQUIRE(!handle);
}

TEST_CASE("unique_handle should be constructible by value.", "[unique_handle]")
{
	constexpr test_handle handle{ 42 };

	STATIC_REQUIRE(handle.is_valid());
	STATIC_REQUIRE(handle);
}

TEST_CASE("unique_handle should be assignable by value.", "[unique_handle]")
{
	SL_UNIQUE_HANDLE_FULL_CONSTEXPR
	test_handle handle = []
	{
		// ReSharper disable once CppInitializedValueIsAlwaysRewritten
		test_handle temp{};
		temp = 42;
		return temp;
	}();

	REQUIRE(handle.is_valid());
	REQUIRE(handle);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"delete action on assignment must only be invoked if unique_handle holds a value.",
	"[unique_handle]",
	((auto VInit, auto VAssign, bool VExpected), VInit, VAssign, VExpected),
	(42, 1337, true),
	(42, nullhandle, true),
	(nullhandle, 1337, false),
	(nullhandle, nullhandle, false)
)
#pragma warning(disable: 26444)
{
	SL_UNIQUE_HANDLE_FULL_CONSTEXPR
	const bool result = []
	{
		int counter{};
		test_handle temp{ VInit };
		temp.delete_action() = delete_action_mock{ .invoke_counter = &counter };
		temp = VAssign;
		return counter == 1;
	}();

	REQUIRE(result == VExpected);
}

TEST_CASE("unique_handle should be move constructible and invalidate the source.", "[unique_handle]")
{
	SL_UNIQUE_HANDLE_FULL_CONSTEXPR
	const bool result = []
	{
		// ReSharper disable once CppInitializedValueIsAlwaysRewritten
		test_handle source{ 42 };
		test_handle target{ std::move(source) };
		return !source.is_valid() && target.is_valid();
	}();

	REQUIRE(result);
}

TEST_CASE("unique_handle should be move constructible and receive the value of other.", "[unique_handle]")
{
	SL_UNIQUE_HANDLE_FULL_CONSTEXPR
	const bool result = []
	{
		// ReSharper disable once CppInitializedValueIsAlwaysRewritten
		test_handle source{ 42 };
		const test_handle target{ std::move(source) };
		return target.raw() == 42;
	}();

	REQUIRE(result);
}

TEST_CASE("unique_handle should be move assignable and invalidate the source.", "[unique_handle]")
{
	SL_UNIQUE_HANDLE_FULL_CONSTEXPR
	const bool result = []
	{
		test_handle source{ 42 };
		// ReSharper disable once CppInitializedValueIsAlwaysRewritten
		test_handle target{ nullhandle };
		target = std::move(source);
		return !source.is_valid() && target.is_valid();
	}();

	REQUIRE(result);
}

TEST_CASE("unique_handle should be move assignable and receive the value of other.", "[unique_handle]")
{
	SL_UNIQUE_HANDLE_FULL_CONSTEXPR
	const bool result = []
	{
		test_handle source{ 1337 };
		// ReSharper disable once CppInitializedValueIsAlwaysRewritten
		test_handle target{ nullhandle };
		target = std::move(source);
		return target.raw() == 1337;
	}();

	REQUIRE(result);
}

TEST_CASE("moving unique_handle with itself should change nothing.", "[unique_handle]")
{
	SL_UNIQUE_HANDLE_FULL_CONSTEXPR
	const bool result = []
	{
		test_handle handle{ 1337 };
		handle = std::move(handle);
		return handle.is_valid() && *handle == 1337;
	}();

	REQUIRE(result);
}

TEST_CASE("swapping unique_handle with itself should change nothing.", "[unique_handle]")
{
	SL_UNIQUE_HANDLE_FULL_CONSTEXPR
	const bool result = []
	{
		test_handle handle{ 1337 };
		handle.swap(handle);
		return handle.is_valid() && *handle == 1337;
	}();

	REQUIRE(result);
}

TEST_CASE("unique_handle should be swapable.", "[unique_handle]")
{
	SL_UNIQUE_HANDLE_FULL_CONSTEXPR
	const bool result = []
	{
		test_handle lhs{ 1337 };
		test_handle rhs{ 42 };
		lhs.swap(rhs);
		return *lhs == 42 && *rhs == 1337;
	}();

	REQUIRE(result);
}

TEST_CASE("unique_handle::raw should expose a const reference of its value.", "[unique_handle]")
{
	constexpr bool result = []
	{
		test_handle handle{ 1337 };
		const int& ref{ handle.raw() };
		return ref == 1337;
	}();

	STATIC_REQUIRE(result);
}

TEST_CASE("unique_handle::raw should throw bad_handle_access if no value is hold.", "[unique_handle]")
{
	constexpr test_handle handle{};

	REQUIRE_THROWS_AS(handle.raw(), bad_handle_access);
}

TEST_CASE("unique_handle's operator * should expose a const reference of its value.", "[unique_handle]")
{
	constexpr bool result = []
	{
		test_handle handle{ 42 };
		const int& ref{ *handle };
		return ref == 42;
	}();

	REQUIRE(result);
}

TEMPLATE_TEST_CASE
(
	"unique_handle's operator -> overload should expose a pointer to its value.",
	"[unique_handle]",
	test_handle,
	const test_handle
)
{
	constexpr bool result = []
	{
		TestType handle{ 1337 };
		auto* ptr = handle.operator ->();
		return *ptr == 1337;
	}();

	STATIC_REQUIRE(result);
}

TEST_CASE("unique_handle::reset should reset to a nullhandle.", "[unique_handle]")
{
	SL_UNIQUE_HANDLE_FULL_CONSTEXPR
	const test_handle handle = []
	{
		test_handle temp{ 42 };
		temp.reset();
		return temp;
	}();

	REQUIRE(!handle);
}

TEST_CASE("resetting a handle without value should do nothing.", "[unique_handle]")
{
	REQUIRE_NOTHROW
	(
		[]
		{
		test_handle temp{ nullhandle };
		temp.reset();
		}()
	);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"delete action on reset must only be invoked if unique_handle holds a value.",
	"[unique_handle]",
	((auto VInit, bool VExpected), VInit, VExpected),
	(42, true),
	(nullhandle, false)
)
#pragma warning(disable: 26444)
{
	SL_UNIQUE_HANDLE_FULL_CONSTEXPR
	const bool result = []
	{
		int counter{};
		test_handle temp{ VInit };
		temp.delete_action() = delete_action_mock{ .invoke_counter = &counter };
		temp.reset();
		return counter == 1;
	}();

	REQUIRE(result == VExpected);
}

TEST_CASE("unique_handle::delete_action should return a reference to the used deleter action object.", "[unique_handle]")
{
	constexpr bool result = []
	{
		test_handle temp{ 42 };

		return &temp.delete_action() == &std::as_const(temp).delete_action();
	}();

	STATIC_REQUIRE(result);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"delete action on destruction must only be invoked if unique_handle holds a value.",
	"[unique_handle]",
	((auto VInit, bool VExpected), VInit, VExpected),
	(42, true),
	(nullhandle, false)
)
#pragma warning(disable: 26444)
{
	constexpr bool result = []
	{
		int counter{};
		{
			test_handle temp{ VInit };
			temp.delete_action() = delete_action_mock{ .invoke_counter = &counter };
		}
		return counter == 1;
	}();

	REQUIRE(result == VExpected);
}
