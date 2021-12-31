//          Copyright Dominic Koepke 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_CONDITIONAL_ALGORITHMS_HPP
#define SL_UTILITY_CONDITIONAL_ALGORITHMS_HPP

#include <optional>
#include <memory>

#include "Simple-Utility/unique_handle.hpp"
#include "Simple-Utility/concepts/operators.hpp"

namespace sl::nullables
{
	template <class T>
	struct conditional_traits
	{
	};

	template <class T>
	using cond_value_t = typename conditional_traits<T>::value_type;

	template <class T>
	constexpr static auto cond_null_v{ conditional_traits<T>::null };

	template <class... TArgs>
	struct conditional_traits<unique_handle<TArgs...>>
	{
		using value_type = typename unique_handle<TArgs...>::value_type;
		constexpr static auto null{ nullhandle };
	};

	template <class... TArgs>
	struct conditional_traits<std::optional<TArgs...>>
	{
		using value_type = typename std::optional<TArgs...>::value_type;
		constexpr static auto null{ std::nullopt };
	};

	template <class... TArgs>
	struct conditional_traits<std::unique_ptr<TArgs...>>
	{
		using value_type = typename std::unique_ptr<TArgs...>::element_type;
		constexpr static std::nullptr_t null{ nullptr };
	};

	template <class... TArgs>
	struct conditional_traits<std::shared_ptr<TArgs...>>
	{
		using value_type = typename std::shared_ptr<TArgs...>::element_type;
		constexpr static std::nullptr_t null{ nullptr };
	};

	template <class T>
		requires std::is_pointer_v<T>
	struct conditional_traits<T>
	{
		using value_type = std::remove_pointer_t<T>;
		constexpr static std::nullptr_t null{ nullptr };
	};

	[[nodiscard]]
	constexpr decltype(auto) value_unchecked(concepts::dereferencable auto&& closure) noexcept
	{
		return *closure;
	}

	template <class TClosure>
	[[nodiscard]]
	constexpr bool has_value(const TClosure& closure) noexcept
		requires requires { static_cast<bool>(closure); }
	{
		return static_cast<bool>(closure);
	}

	template <class TClosure, class T>
	[[nodiscard]]
	constexpr auto value_or(TClosure&& closure, T&& alternative)
		requires requires { closure.value_or(std::forward<T>(alternative)); }
	{
		// let existing implementations handle the errors; don't want to over-constraint this
		return closure.value_or(std::forward<T>(alternative));
	}

	template <class TClosure, std::convertible_to<cond_value_t<TClosure>> T>
	[[nodiscard]]
	constexpr auto value_or(TClosure&& closure, T&& alternative)
		requires !requires { closure.value_or(std::forward<T>(alternative)); }
	{
		if (has_value(closure))
		{
			return value_unchecked(std::forward<TClosure>(closure));
		}
		return std::forward<T>(alternative);
	}
}

#endif
