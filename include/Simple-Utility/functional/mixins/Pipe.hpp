//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_FUNCTIONAL_MIXINS_PIPE_HPP
#define SIMPLE_UTILITY_FUNCTIONAL_MIXINS_PIPE_HPP

#pragma once

#include <concepts>
#include <tuple>

#include "Simple-Utility/Tuple.hpp"
#include "Simple-Utility/TypeList.hpp"
#include "Simple-Utility/concepts/stl_extensions.hpp"
#include "Simple-Utility/functional/Composition.hpp"

namespace sl::functional
{
	struct PipeStrategy
	{
		template <class Fn, class... Args>
			requires std::invocable<Fn, Args...>
		constexpr decltype(auto) operator ()(
			std::tuple<Fn>&& fn,
			Args&&... args
		) const noexcept(std::is_nothrow_invocable_v<Fn, Args...>)
		{
			return std::invoke(std::get<0>(std::move(fn)), std::forward<Args>(args)...);
		}

		template <concepts::tuple Fns, class... Args>
			requires std::invocable<type_list::front_t<Fns>, Args...>
					&& std::invocable<
						PipeStrategy,
						type_list::pop_front_t<Fns>,
						std::invoke_result_t<type_list::front_t<Fns>, Args...>>
		constexpr decltype(auto) operator ()(
			Fns fns,
			Args&&... args
		) const
			noexcept(std::is_nothrow_invocable_v<type_list::front_t<Fns>, Args...>
					&& std::is_nothrow_invocable_v<PipeStrategy,
													type_list::pop_front_t<Fns>,
													std::invoke_result_t<type_list::front_t<Fns>, Args...>>)
		{
			return std::apply(
				*this,
				std::apply(
					[&]<class FirstFn, class... OtherFns>(FirstFn&& first, OtherFns&&... others)
					{
						return std::make_tuple(
							std::forward_as_tuple(std::forward<OtherFns>(others)...),
							std::invoke(std::forward<FirstFn>(first), std::forward<Args>(args)...));
					},
					std::move(fns)));
		}
	};

	template <concepts::unqualified Derived>
	class PipeOperator
	{
	protected:
		[[nodiscard]]
		constexpr PipeOperator() noexcept
		{
			static_assert(std::is_base_of_v<PipeOperator, Derived>, "Derived doesn't inherit from PipeOperator.");
			static_assert(
				std::copy_constructible<Derived> || std::move_constructible<Derived>,
				"Derived is neither move- nor copy-constructible.");
		}

		[[nodiscard]]
		constexpr PipeOperator(const PipeOperator&) noexcept = default;
		constexpr PipeOperator& operator =(const PipeOperator&) noexcept = default;

		[[nodiscard]]
		constexpr PipeOperator(PipeOperator&&) noexcept = default;
		constexpr PipeOperator& operator =(PipeOperator&&) noexcept = default;

		constexpr ~PipeOperator() noexcept = default;

	public:
		template <class Other>
			requires function<std::remove_cvref_t<Other>>
		[[nodiscard]]
		friend constexpr auto operator |(
			const Derived& first,
			Other&& other
		) noexcept(noexcept(make_composition<PipeStrategy>(first, std::declval<Other>())))
		{
			return make_composition<PipeStrategy>(first, std::forward<Other>(other));
		}

		template <class Other>
			requires function<std::remove_cvref_t<Other>>
		[[nodiscard]]
		friend constexpr auto operator |(
			Derived&& first,
			Other&& other
		) noexcept(noexcept(make_composition<PipeStrategy>(std::move(first), std::declval<Other>())))
		{
			return make_composition<PipeStrategy>(std::move(first), std::forward<Other>(other));
		}
	};
}

#endif
