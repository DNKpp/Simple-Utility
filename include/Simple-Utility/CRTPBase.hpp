//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_CRTP_BASE_HPP
#define SIMPLE_UTILITY_CRTP_BASE_HPP

#pragma once

#include "Simple-Utility/concepts/stl_extensions.hpp"

namespace sl
{
	/**
	 * \brief Utility type, which can be used as a base class for the crtp pattern.
	 * \tparam Derived The derived type.
	 * \tparam Disambiguity In cases of multiple inheritance, this param can be used to disambiguity the base class.
	 */
	template <concepts::unqualified Derived, class Disambiguity = void>
	class CRTPBase
	{
	protected:
		/**
		 * \brief Up-casts the this pointer to a const lvalue reference to ``Derived``.
		 * \return A const lvalue reference.
		 */
		[[nodiscard]]
		constexpr const Derived& cast() const & noexcept
		{
			return static_cast<const Derived&>(*this);
		}

		/**
		 * \brief Up-casts the this pointer to a lvalue reference to ``Derived``.
		 * \return A lvalue reference.
		 */
		[[nodiscard]]
		constexpr Derived& cast() & noexcept
		{
			return static_cast<Derived&>(*this);
		}

		/**
		 * \brief Up-casts the this pointer to a const rvalue reference to ``Derived``.
		 * \return A const rvalue reference.
		 */
		[[nodiscard]]
		constexpr const Derived&& cast() const && noexcept
		{
			return static_cast<const Derived&&>(*this);
		}

		/**
		 * \brief Up-casts the this pointer to a rvalue reference to ``Derived``.
		 * \return A rvalue reference.
		 */
		[[nodiscard]]
		constexpr Derived&& cast() && noexcept
		{
			return static_cast<Derived&&>(*this);
		}

		/**
		 * \brief Default constructor, performing compile-time checks.
		 */
		constexpr CRTPBase() noexcept
		{
			static_assert(std::derived_from<Derived, CRTPBase>, "Derived doesn't inherit from CRTPBase.");
		}

		/**
		 * \brief Defaulted copy-constructor.
		 */
		constexpr CRTPBase(const CRTPBase&) noexcept = default;

		/**
		 * \brief Defaulted copy-assignment operator.
		 */
		constexpr CRTPBase& operator =(const CRTPBase&) noexcept = default;

		/**
		 * \brief Defaulted move-constructor.
		 */
		constexpr CRTPBase(CRTPBase&&) noexcept = default;

		/**
		 * \brief Defaulted move-assignment operator.
		 */
		constexpr CRTPBase& operator =(CRTPBase&&) noexcept = default;

		/**
		 * \brief Defaulted destructor.
		 */
		constexpr ~CRTPBase() noexcept = default;

	public:
		/**
		 * \brief Defaulted equality operator.
		 * \return Returns true, if both operators are equal.
		 */
		[[nodiscard]]
		constexpr bool operator ==(const CRTPBase&) const noexcept = default;
	};
}

#endif