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

	template <class T>
	struct default_delete_action
	{
		constexpr void operator ()(T&) const noexcept
		{
		}
	};

	template <std::movable T, std::invocable<T&> TDeleteAction = default_delete_action<T>>
		requires std::copyable<TDeleteAction>
	class unique_handle
	{
	public:
		using element_type = T;
		using delete_action_type = TDeleteAction;

		constexpr unique_handle() noexcept = default;

		constexpr ~unique_handle() noexcept
		{
			invoke_delete_action_if_necessary();
		}

		SL_UNIQUE_HANDLE_FULL_CONSTEXPR unique_handle
		(
			unique_handle&& other
		) noexcept(noexcept(std::is_nothrow_move_constructible_v<T>
							&& std::is_nothrow_copy_constructible_v<TDeleteAction>))
			: m_Value{ std::exchange(other.m_Value, std::nullopt) },
			m_DeleteAction{ other.m_DeleteAction }
		{
			other.m_Value.reset();
		}

		SL_UNIQUE_HANDLE_FULL_CONSTEXPR unique_handle& operator =
		(
			unique_handle&& other
		) noexcept(noexcept(std::is_nothrow_move_constructible_v<T>
							&& std::is_nothrow_move_assignable_v<T>
							&& std::is_nothrow_copy_constructible_v<TDeleteAction>
							&& std::is_nothrow_copy_assignable_v<TDeleteAction>))
		{
			if (this != &other)
			{
				invoke_delete_action_if_necessary();

				m_Value = std::exchange(other.m_Value, std::nullopt);
				m_DeleteAction = other.m_DeleteAction;
			}
			return *this;
		}

		constexpr void swap
		(
			unique_handle& other
		) noexcept(noexcept(std::is_nothrow_move_constructible_v<T>
							&& std::is_nothrow_swappable_v<T>
							&& std::is_nothrow_move_constructible_v<TDeleteAction>
							&& std::is_nothrow_swappable_v<TDeleteAction>))
		{
			using std::swap;

			swap(m_Value, other.m_Value);
			swap(m_DeleteAction, other.m_DeleteAction);
		}

		unique_handle(const unique_handle&) = delete;
		unique_handle& operator =(const unique_handle&) = delete;

		constexpr unique_handle(nullhandle_t) noexcept
			: m_Value{ std::nullopt }
		{
		}

		SL_UNIQUE_HANDLE_FULL_CONSTEXPR unique_handle& operator =(nullhandle_t) noexcept
		{
			invoke_delete_action_if_necessary();

			m_Value = std::nullopt;
			return *this;
		}

		constexpr unique_handle(const delete_action_type& deleteAction) noexcept
			: m_Value{ std::nullopt },
			m_DeleteAction{ deleteAction }
		{
		}

		template <concepts::constructs<T> T2>
			requires concepts::not_same_as<std::remove_cvref_t<T2>, unique_handle>
					&& concepts::not_same_as<std::remove_cvref_t<T2>, nullhandle_t>
		explicit (!std::convertible_to<T2&&, T>)
		constexpr unique_handle(T2&& value, const delete_action_type& deleteAction = delete_action_type{})
			: m_Value{ std::forward<T2>(value) },
			m_DeleteAction{ deleteAction }
		{
		}

		template <concepts::assignable_to<T&> T2>
			requires concepts::not_same_as<std::remove_cvref_t<T2>, unique_handle>
					&& concepts::not_same_as<std::remove_cvref_t<T2>, nullhandle_t>
					&& concepts::constructs<std::remove_cvref_t<T2>, T>
		constexpr unique_handle& operator =(T2&& value)
		{
			invoke_delete_action_if_necessary();

			m_Value = std::forward<T2>(value);
			return *this;
		}

		template <class... TArgs>
			requires std::constructible_from<T, TArgs...>
		constexpr explicit unique_handle(std::in_place_t, TArgs&&... args)
			: m_Value{ std::in_place, std::forward<TArgs>(args)... }
		{
		}

		template <class... TArgs>
			requires std::constructible_from<T, TArgs...>
		constexpr explicit unique_handle(std::in_place_t, const delete_action_type& deleteAction, TArgs&&... args)
			: m_Value{ std::in_place, std::forward<TArgs>(args)... },
			m_DeleteAction{ deleteAction }
		{
		}

		template <class... TArgs>
			requires std::constructible_from<T, TArgs...>
		SL_UNIQUE_HANDLE_FULL_CONSTEXPR T& emplace(TArgs&&... args)
		{
			return m_Value.emplace(std::forward<TArgs>(args)...);
		}

		SL_UNIQUE_HANDLE_FULL_CONSTEXPR void reset() noexcept
		{
			invoke_delete_action_if_necessary();

			m_Value.reset();
		}

		[[nodiscard]]
		constexpr const T& raw() const { return m_Value.value(); }

		[[nodiscard]]
		constexpr const T& operator *() const noexcept { return *m_Value; }

		[[nodiscard]]
		constexpr T* operator ->() noexcept { return &*m_Value; }

		[[nodiscard]]
		constexpr const T* operator ->() const noexcept { return &*m_Value; }

		[[nodiscard]]
		constexpr explicit operator bool() const noexcept { return m_Value.has_value(); }

		[[nodiscard]]
		constexpr bool is_valid() const noexcept { return m_Value.has_value(); }

		[[nodiscard]]
		constexpr delete_action_type& delete_action() noexcept { return m_DeleteAction; }

		[[nodiscard]]
		constexpr const delete_action_type& delete_action() const noexcept { return m_DeleteAction; }

	private:
		std::optional<T> m_Value{};

		[[no_unique_address]]
		TDeleteAction m_DeleteAction{};

		constexpr void invoke_delete_action_if_necessary() noexcept
		{
			if (m_Value)
				std::invoke(m_DeleteAction, *m_Value);
		}
	};
}

#endif
