//          Copyright Dominic Koepke 2019 - 2022.
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
		constexpr static std::nullopt_t null{ std::nullopt };
	};

	template <class T, class TAlternative>
	[[nodiscard]]
	constexpr T value_or_algorithm(const std::optional<T>& opt, TAlternative&& alternative)
	{
		return opt.value_or(std::forward<TAlternative>(alternative));
	}

	template <class T, class TAlternative>
	[[nodiscard]]
	constexpr T value_or_algorithm(std::optional<T>& opt, TAlternative&& alternative)
	{
		return opt.value_or(std::forward<TAlternative>(alternative));
	}

	template <class T, class TAlternative>
	[[nodiscard]]
	constexpr T value_or_algorithm(std::optional<T>&& opt, TAlternative&& alternative)
	{
		return std::move(opt).value_or(std::forward<TAlternative>(alternative));
	}
}

#endif
