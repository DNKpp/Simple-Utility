//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_OPERATORS_BIND_HPP
#define SL_UTILITY_FUNCTIONAL_OPERATORS_BIND_HPP

#pragma once

#include "Simple-Utility/tuple_utility.hpp"
#include "Simple-Utility/concepts/utility.hpp"
#include "Simple-Utility/functional/base.hpp"

namespace sl::functional::operators::detail
{
	struct bind_front_caller_fn
	{
		template <class TCallArgsTuple, class TFunction, class... TBoundFunctions>
			requires concepts::apply_invocable<
				TFunction, tuple_cat_result_t<std::tuple<std::invoke_result_t<TBoundFunctions>...>, TCallArgsTuple>
			>
		[[nodiscard]]
		constexpr auto operator ()
		(
			TCallArgsTuple&& callArgsTuple,
			TFunction&& func,
			TBoundFunctions&&... boundFunctions
		) const
		noexcept(concepts::nothrow_apply_invocable<
			TFunction, tuple_cat_result_t<std::tuple<std::invoke_result_t<TBoundFunctions>...>, TCallArgsTuple>
		>)
		{
			return std::apply(
				[&]<class... TCallArgs>(TCallArgs&&... callArgs) -> decltype(auto)
				{
					static_assert(std::invocable<TFunction, std::invoke_result_t<TBoundFunctions>..., TCallArgs...>,
						"Functional is not invokable with bound and call arguments.");

					return std::invoke(
						std::forward<TFunction>(func),
						std::invoke(std::forward<TBoundFunctions>(boundFunctions))...,
						std::forward<TCallArgs>(callArgs)...
					);
				},
				std::forward<TCallArgsTuple>(callArgsTuple)
			);
		}
	};

	struct bind_back_caller_fn
	{
		template <class TCallArgsTuple, class TFunction, class... TBoundFunctions>
			requires concepts::apply_invocable<
				TFunction, tuple_cat_result_t<TCallArgsTuple, std::tuple<std::invoke_result_t<TBoundFunctions>...>>
			>
		[[nodiscard]]
		constexpr auto operator ()
		(
			TCallArgsTuple&& callArgsTuple,
			TFunction&& func,
			TBoundFunctions&&... boundFunctions
		) const
		noexcept(concepts::nothrow_apply_invocable<
			TFunction, tuple_cat_result_t<TCallArgsTuple, std::tuple<std::invoke_result_t<TBoundFunctions>...>>
		>)
		{
			return std::apply(
				[&]<class... TCallArgs>(TCallArgs&&... callArgs) -> decltype(auto)
				{
					static_assert(std::invocable<TFunction, TCallArgs..., std::invoke_result_t<TBoundFunctions>...>,
						"Functional is not invokable with bound and call arguments.");

					return std::invoke(
						std::forward<TFunction>(func),
						std::forward<TCallArgs>(callArgs)...,
						std::invoke(std::forward<TBoundFunctions>(boundFunctions))...
					);
				},
				std::forward<TCallArgsTuple>(callArgsTuple)
			);
		}
	};
}

namespace sl::functional::operators
{
	/**
	 * \addtogroup GROUP_FUNCTIONAL_OPERATORS
	 * \ingroup GROUP_FUNCTIONAL
	 * @{
	 */

	/**
	 * \brief Tag type which enables front currying on functionals via operator <<.
	 * \relatesalso sl::functional::enable_operation
	 */
	struct bind_front
	{};

	/**
	 * \brief Specialized traits for \ref bind_front.
	 * \relatesalso bind_front
	 */
	template <>
	struct tag_traits<bind_front>
	{
		using operation_t = detail::bind_front_caller_fn;
		static constexpr composition_strategy_t composition_strategy{ composition_strategy_t::join };
	};

	/**
	 * \brief Curries the front parameter of the functional object.
	 * \relatesalso bind_front
	 * \tparam TFunc The functional type.
	 * \tparam TValue The type of the curried value.
	 * \param func The functional object.
	 * \param value The value to be curried.
	 * \return The curried functional, as a new functional object.
	 */
	template <derived_from_unified_base<functional::detail::enable_operators_base_tag> TFunc, class TValue>
		requires std::derived_from<std::remove_cvref_t<TFunc>, bind_front>
	[[nodiscard]]
	constexpr auto operator <<
	(
		TFunc&& func,
		TValue&& value
	)
	noexcept(is_nothrow_composable_v<bind_front, TFunc, value_fn<std::remove_cvref_t<TValue>>&&>)
	{
		return functional::detail::make_composition_from_tag<bind_front>(
			std::forward<TFunc>(func),
			value_fn{ std::forward<TValue>(value) }
		);
	}

	/**
	 * \brief Tag type which enables back currying on functionals via operator >>.
	 * \relatesalso sl::functional::enable_operation
	 */
	struct bind_back
	{};

	/**
	 * \brief Specialized traits for \ref bind_back.
	 * \relatesalso bind_back
	 */
	template <>
	struct tag_traits<bind_back>
	{
		using operation_t = detail::bind_back_caller_fn;
		static constexpr composition_strategy_t composition_strategy{ composition_strategy_t::join };
	};

	/**
	 * \brief Curries the back parameter of the functional object.
	 * \relatesalso bind_back
	 * \tparam TFunc The functional type.
	 * \tparam TValue The type of the curried value.
	 * \param func The functional object.
	 * \param value The value to be curried.
	 * \return The curried functional, as a new functional object.
	 */
	template <derived_from_unified_base<functional::detail::enable_operators_base_tag> TFunc, class TValue>
		requires std::derived_from<std::remove_cvref_t<TFunc>, bind_back>
	[[nodiscard]]
	constexpr auto operator >>
	(
		TFunc&& func,
		TValue&& value
	)
	noexcept(is_nothrow_composable_v<bind_back, TFunc, value_fn<std::remove_cvref_t<TValue>>&&>)
	{
		return functional::detail::make_composition_from_tag<bind_back>(
			std::forward<TFunc>(func),
			value_fn{ std::forward<TValue>(value) }
		);
	}

	/** @} */
}

#endif
