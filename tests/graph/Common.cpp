//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>

#include "Defines.hpp"

#include "Simple-Utility/graph/Common.hpp"

namespace
{
	struct non_equality_comparable
	{
		bool operator ==(const non_equality_comparable&) const = delete;
		auto operator <=>(const non_equality_comparable&) const = default;
	};

	struct non_copyable
	{
		non_copyable(const non_copyable&) = delete;
		non_copyable& operator =(const non_copyable&) = delete;
		auto operator <=>(const non_copyable&) const = default;
	};

	struct valid_vertex
	{
		friend bool operator==(const valid_vertex&, const valid_vertex&) = default;
	};
}

TEMPLATE_TEST_CASE_SIG(
	"graph::concepts::vertex determines whether the given type can be used as vertex type.",
	"[graph][graph::concepts]",
	((bool expected, class T), expected, T),
	(true, int),
	(true, float),
	(true, valid_vertex),
	(false, const int),
	(false, int&),
	(false, non_equality_comparable),
	(false, non_copyable)
)
{
	STATIC_REQUIRE(expected == sg::concepts::vertex<T>);
}

