//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_BENCHMARKS_DEFINES_HPP
#define SIMPLE_UTILITY_BENCHMARKS_DEFINES_HPP

#pragma once

#include <filesystem>

[[nodiscard]]
inline std::filesystem::path artifacts_root_path()
{
	return std::filesystem::current_path() / "artifacts";
}

#endif
