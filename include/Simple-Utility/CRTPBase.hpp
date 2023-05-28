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
		constexpr const Derived& derived() const noexcept
		{
			return static_cast<const Derived&>(*this);
		}

		/**
		 * \brief Up-casts the this pointer to a lvalue reference to ``Derived``.
		 * \return A lvalue reference.
		 */
		[[nodiscard]]
		constexpr Derived& derived() noexcept
		{
			return static_cast<Derived&>(*this);
		}

		/**
		 * \brief Default constructor, performing compile-time checks.
		 */
		constexpr CRTPBase() noexcept
		{
			static_assert(std::is_base_of_v<CRTPBase, Derived>, "Derived doesn't inherit from CRTPBase.");
		}

		/**
		 * \brief Defaulted copy-constructor.
		 */
		CRTPBase(const CRTPBase&) = default;

		/**
		 * \brief Defaulted copy-assignment operator.
		 */
		CRTPBase& operator =(const CRTPBase&) = default;

		/**
		 * \brief Defaulted move-constructor.
		 */
		CRTPBase(CRTPBase&&) = default;

		/**
		 * \brief Defaulted move-assignment operator.
		 */
		CRTPBase& operator =(CRTPBase&&) = default;

		/**
		 * \brief Defaulted destructor.
		 */
		~CRTPBase() = default;

		/**
		 * \brief Defaulted equality operator.
		 * \return Returns true, if both operators are equal.
		 */
		[[nodiscard]]
		constexpr bool operator ==(const CRTPBase&) const noexcept = default;
	};
}

#endif
