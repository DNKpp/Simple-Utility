//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>

#include <optional>

#include "Simple-Utility/functional/utility.hpp"

using namespace sl::functional;

template <class T>
struct explicitly_constructible
{
	T t{};

	explicit explicitly_constructible(T t)
		: t{ t }
	{}

	[[nodiscard]]
	bool operator ==(T other) const
	{
		return t == other;
	}
};

TEMPLATE_TEST_CASE_SIG(
	"as casts the given argument when invoked.",
	"[functional][transform][utility]",
	((class TTarget, auto VSource, auto VExpected), TTarget, VSource, VExpected),
	(int, 42ul, 42),
	(char, 42, '*'),
	(std::optional<int>, 3, 3),
	(explicitly_constructible<int>, 1337, 1337)
)
{
	STATIC_REQUIRE(std::same_as<TTarget, decltype(util::as<TTarget>(VSource))>);
	REQUIRE(util::as<TTarget>(VSource) == VExpected);
}

TEST_CASE(
	"util::dereference uses the dereferencing operator on the argument and returns the result.",
	"[functional][transform][utility]"
)
{
	std::optional<int> opt{ 42 };

	REQUIRE(util::dereference(opt) == 42);
	REQUIRE(util::dereference(std::as_const(opt)) == 42);
	REQUIRE(util::dereference(std::move(opt)) == 42);
}

TEST_CASE("util::addressof returns the address of the given argument.", "[functional][transform][utility]")
{
	int value{ 1337 };

	REQUIRE(util::addressof(value) == &value);
	REQUIRE(util::addressof(std::as_const(value)) == &value);
}
