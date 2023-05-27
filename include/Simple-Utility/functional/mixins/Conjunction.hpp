//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_FUNCTIONAL_MIXINS_CONJUNCTION_HPP
#define SIMPLE_UTILITY_FUNCTIONAL_MIXINS_CONJUNCTION_HPP

#pragma once

#include <concepts>
#include <tuple>

#include "Simple-Utility/concepts/stl_extensions.hpp"
#include "Simple-Utility/functional/Composition.hpp"

namespace sl::functional
{
	struct ConjunctionStrategy
	{
		template <class... PackedFns, class... Args>
			requires (... && std::predicate<PackedFns, Args...>)
		constexpr bool operator ()(
			std::tuple<PackedFns...>&& fnPack,
			Args&&... args
		) const
			noexcept((... && std::is_nothrow_invocable_v<PackedFns, Args...>))
		{
			return std::apply(
				[&]<class... Fns>(Fns&&... fns)
				{
					return (std::invoke(std::forward<Fns>(fns), args...) && ...);
				},
				std::move(fnPack));
		}
	};

	template <concepts::unqualified Derived>
	class ConjunctionOperator
	{
	protected:
		[[nodiscard]]
		constexpr ConjunctionOperator() noexcept
		{
			static_assert(std::is_base_of_v<ConjunctionOperator, Derived>, "Derived doesn't inherit from ConjunctionOperator.");
			static_assert(
				std::copy_constructible<Derived> || std::move_constructible<Derived>,
				"Derived is neither move- nor copy-constructible.");
		}

		[[nodiscard]]
		ConjunctionOperator(const ConjunctionOperator&) = default;
		ConjunctionOperator& operator =(const ConjunctionOperator&) = default;

		[[nodiscard]]
		ConjunctionOperator(ConjunctionOperator&&) = default;
		ConjunctionOperator& operator =(ConjunctionOperator&&) = default;

		~ConjunctionOperator() = default;

	public:
		template <class Other>
			requires function<std::remove_cvref_t<Other>>
		[[nodiscard]]
		friend constexpr auto operator &&(
			const Derived& first,
			Other&& other
		) noexcept(noexcept(make_composition<ConjunctionStrategy>(first, std::declval<Other>())))
		{
			return make_composition<ConjunctionStrategy>(first, std::forward<Other>(other));
		}

		template <class Other>
			requires function<std::remove_cvref_t<Other>>
		[[nodiscard]]
		friend constexpr auto operator &&(
			Derived&& first,
			Other&& other
		) noexcept(noexcept(make_composition<ConjunctionStrategy>(std::move(first), std::declval<Other>())))
		{
			return make_composition<ConjunctionStrategy>(std::move(first), std::forward<Other>(other));
		}
	};
}

#endif
