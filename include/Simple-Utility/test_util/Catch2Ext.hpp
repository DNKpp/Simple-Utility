//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_TEST_UTIL_CATCH2_EXT_HPP
#define SIMPLE_UTILITY_TEST_UTIL_CATCH2_EXT_HPP

#pragma once

#include "Simple-Utility/Config.hpp"
#include "Simple-Utility/concepts/stl_extensions.hpp"

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

#ifdef SL_UTLITY_HAS_STD_FORMAT

#include <format>

template <class T>
	requires sl::concepts::formattable<T, char>
			&& (!Catch::Detail::IsStreamInsertable<T>::value)
struct Catch::StringMaker<T>
{
	static std::string convert(const T& value)
	{
		return std::format("{}", value);
	}
};

#endif

#endif
