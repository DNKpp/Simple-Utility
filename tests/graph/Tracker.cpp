//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/graph/Tracker.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>

#include "Defines.hpp"

#include <string>

// ReSharper disable CppDeclaratorNeverUsed

namespace
{
	struct member_fun_set_discovered
	{
		MAKE_MOCK1(set_discovered, bool(int));
	};

	struct free_fun_set_discovered
	{
		MAKE_MOCK1(do_set_discovered, bool(int));

		friend bool set_discovered(free_fun_set_discovered& obj, const int vertex)
		{
			return obj.do_set_discovered(vertex);
		}
	};

	struct member_fun_set_visited
	{
		MAKE_MOCK1(set_visited, void(int));
	};

	struct free_fun_set_visited
	{
		MAKE_MOCK1(do_set_visited, void(int));

		friend void set_visited(free_fun_set_visited& obj, const int vertex)
		{
			return obj.do_set_visited(vertex);
		}
	};
}

TEST_CASE("graph::queue::set_discovered serves as a customization point, modifing the tracker state.", "[graph][graph::tracker]")
{
	const int vertex = GENERATE(take(5, random(0, std::numeric_limits<int>::max())));
	const bool expected = GENERATE(true, false);

	SECTION("Access via the member function.")
	{
		member_fun_set_discovered mock{};
		REQUIRE_CALL(mock, set_discovered(vertex))
			.RETURN(expected);
		REQUIRE(expected == sg::tracker::set_discovered(mock, vertex));
	}

	SECTION("Access via the free function.")
	{
		free_fun_set_discovered mock{};
		REQUIRE_CALL(mock, do_set_discovered(vertex))
			.RETURN(expected);
		REQUIRE(expected == sg::tracker::set_discovered(mock, vertex));
	}
}

TEST_CASE("graph::queue::set_visited serves as a customization point, modifing the tracker state.", "[graph][graph::tracker]")
{
	const int vertex = GENERATE(take(5, random(0, std::numeric_limits<int>::max())));

	SECTION("Access via the member function.")
	{
		member_fun_set_visited mock{};
		REQUIRE_CALL(mock, set_visited(vertex));

		sg::tracker::set_visited(mock, vertex);
	}

	SECTION("Access via the free function.")
	{
		free_fun_set_visited mock{};
		REQUIRE_CALL(mock, do_set_visited(vertex));

		sg::tracker::set_visited(mock, vertex);
	}
}

TEMPLATE_TEST_CASE_SIG(
	"concepts::tracker_for determines, whether the given type can be used to track the visitation state of the specified vertex.",
	"[graph][graph::concepts][graph::tracker]",
	((bool expected, class T, class Vertex), expected, T, Vertex),
	(false, member_fun_set_discovered, int),
	(false, free_fun_set_discovered, int),
	(false, member_fun_set_visited, int),
	(false, free_fun_set_visited, int),
	(false, TrackerMock<int>, std::string),
	(true, TrackerMock<int>, int)
)
{
	STATIC_REQUIRE(expected == sg::concepts::tracker_for<T, Vertex>);
}
