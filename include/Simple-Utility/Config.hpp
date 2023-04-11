//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_CONFIG_HPP
#define SL_UTILITY_CONFIG_HPP

#pragma once

#if __has_cpp_attribute(no_unique_address)
	#define SL_UTILITY_NO_UNIQUE_ADDRESS [[no_unique_address]]
	#define SL_UTILITY_NO_UNIQUE_ADDRESS_ENABLED
#elif _MSC_VER >= 1929 && not defined(__clang__)
	#define SL_UTILITY_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
	#define SL_UTILITY_NO_UNIQUE_ADDRESS_ENABLED
#else
	#define SL_UTILITY_NO_UNIQUE_ADDRESS
#endif

namespace sl
{
	struct
	{
		constexpr int major = 2;
		constexpr int minor = 1;
		constexpr int patch = 2;
	} static constexpr version;
}

#endif
