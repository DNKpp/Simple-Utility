//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/test_util/TrompeloeilExt.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

TEST_CASE("Catch2 matchers can be used as argument for the trompeloeil_ext::matches matcher", "[test_util][test_util::trompeloeil]")
{
	const std::vector data{42, 43, 47};

	// new style
	REQUIRE(!trompeloeil_ext::matches(Catch::Matchers::RangeEquals(std::array{42, 47})).matches(data));
	REQUIRE(trompeloeil_ext::matches(Catch::Matchers::RangeEquals(std::array{42, 43, 47})).matches(data));

	// old style
	REQUIRE(trompeloeil_ext::matches(Catch::Matchers::VectorContains(43)).matches(data));
	REQUIRE(!trompeloeil_ext::matches(Catch::Matchers::VectorContains(1337)).matches(data));
}

TEST_CASE("trompeloeil_ext::matches can be used to print something to an ostream.", "[test_util][test_util::trompeloeil]")
{
	std::ostringstream ss{};
	ss << trompeloeil_ext::matches(Catch::Matchers::RangeEquals(std::vector{42, 43, 47}));

	REQUIRE(!std::ranges::empty(ss.str()));
}

#ifdef SL_UTILITY_HAS_STD_FORMAT

namespace
{
	struct TestType
	{
		int value{};
	};
}

template <class Char>
struct std::formatter<TestType, Char>
	: public std::formatter<int, Char>
{
	template<class FormatContext>
    auto format(TestType t, FormatContext& fc) const
    {
		return std::format_to(fc.out(), "TestType: {}", t.value);
    }
};

TEST_CASE("trompeloeil::printer is extended by std::format compatible types.", "[test_util][test_util::trompeloeil]")
{
	STATIC_CHECK(sl::concepts::formattable<TestType, char>);

	std::ostringstream os{};
	trompeloeil::print(os, TestType{42});

	REQUIRE("TestType: 42" == os.str());
}

#endif
