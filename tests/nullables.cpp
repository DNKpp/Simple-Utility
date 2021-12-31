//          Copyright Dominic Koepke 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch.hpp>

#include "helper.hpp"

#include "Simple-Utility/nullables.hpp"
#include "Simple-Utility/unique_handle.hpp"

#include <memory>
#include <optional>

namespace
{
	struct target_t
	{
		int x{};

		[[nodiscard]]
		constexpr bool operator ==(empty_t) const noexcept
		{
			return false;
		}
	};

	[[nodiscard]]
	constexpr const int& value_unchecked(const target_t& target) noexcept
	{
		return target.x;
	}
}

template <>
struct sl::nullables::nullable_traits<target_t>
{
	using value_type = int;
	constexpr static empty_t null{};
};

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"nullable_value_t should expose expected value type.",
	"[nullables]",
	// VIgnore is necessary to disambiguate the internal catch overload
	((class TNullable, class TExpectedValueType, auto VIgnore), TNullable, TExpectedValueType, VIgnore),
	(sl::unique_handle<int>, int, false),
	(sl::unique_handle<int>&&, int, false),
	(const sl::unique_handle<int>&&, int, false),
	(sl::unique_handle<int>&, int, false),
	(const sl::unique_handle<int>&, int, false),
	(const sl::unique_handle<sl::unique_handle<int>>&, sl::unique_handle<int>, false),

	(std::optional<float>, float, false),
	(int*, int, false),
	(std::unique_ptr<int>, int, false),
	(std::shared_ptr<int>, int, false),
	(target_t, int, false) // check for external customizability
)
#pragma warning(default: 26444)
{
	using sl::nullables::nullable_value_t;

	REQUIRE(std::same_as<nullable_value_t<TNullable>, TExpectedValueType>);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"nullable_null_v should expose expected null object.",
	"[nullables]",
	// VIgnore is necessary to disambiguate the internal catch overload
	((class TNullable, class TExpectedNullType, auto VIgnore), TNullable, TExpectedNullType, VIgnore),
	(sl::unique_handle<int>, sl::nullhandle_t, false),
	(sl::unique_handle<int>&&, sl::nullhandle_t, false),
	(const sl::unique_handle<int>&&, sl::nullhandle_t, false),
	(sl::unique_handle<int>&, sl::nullhandle_t, false),
	(const sl::unique_handle<int>&, sl::nullhandle_t, false),
	(const sl::unique_handle<sl::unique_handle<int>>&, sl::nullhandle_t, false),

	(std::optional<float>, std::nullopt_t, false),
	(int*, std::nullptr_t, false),
	(std::unique_ptr<int>, std::nullptr_t, false),
	(std::shared_ptr<int>, std::nullptr_t, false),
	(target_t, empty_t, false) // check for external customizability
)
#pragma warning(default: 26444)
{
	using sl::nullables::nullable_null_v;

	REQUIRE(std::same_as<std::decay_t<decltype(nullable_null_v<TNullable>)>, TExpectedNullType>);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"nullable should determine whether T is a nullable",
	"[nullables]",
	((class T, bool VExpectedResult), T, VExpectedResult),
	(int, false),
	(int*, true),
	(sl::unique_handle<int>, true),
	(std::optional<float>, true),
	(std::unique_ptr<int>, true),
	(std::shared_ptr<int>, true),
	(target_t, true)
)
#pragma warning(default: 26444)
{
	using sl::nullables::nullable;

	REQUIRE(nullable<T> == VExpectedResult);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE
(
	"value_unchecked should use the expected overload",
	"[nullables]",
	sl::unique_handle<int>,
	std::optional<int>,
	target_t
)
#pragma warning(default: 26444)
{
	using sl::nullables::value_unchecked;

	constexpr TestType object{ 42 };

	STATIC_REQUIRE(value_unchecked(object) == 42);
}

TEST_CASE("or_else should return the expected object when received as rvalue ref","[nullables][algorithm]")
{
	using sl::nullables::or_else;

	constexpr auto func = [] { return 1337; };
	sl::unique_handle<int> handle{};

	SECTION("when handle is empty, the invoke result should be returned")
	{
		handle = std::move(handle) | or_else{ func };
		REQUIRE(handle == 1337);
	}

	SECTION("when handle is non-empty, the handle itself should be returned")
	{
		handle = 42;
		handle = std::move(handle) | or_else{ func };
		REQUIRE(handle == 42);
	}
}

TEST_CASE("or_else should return the expected object when received as const lvalue ref", "[nullables][algorithm]")
{
	using sl::nullables::or_else;

	constexpr auto func = [] { return 1337; };
	std::optional<int> opt{};

	SECTION("when optional is empty, the invoke result should be returned")
	{
		opt = std::as_const(opt) | or_else{ func };
		REQUIRE(opt == 1337);
	}

	SECTION("when optional is non-empty, the optional itself should be returned")
	{
		opt = 42;
		opt = std::as_const(opt) | or_else{ func };
		REQUIRE(opt == 42);
	}
}

TEST_CASE("or_else should return the expected object when used in a chain", "[nullables][algorithm]")
{
	using sl::nullables::or_else;
	std::optional<int> opt{};

	SECTION("when optional is empty, the first non empty invoke result should be returned")
	{
		opt = opt | or_else{ [] { return std::nullopt; } }
			| or_else{ [] { return 42; } };

		REQUIRE(opt == 42);
	}

	SECTION("when optional is non-empty, the optional itself should be returned")
	{
		opt = 1337;
		opt = opt | or_else{ [] { return std::nullopt; } }
			| or_else{ [] { return 42; } };

		REQUIRE(opt == 1337);
	}
}

//
//#pragma warning(disable: 26444)
//TEMPLATE_TEST_CASE_SIG
//(
//	"has_value should use the expected overload",
//	"[algorithm]",
//	((class T, auto VInit, auto VAlt, auto VExpectedValue), T, VInit, VAlt, VExpectedValue),
//	(sl::unique_handle<int>, sl::nullhandle, 42, 42),
//	(sl::unique_handle<int>, 1337, 42, 1337)/*,
//	(std::optional<int>, std::nullopt, 42, 42)*/
//)
//#pragma warning(default: 26444)
//{
//	auto value = []
//	{
//		T object{ VInit };
//		return sl::nullables::value_or(std::move(object), VAlt);
//	}();
//
//	REQUIRE(value == VExpectedValue);
//}
