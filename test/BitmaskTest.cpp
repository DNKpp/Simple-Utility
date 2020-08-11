
//          Copyright Dominic Koepke 2019 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <limits>

#include "catch.hpp"
#include "Simple-Utility/Bitmask.hpp"

enum class Test
{
	one		= 0b0001,
	two		= 0b0010,
	four	= 0b0100,
	eight	= 0b1000
};

using TestFlags = sl::Bitmask<Test>;

TEST_CASE("basic tests", "[Bitmask]")
{
	const unsigned int initMask = 15;
	
	SECTION("construction test")
	{
		TestFlags flags;
		REQUIRE(*flags == 0);

		flags.setRawMask(initMask);
		REQUIRE(*flags == initMask);
		REQUIRE(flags == TestFlags{ initMask });
		REQUIRE_FALSE(flags == TestFlags{ initMask - 1 });
		REQUIRE(*TestFlags{ Test::one, Test::four } == 5);
	}

	SECTION("function tests")
	{
		TestFlags flags{ initMask };
		TestFlags flags2{};
		REQUIRE(*flags == initMask);
		REQUIRE(*flags2 == 0);

		SECTION("empty")
		{
			REQUIRE_FALSE(flags.empty());
			REQUIRE_FALSE(*flags == 0);
			REQUIRE(flags2.empty());
			REQUIRE(*flags2 == 0);

			flags2 = TestFlags{ 1 };
			REQUIRE_FALSE(flags2.empty());
			REQUIRE_FALSE(*flags2 == 0);
		}

		SECTION("clear")
		{
			flags.clear();
			REQUIRE(flags.empty());

			flags2.clear();
			REQUIRE(flags2.empty());
		}

		SECTION("contains")
		{
			REQUIRE(flags.contains(Test::one));
			REQUIRE(flags.contains(Test::four, Test::two, Test::four));
			REQUIRE_FALSE(flags.empty());

			REQUIRE_FALSE(flags2.contains(Test::one));
			REQUIRE_FALSE(flags2.contains(Test::four, Test::two, Test::four));
			REQUIRE(flags2.empty());

			flags2 = TestFlags{ 1 };
			REQUIRE_FALSE(flags2.contains(Test::one, Test::four));
			REQUIRE_FALSE(flags2.empty());
		}

		SECTION("apply")
		{
			flags.apply(Test::one);
			REQUIRE(flags.contains(Test::one, Test::two, Test::four, Test::eight));
			flags.apply(Test::two, Test::four, Test::eight);
			REQUIRE(flags.contains(Test::one, Test::two, Test::four, Test::eight));
			
			flags2.apply(Test::one);
			REQUIRE(flags2.contains(Test::one));
			flags2.apply(Test::two, Test::four, Test::eight);
			REQUIRE(flags2.contains(Test::one, Test::two, Test::four, Test::eight));
		}

		SECTION("remove")
		{
			flags.remove(Test::one);
			REQUIRE(flags.contains(Test::two, Test::four, Test::eight));
			flags.remove(Test::two, Test::four, Test::eight);
			REQUIRE(flags.empty());
			
			flags2.remove(Test::one);
			REQUIRE(flags2.empty());
			flags2.remove(Test::two, Test::four, Test::eight);
			REQUIRE(flags2.empty());
		}
	}

	SECTION("operator tests")
	{
		TestFlags flags{ initMask };
		TestFlags flags2{};

		SECTION("operator AND")
		{
			REQUIRE((flags & flags2) == flags2);
			REQUIRE((flags & Test::four) == TestFlags{ Test::four });
		}

		SECTION("operator OR")
		{
			REQUIRE((flags | flags2) == flags);
			REQUIRE((flags2 | Test::four) == TestFlags{ Test::four });
		}

		SECTION("operator XOR")
		{
			flags2.apply(Test::eight);
			
			REQUIRE((flags ^ flags2) == TestFlags{ Test::one, Test::two, Test::four });
			REQUIRE((flags2 ^ Test::four) == TestFlags{ Test::four, Test::eight });
		}

		SECTION("operator negation")
		{
			auto tmp = ~flags2;
			REQUIRE(std::numeric_limits<unsigned int>::max() == *tmp);
			
			tmp = ~flags;
			REQUIRE(std::numeric_limits<unsigned int>::max() - *flags == *tmp);
		}
	}
}
