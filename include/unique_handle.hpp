//          Copyright Dominic Koepke 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UNIQUE_HANDLE_HPP
#define SL_UNIQUE_HANDLE_HPP

#pragma once

#include <concepts>
#include <optional>

namespace sl
{
	// ReSharper disable once IdentifierTypo
	struct nullhandle_t
	{
	};

	// ReSharper disable once IdentifierTypo
	constexpr nullhandle_t nullhandle{};

	template <std::movable T>
	class unique_handle
	{
	public:
		constexpr unique_handle() noexcept = default;
		constexpr ~unique_handle() noexcept = default;

		constexpr unique_handle(nullhandle_t) noexcept
			: m_Value{ std::nullopt }
		{
		}

		unique_handle(const unique_handle&) = delete;
		unique_handle& operator =(const unique_handle&) = delete;

		[[nodiscard]]
		constexpr explicit operator bool() const noexcept { return m_Value.has_value(); }

		[[nodiscard]]
		constexpr bool has_value() const noexcept { return m_Value.has_value(); }

	private:
		std::optional<T> m_Value{};
	};
}

#endif
