//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_TEST_UTIL_TROMPELOEIL_EXT_HPP
#define SIMPLE_UTILITY_TEST_UTIL_TROMPELOEIL_EXT_HPP

#pragma once

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_templated.hpp>

// strictly include this after the catch header
#include <catch2/trompeloeil.hpp>

#include <concepts>
#include <type_traits>

namespace trompeloeil_ext
{
	template <class T>
	struct matches_matcher_fn;

	template <class Matcher>
		requires std::derived_from<std::remove_cvref_t<Matcher>, Catch::Matchers::MatcherGenericBase>   // new style
				|| std::derived_from<std::remove_cvref_t<Matcher>, Catch::Matchers::MatcherUntypedBase> // old style
	struct matches_matcher_fn<Matcher>
	{
		constexpr auto operator ()(Matcher&& matcher) const
		{
			using matcher_type = std::remove_cvref_t<Matcher>;
			return trompeloeil::make_matcher<trompeloeil::wildcard>(
				[](const auto& value, const matcher_type& matcher) -> bool
				{
					return matcher.match(value);
				},
				[](std::ostream& os, const matcher_type& matcher)
				{
					os << "matching matcher: " << matcher.describe();
				},
				std::forward<Matcher>(matcher));
		}
	};

	inline constexpr auto matches = []<class Matcher>(Matcher&& matcher)
	{
		return matches_matcher_fn<std::remove_cvref_t<Matcher>>{}(std::forward<Matcher>(matcher));
	};
}

#endif
