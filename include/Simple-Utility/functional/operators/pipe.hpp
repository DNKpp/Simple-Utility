//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_OPERATORS_PIPE_HPP
#define SL_UTILITY_FUNCTIONAL_OPERATORS_PIPE_HPP

#pragma once

#include "Simple-Utility/functional/base.hpp"
#include "Simple-Utility/tuple/general.hpp"

namespace sl::functional::operators::detail
{
	template <class TInput, std::invocable<TInput> TFunc, class... TOthers>
	struct is_recursive_supply_invocable
		: std::bool_constant<
			std::is_invocable_v<TFunc, TInput>
			&& is_recursive_supply_invocable<std::invoke_result_t<TFunc, TInput>, TOthers...>::value
		>
	{
	};

	template <class TInput, std::invocable<TInput> TFunc>
	struct is_recursive_supply_invocable<TInput, TFunc>
		: std::bool_constant<std::is_invocable_v<TFunc, TInput>>
	{
	};

	template <class TInput, class... TFunctions>
	concept recursive_supply_invocable = is_recursive_supply_invocable<TInput, TFunctions...>::value;

	template <class TInput, std::invocable<TInput> TFunc, class... TOthers>
	struct is_nothrow_recursive_supply_invocable
		: std::bool_constant<
			std::is_nothrow_invocable_v<TFunc, TInput>
			&& is_nothrow_recursive_supply_invocable<std::invoke_result_t<TFunc, TInput>, TOthers...>::value
		>
	{
	};

	template <class TInput, std::invocable<TInput> TFunc>
	struct is_nothrow_recursive_supply_invocable<TInput, TFunc>
		: std::bool_constant<std::is_nothrow_invocable_v<TFunc, TInput>>
	{
	};

	template <class TInput, class... TFunctions>
	concept nothrow_recursive_supply_invocable = is_nothrow_recursive_supply_invocable<TInput, TFunctions...>::value;

	template <class TInput, std::invocable<TInput> TFunc, class... TOthers>
		requires recursive_supply_invocable<TInput, TFunc, TOthers...>
	[[nodiscard]]
	constexpr decltype(auto) recursive_supply_invoke(
		TInput&& input,
		TFunc&& func,
		TOthers&&... otherFunctions
	) noexcept(nothrow_recursive_supply_invocable<TInput, TFunc, TOthers...>)
	{
		if constexpr (0 < sizeof...(TOthers))
		{
			return recursive_supply_invoke(
				std::invoke(std::forward<TFunc>(func), std::forward<TInput>(input)),
				std::forward<TOthers>(otherFunctions)...
			);
		}
		else
		{
			return std::invoke(std::forward<TFunc>(func), std::forward<TInput>(input));
		}
	}

	struct nested_invoke_caller_fn
	{
		template <class TCallArgsTuple, concepts::applicable<TCallArgsTuple> TInitFunction, class... TFunctions>
			requires recursive_supply_invocable<tuple::applicable_result_t<TInitFunction, TCallArgsTuple>, TFunctions...>
		[[nodiscard]]
		constexpr auto operator ()(
			TCallArgsTuple&& callArgsTuple,
			TInitFunction&& initFunction,
			TFunctions&&... functions
		) const noexcept(concepts::nothrow_applicable<TInitFunction, TCallArgsTuple>
						&& nothrow_recursive_supply_invocable<
							tuple::applicable_result_t<TInitFunction, TCallArgsTuple>, TFunctions...>)
		{
			return recursive_supply_invoke(
				std::apply(std::forward<TInitFunction>(initFunction), std::forward<TCallArgsTuple>(callArgsTuple)),
				std::forward<TFunctions>(functions)...
			);
		}
	};
}

namespace sl::functional::operators
{
	/**
	 * \defgroup GROUP_FUNCTIONAL_OPERATORS operators
	 * \ingroup GROUP_FUNCTIONAL
	 * @{
	 */

	/**
	 * \brief Tag type which enables pipe composing of two functional objects via operator |.
	 * \relatesalso sl::functional::enable_operation
	 */
	struct pipe
	{
	};

	/**
	 * \brief Specialized traits for \ref pipe.
	 * \relatesalso pipe
	 */
	template <>
	struct tag_traits<pipe>
	{
		using operation_t = detail::nested_invoke_caller_fn;
		static constexpr composition_strategy_t composition_strategy{composition_strategy_t::join};
	};

	/**
	 * \brief Composes both functional objects as nested function (result of left-hand-side as argument of right-hand-side).
	 * \relatesalso pipe
	 * \tparam TLhs The left-hand-side functional type.
	 * \tparam TRhs The right-hand-side functional type.
	 * \param lhs The left-hand-side functional object.
	 * \param rhs The right-hand-side functional object.
	 * \return The nested composition of both functional objects as new functional object.
	 */
	template <class TLhs, class TRhs>
		requires (std::derived_from<std::remove_cvref_t<TLhs>, pipe>
				&& derived_from_unified_base<TLhs, functional::detail::enable_operators_base_tag>)
				|| (std::derived_from<std::remove_cvref_t<TRhs>, pipe>
					&& derived_from_unified_base<TRhs, functional::detail::enable_operators_base_tag>)
	[[nodiscard]]
	constexpr auto operator |(
		TLhs&& lhs,
		TRhs&& rhs
	) noexcept(is_nothrow_composable_v<pipe, TLhs, TRhs>)
	{
		return functional::detail::make_composition_from_tag<pipe>(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs));
	}

	/** @} */
}

#endif
