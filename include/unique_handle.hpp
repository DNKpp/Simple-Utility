//          Copyright Dominic Koepke 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UNIQUE_HANDLE_HPP
#define SL_UNIQUE_HANDLE_HPP

#pragma once

#include <concepts>
#include <optional>

#include "Simple-Utility/concepts/stl_counterparts.hpp"

// some of the std::optional interface hasn't declared constexpr before
#if __cpp_lib_optional >= 202106
#define SL_UNIQUE_HANDLE_FULL_CONSTEXPR constexpr
#else
#define SL_UNIQUE_HANDLE_FULL_CONSTEXPR
#endif

namespace sl
{
	// ReSharper disable once IdentifierTypo
	struct nullhandle_t
	{
	};

	// ReSharper disable once IdentifierTypo
	constexpr nullhandle_t nullhandle{};

	using bad_handle_access = std::bad_optional_access;

	template <std::movable T>
	class unique_handle
	{
	public:
		constexpr unique_handle() noexcept = default;
		constexpr ~unique_handle() noexcept = default;
		constexpr unique_handle(unique_handle&&) noexcept = default;
		constexpr unique_handle& operator =(unique_handle&&) noexcept = default;

		unique_handle(const unique_handle&) = delete;
		unique_handle& operator =(const unique_handle&) = delete;

		constexpr unique_handle(nullhandle_t) noexcept
			: m_Value{ std::nullopt }
		{
		}

		SL_UNIQUE_HANDLE_FULL_CONSTEXPR unique_handle& operator =(nullhandle_t) noexcept
		{
			m_Value = std::nullopt;
			return *this;
		}

		template <concepts::constructs<T> T2>
			requires concepts::not_same_as<std::remove_cvref_t<T2>, unique_handle>
					&& concepts::not_same_as<std::remove_cvref_t<T2>, nullhandle_t>
		explicit (!std::convertible_to<T2&&, T>)
		constexpr unique_handle(T2&& value)
			: m_Value{ std::forward<T2>(value) }
		{
		}

		template <concepts::assignable_to<T&> T2>
			requires concepts::not_same_as<std::remove_cvref_t<T2>, unique_handle>
					&& concepts::not_same_as<std::remove_cvref_t<T2>, nullhandle_t>
					&& concepts::constructs<std::remove_cvref_t<T2>, nullhandle_t>
		constexpr unique_handle& operator =(T2&& value)
		{
			m_Value = std::forward<T2>(value);
			return *this;
		}

		[[nodiscard]]
		constexpr const T& raw() const
		{
			return m_Value.value();
		}

		[[nodiscard]]
		constexpr const T& operator *() const
		{
			return *m_Value;
		}

		[[nodiscard]]
		constexpr T* operator ->()
		{
			return &*m_Value;
		}

		[[nodiscard]]
		constexpr const T* operator ->() const
		{
			return &*m_Value;
		}

		[[nodiscard]]
		constexpr explicit operator bool() const noexcept { return m_Value.has_value(); }

		[[nodiscard]]
		constexpr bool is_valid() const noexcept { return m_Value.has_value(); }

		[[nodiscard]]
		SL_UNIQUE_HANDLE_FULL_CONSTEXPR void reset() noexcept
		{
			m_Value.reset();
		}

	private:
		std::optional<T> m_Value{};
	};
}

#endif
