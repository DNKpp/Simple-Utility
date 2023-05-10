//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_CONFIG_HPP
#define SL_UTILITY_CONFIG_HPP

#pragma once

#define SL_UTILITY_STR(arg) #arg
#define SL_UTILITY_XSTR(arg) SL_UTILITY_STR(arg)

#if __has_cpp_attribute(no_unique_address)
	#define SL_UTILITY_NO_UNIQUE_ADDRESS [[no_unique_address]]
	#define SL_UTILITY_NO_UNIQUE_ADDRESS_ENABLED
#elif _MSC_VER >= 1929 && not defined(__clang__)
	#define SL_UTILITY_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
	#define SL_UTILITY_NO_UNIQUE_ADDRESS_ENABLED
#else
	#define SL_UTILITY_NO_UNIQUE_ADDRESS
#endif

#define SL_UTILITY_VERSION_MAJOR 2  // NOLINT(modernize-macro-to-enum)
#define SL_UTILITY_VERSION_MINOR 2  // NOLINT(modernize-macro-to-enum)
#define SL_UTILITY_VERSION_PATCH 1  // NOLINT(modernize-macro-to-enum)

#define SL_UTILITY_VERSION \
    SL_UTILITY_XSTR(SL_UTILITY_VERSION_MAJOR) \
    "." SL_UTILITY_XSTR(SL_UTILITY_VERSION_MINOR) \
    "." SL_UTILITY_XSTR(SL_UTILITY_VERSION_PATCH)

#endif
