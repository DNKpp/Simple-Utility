//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_OPERATORS_BIND_HPP
#define SL_UTILITY_FUNCTIONAL_OPERATORS_BIND_HPP

#pragma once

#include "Simple-Utility/functional/base.hpp"

namespace sl::functional::operators::detail
{
	struct bind_front_caller_fn
	{
		template <class TFunctionsTuple, class... TCallArgs>
		[[nodiscard]]
		constexpr decltype(auto) operator ()
		(
			TFunctionsTuple&& functionsTuple,
			TCallArgs&&... callArgs
		) const
		{
			static_assert(1 < std::tuple_size_v<std::remove_cvref_t<TFunctionsTuple>>, "Tuple must at least contain two functions.");

			const auto caller = [&]<class TFunction, std::invocable... TBoundArgs>
			(
				TFunction&& func,
				TBoundArgs&&... boundArgs
			) -> decltype(auto)
			{
				static_assert(std::invocable<TFunction, std::invoke_result_t<TBoundArgs>..., TCallArgs...>,
					"Functional is not invokable with bound and call arguments.");

				return std::invoke(
					std::forward<TFunction>(func),
					std::invoke(std::forward<TBoundArgs>(boundArgs))...,
					std::forward<TCallArgs>(callArgs)...
				);
			};

			return std::apply(caller, std::forward<TFunctionsTuple>(functionsTuple));
		}
	};

	struct bind_back_caller_fn
	{
		template <class TFunctionsTuple, class... TCallArgs>
		[[nodiscard]]
		constexpr decltype(auto) operator ()
		(
			TFunctionsTuple&& functionsTuple,
			TCallArgs&&... callArgs
		) const
		{
			static_assert(1 < std::tuple_size_v<std::remove_cvref_t<TFunctionsTuple>>, "Tuple must at least contain two functions.");

			const auto caller = [&]<class TFunction, std::invocable... TBoundArgs>
			(
				TFunction&& func,
				TBoundArgs&&... boundArgs
			) -> decltype(auto)
			{
				static_assert(std::invocable<TFunction, TCallArgs..., std::invoke_result_t<TBoundArgs>...>,
					"Functional is not invokable with bound and call arguments.");

				return std::invoke(
					std::forward<TFunction>(func),
					std::forward<TCallArgs>(callArgs)...,
					std::invoke(std::forward<TBoundArgs>(boundArgs))...
				);
			};

			return std::apply(caller, std::forward<TFunctionsTuple>(functionsTuple));
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
		inline static constexpr composition_strategy_t composition_strategy{ composition_strategy_t::prefer_join };
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
	 * \brief Specialized traits for \ref back_front.
	 * \relatesalso bind_back
	 */
	template <>
	struct tag_traits<bind_back>
	{
		using operation_t = detail::bind_back_caller_fn;
		inline static constexpr composition_strategy_t composition_strategy{ composition_strategy_t::prefer_join };
	};

	/**
	 * \brief Curries the back parameter of the functional object.
	 * \relatesalso bind_front
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
