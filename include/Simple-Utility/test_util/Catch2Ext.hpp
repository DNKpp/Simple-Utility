//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_TEST_UTIL_CATCH2_EXT_HPP
#define SIMPLE_UTILITY_TEST_UTIL_CATCH2_EXT_HPP

#pragma once

#include <catch2/matchers/catch_matchers_templated.hpp>

#include <ranges>

namespace catch_ext
{
	class RangesEmpty final
		: public Catch::Matchers::MatcherGenericBase
	{
	public:
		static constexpr bool match(const std::ranges::range auto& range)
		{
			return std::ranges::empty(range);
		}

		std::string describe() const override
		{
			return "Empty";
		}
	};
}

#endif
