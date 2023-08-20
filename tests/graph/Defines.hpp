//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <catch2/catch_test_macros.hpp>
#include <catch2/trompeloeil.hpp>

#include <Simple-Utility/graph/Common.hpp>

namespace sg = sl::graph;

template <sg::concepts::vertex Vertex>
struct BasicTestNode
{
	using vertex_type = Vertex;

	struct Mock
	{
		MAKE_CONST_MOCK1(vertex, vertex_type(const BasicTestNode&));
	} inline static mock{};

	friend constexpr vertex_type vertex(const BasicTestNode& node)
	{
		return mock.vertex(node);
	}
};
