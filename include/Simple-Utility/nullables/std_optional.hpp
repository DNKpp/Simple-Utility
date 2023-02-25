//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_NULLABLES_STD_OPTIONAL_HPP
#define SL_UTILITY_NULLABLES_STD_OPTIONAL_HPP

#pragma once

#include "Simple-Utility/nullables/base.hpp"

#include <optional>

namespace sl::nullables
{
	/**
	 * \brief Specialization for std::optional
	 * \ingroup GROUP_NULLABLES_TRAITS
	 * \tparam TArgs Arguments of std::optional
	 */
	template <class... TArgs>
	struct traits<std::optional<TArgs...>>
	{
		using value_type = typename std::optional<TArgs...>::value_type;
		static constexpr std::nullopt_t null{ std::nullopt };
	};
}

#endif
