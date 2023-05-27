//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_FUNCTIONAL_MIXINS_NEGATION_HPP
#define SIMPLE_UTILITY_FUNCTIONAL_MIXINS_NEGATION_HPP

#pragma once

#include <concepts>
#include <functional>

#include "Simple-Utility/concepts/stl_extensions.hpp"
#include "Simple-Utility/functional/BasicClosure.hpp"

namespace sl::functional
{
	template <concepts::unqualified Derived>
	class NegationOperator
	{
	private:
		[[nodiscard]]
		constexpr const Derived& derived() const noexcept
		{
			return static_cast<const Derived&>(*this);
		}

		[[nodiscard]]
		constexpr Derived& derived() noexcept
		{
			return static_cast<Derived&>(*this);
		}

	protected:
		[[nodiscard]]
		constexpr NegationOperator() noexcept
		{
			static_assert(std::is_base_of_v<NegationOperator, Derived>, "Derived doesn't inherit from NegationOperator.");
			static_assert(
				std::copy_constructible<Derived> || std::move_constructible<Derived>,
				"Derived is neither move- nor copy-constructible.");
		}

		[[nodiscard]]
		NegationOperator(const NegationOperator&) = default;
		NegationOperator& operator =(const NegationOperator&) = default;

		[[nodiscard]]
		NegationOperator(NegationOperator&&) = default;
		NegationOperator& operator =(NegationOperator&&) = default;

		~NegationOperator() = default;

	public:
		[[nodiscard]]
		constexpr auto operator !() const & noexcept(std::is_nothrow_copy_constructible_v<Derived>)
		{
			return enclose_functional<closure_template<Derived>::template type>(
				std::not_fn(derived()));
		}

		[[nodiscard]]
		constexpr auto operator !() && noexcept(std::is_nothrow_move_constructible_v<Derived>)
		{
			return enclose_functional<closure_template<Derived>::template type>(
				std::not_fn(std::move(derived())));
		}
	};
}

#endif
