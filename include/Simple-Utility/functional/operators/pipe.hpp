//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_OPERATORS_PIPE_HPP
#define SL_UTILITY_FUNCTIONAL_OPERATORS_PIPE_HPP

#pragma once

#include "Simple-Utility/unified_base.hpp"
#include "Simple-Utility/functional/base.hpp"

namespace sl::functional::operators::detail
{
	struct nested_invoke_caller_fn
	{
		static constexpr composition_strategy_t composition_strategy{ composition_strategy_t::prefer_join };

		template <class TFunctionsTuple, class... TCallArgs>
		[[nodiscard]]
		constexpr decltype(auto) operator ()
		(
			TFunctionsTuple&& functionsTuple,
			TCallArgs&&... callArgs
		) const
		{
			return std::apply(
				[&]<std::invocable<TCallArgs...> TFunc, class... TOthers>(TFunc&& func, TOthers&&... otherFuncs) -> decltype(auto)
				{
					return recursive_invoke(
						std::invoke(std::forward<TFunc>(func), std::forward<TCallArgs>(callArgs)...),
						std::forward<TOthers>(otherFuncs)...
					);
				},
				std::forward<TFunctionsTuple>(functionsTuple)
			);
		}

	private:
		template <class TInput, std::invocable<TInput> TFunc, class... TOthers>
		[[nodiscard]]
		static constexpr decltype(auto) recursive_invoke
		(
			TInput&& input,
			TFunc&& func,
			TOthers&&... otherFunctions
		)
		{
			if constexpr (0 == sizeof...(TOthers))
			{
				return std::invoke(std::forward<TFunc>(func), std::forward<TInput>(input));
			}
			else
			{
				return recursive_invoke(
					std::invoke(std::forward<TFunc>(func), std::forward<TInput>(input)),
					std::forward<TOthers>(otherFunctions)...
				);
			}
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
	 * \brief Tag type which enables pipe composing of two functional objects via operator |.
	 * \relatesalso sl::functional::enable_operation
	 */
	struct pipe
	{};

	/**
	 * \brief Specialized traits for \ref pipe.
	 * \relatesalso pipe
	 */
	template <>
	struct tag_traits<pipe>
	{
		using operation_t = detail::nested_invoke_caller_fn;
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
	constexpr auto operator |
	(
		TLhs&& lhs,
		TRhs&& rhs
	)
	noexcept(is_nothrow_composable_v<pipe, TLhs, TRhs>)
	{
		return functional::detail::make_composition_from_tag<pipe>(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs));
	}

	/** @} */
}

#endif
