//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_FUNCTIONAL_MIXINS_DISJUNCTION_HPP
#define SIMPLE_UTILITY_FUNCTIONAL_MIXINS_DISJUNCTION_HPP

#pragma once

#include <concepts>
#include <tuple>

#include "Simple-Utility/concepts/stl_extensions.hpp"
#include "Simple-Utility/functional/Composition.hpp"

namespace sl::functional
{
	struct DisjunctionStrategy
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
					return (std::invoke(std::forward<Fns>(fns), args...) || ...);
				},
				std::move(fnPack));
		}
	};

	template <concepts::unqualified Derived>
	class DisjunctionOperator
	{
	protected:
		[[nodiscard]]
		constexpr DisjunctionOperator() noexcept
		{
			static_assert(std::is_base_of_v<DisjunctionOperator, Derived>, "Derived doesn't inherit from DisjunctionOperator.");
			static_assert(
				std::copy_constructible<Derived> || std::move_constructible<Derived>,
				"Derived is neither move- nor copy-constructible.");
		}

		[[nodiscard]]
		DisjunctionOperator(const DisjunctionOperator&) = default;
		DisjunctionOperator& operator =(const DisjunctionOperator&) = default;

		[[nodiscard]]
		DisjunctionOperator(DisjunctionOperator&&) = default;
		DisjunctionOperator& operator =(DisjunctionOperator&&) = default;

		~DisjunctionOperator() = default;

	public:
		template <class Other>
			requires function<std::remove_cvref_t<Other>>
		[[nodiscard]]
		friend constexpr auto operator ||(
			const Derived& first,
			Other&& other
		) noexcept(noexcept(make_composition<DisjunctionStrategy>(first, std::declval<Other>())))
		{
			return enclose_functional<closure_template<Derived>::type>(
				make_composition<DisjunctionStrategy>(first, std::forward<Other>(other)));
		}

		template <class Other>
			requires function<std::remove_cvref_t<Other>>
		[[nodiscard]]
		friend constexpr auto operator ||(
			Derived&& first,
			Other&& other
		) noexcept(noexcept(make_composition<DisjunctionStrategy>(std::move(first), std::declval<Other>())))
		{
			return enclose_functional<closure_template<Derived>::type>(
				make_composition<DisjunctionStrategy>(std::move(first), std::forward<Other>(other)));
		}
	};
}

#endif
