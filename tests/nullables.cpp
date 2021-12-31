//          Copyright Dominic Koepke 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch.hpp>

#include "Simple-Utility/nullables.hpp"
#include "Simple-Utility/unique_handle.hpp"

#include <optional>

namespace
{
	struct target_t
	{
		int x{};
	};

	[[nodiscard]]
	constexpr const int& value_unchecked(const target_t& target) noexcept
	{
		return target.x;
	}

	[[nodiscard]]
	constexpr bool has_value(const target_t& target) noexcept
	{
		return target.x != 0;
	}
}

template <>
struct sl::nullables::nullable_traits<target_t>
{
	using value_type = int;
	constexpr static auto null{ target_t{ 0 } };
};

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"has_value should use the expected overload",
	"[algorithm]",
	((class T, auto VInit, bool VExpected), T, VInit, VExpected),
	(sl::unique_handle<int>, sl::nullhandle, false),
	(sl::unique_handle<int>, 42, true),
	(std::optional<int>, std::nullopt, false),
	(target_t, 42, true)
)
#pragma warning(default: 26444)
{
	using sl::nullables::has_value;

	constexpr T object{ VInit };

	STATIC_REQUIRE(has_value(object) == VExpected);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE
(
	"value_unchecked should use the expected overload",
	"[algorithm]",
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

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"has_value should use the expected overload",
	"[algorithm]",
	((class T, auto VInit, auto VAlt, auto VExpectedValue), T, VInit, VAlt, VExpectedValue),
	(sl::unique_handle<int>, sl::nullhandle, 42, 42),
	(sl::unique_handle<int>, 1337, 42, 1337),
	(std::optional<int>, std::nullopt, 42, 42),
	(target_t, 1337, 42, 1337)
)
#pragma warning(default: 26444)
{
	constexpr auto value = []
	{
		T object{ VInit };
		return sl::nullables::value_or(std::move(object), VAlt);
	}();

	REQUIRE(value == VExpectedValue);
}
