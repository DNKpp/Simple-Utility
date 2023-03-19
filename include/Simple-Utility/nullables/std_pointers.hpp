//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_NULLABLES_STD_POINTERS_HPP
#define SL_UTILITY_NULLABLES_STD_POINTERS_HPP

#pragma once

#include "Simple-Utility/nullables/base.hpp"

#include <memory>

/**
 * \brief Specialization for std::unique_ptr
 * \ingroup GROUP_NULLABLES_TRAITS
 * \tparam TArgs Arguments of std::unique_ptr
 */
template <class... TArgs>
struct sl::nullables::traits<std::unique_ptr<TArgs...>>
{
	using value_type = typename std::unique_ptr<TArgs...>::element_type;
	static constexpr std::nullptr_t null{ nullptr };
};

/**
 * \brief Specialization for std::shared_ptr
 * \ingroup GROUP_NULLABLES_TRAITS
 * \tparam TArgs Arguments of std::shared_ptr
 */
template <class... TArgs>
struct sl::nullables::traits<std::shared_ptr<TArgs...>>
{
	using value_type = typename std::shared_ptr<TArgs...>::element_type;
	static constexpr std::nullptr_t null{ nullptr };
};

#endif
