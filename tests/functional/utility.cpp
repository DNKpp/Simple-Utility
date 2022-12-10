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

TEST_CASE("util::derefence uses the dereferencing operator on the argument and returns the result.", "[functional][transform][utility]")
{
	const std::optional<int> opt{42};

	const int value{ util::dereference(opt) };

	REQUIRE(value == 42);
}
