
//          Copyright Dominic Koepke 2019 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_BITMASK_HPP
#define SL_BITMASK_HPP

#pragma once

#include <compare>
#include <concepts>
#include <type_traits>

#include "Simple-Utility/operators/bitwise.hpp"

namespace sl::detail
{
	template <class T>
	concept Flag = std::is_enum_v<T> || std::unsigned_integral<T>;

	template <class T, Flag... TArgs>
	constexpr T combine(TArgs... args) noexcept
	{
		return (static_cast<T>(args) | ...);
	}
}

namespace sl
{
	template <detail::Flag TFlag, std::unsigned_integral TStorage = unsigned int>
	requires requires (TFlag flag) { static_cast<TStorage>(flag); } && (sizeof(TFlag) <= sizeof(TStorage))
	class Bitmask :
		operators::BitwiseLogical
	{
	public:
		constexpr Bitmask() noexcept = default;
		/*ToDo: c++20
		 constexpr*/
		~Bitmask() noexcept = default;

		template <detail::Flag... TArgs>
		constexpr explicit Bitmask(TArgs... args) noexcept :
			m_Mask{ detail::combine<TStorage>(args...) }
		{}

		constexpr explicit Bitmask(TStorage mask) noexcept :
			m_Mask{ mask }
		{}

		constexpr Bitmask(const Bitmask&) noexcept = default;
		constexpr Bitmask& operator =(const Bitmask&) noexcept = default;
		constexpr Bitmask(Bitmask&&) noexcept = default;
		constexpr Bitmask& operator =(Bitmask&&) noexcept = default;

		constexpr bool operator ==(const Bitmask&) const noexcept = default;

		constexpr void clear() noexcept
		{
			m_Mask = TStorage(0);
		}

		[[nodiscard]] constexpr bool empty() noexcept
		{
			return m_Mask == 0;
		}

		[[nodiscard]] constexpr TStorage operator*() const noexcept
		{
			return m_Mask;
		}

		void setRawMask(TStorage raw) noexcept
		{
			m_Mask = raw;
		}

		template <detail::Flag... TArgs>
		[[nodiscard]] constexpr bool contains(TArgs... args) const noexcept
		{
			auto mask = detail::combine<TStorage>(args...);
			return (m_Mask & mask) == mask;
		}

		template <detail::Flag... TArgs>
		constexpr void set(TArgs... args) noexcept
		{
			m_Mask = detail::combine<TStorage>(args...);
		}

		template <detail::Flag... TArgs>
		constexpr void apply(TArgs... args) noexcept
		{
			m_Mask |= detail::combine<TStorage>(args...);
		}

		template <detail::Flag... TArgs>
		constexpr void remove(TArgs... args) noexcept
		{
			m_Mask &= ~detail::combine<TStorage>(args...);
		}

		constexpr Bitmask& operator &=(const Bitmask& mask) noexcept
		{
			m_Mask &= mask.m_Mask;
			return *this;
		}

		constexpr Bitmask& operator |=(const Bitmask& mask) noexcept
		{
			m_Mask |= mask.m_Mask;
			return *this;
		}

		constexpr Bitmask& operator ^=(const Bitmask& mask) noexcept
		{
			m_Mask ^= mask.m_Mask;
			return *this;
		}

		constexpr Bitmask& operator &=(TFlag flag) noexcept
		{
			m_Mask &= static_cast<TStorage>(flag);
			return *this;
		}

		constexpr Bitmask& operator |=(TFlag flag) noexcept
		{
			m_Mask |= static_cast<TStorage>(flag);
			return *this;
		}

		constexpr Bitmask& operator ^=(TFlag flag) noexcept
		{
			m_Mask ^= static_cast<TStorage>(flag);
			return *this;
		}

		[[nodiscard]] constexpr Bitmask operator ~() const noexcept
		{
			return Bitmask{ ~m_Mask };
		}

	private:
		TStorage m_Mask{};
	};
}

#endif
