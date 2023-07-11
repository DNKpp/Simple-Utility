//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_FUNCTIONAL_MIXINS_EQUALITY_HPP
#define SIMPLE_UTILITY_FUNCTIONAL_MIXINS_EQUALITY_HPP

#pragma once

#include <concepts>
#include <functional>
#include <tuple>

#include "Simple-Utility/concepts/stl_extensions.hpp"
#include "Simple-Utility/functional/Composition.hpp"

namespace sl::functional
{
	struct EqualityStrategy
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
				[&]<class FirstFn, class... OtherFns>(FirstFn&& firstFn, OtherFns&&... otherFns)
				{
					const auto& firstResult = std::invoke(std::forward<FirstFn>(firstFn), args...);

					// This workaround is necessary, due to some strange compile errors in the msvc v142 toolset
					const auto equalResults = [&]<class Fn>(Fn&& fn)
					{
						return firstResult == std::invoke(std::forward<Fn>(fn), args...);
					};
					return (equalResults(std::forward<OtherFns>(otherFns)) && ...);
				},
				std::move(fnPack));
		}
	};

	template <concepts::unqualified Derived>
	class EqualityOperator
	{
	private:
		using Strategy = EqualityStrategy;

	protected:
		[[nodiscard]]
		constexpr EqualityOperator() noexcept
		{
			static_assert(std::is_base_of_v<EqualityOperator, Derived>, "Derived doesn't inherit from EqualityOperator.");
			static_assert(
				std::copy_constructible<Derived> || std::move_constructible<Derived>,
				"Derived is neither move- nor copy-constructible.");
		}

		[[nodiscard]]
		EqualityOperator(const EqualityOperator&) = default;
		EqualityOperator& operator =(const EqualityOperator&) = default;

		[[nodiscard]]
		EqualityOperator(EqualityOperator&&) = default;
		EqualityOperator& operator =(EqualityOperator&&) = default;

		~EqualityOperator() = default;

	public:
		template <class Other>
			requires function<std::remove_cvref_t<Other>>
		[[nodiscard]]
		friend constexpr auto operator ==(
			const Derived& first,
			Other&& other
		) noexcept(noexcept(make_composition<Strategy>(first, std::declval<Other>())))
		{
			return envelop<closure_template<Derived>::template type>(
				make_composition<Strategy>(first, std::forward<Other>(other)));
		}

		template <class Other>
			requires function<std::remove_cvref_t<Other>>
		[[nodiscard]]
		friend constexpr auto operator ==(
			Derived&& first,
			Other&& other
		) noexcept(noexcept(make_composition<Strategy>(std::move(first), std::declval<Other>())))
		{
			return envelop<closure_template<Derived>::template type>(
				make_composition<Strategy>(std::move(first), std::forward<Other>(other)));
		}
	};
}

#endif
