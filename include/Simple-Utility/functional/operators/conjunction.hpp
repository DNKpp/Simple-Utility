//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_OPERATORS_CONJUNCTION_HPP
#define SL_UTILITY_FUNCTIONAL_OPERATORS_CONJUNCTION_HPP

#pragma once

#include "Simple-Utility/functional/base.hpp"

namespace sl::functional::operators::detail
{
	struct conjunction_caller_fn
	{
		template <class TFunctionsTuple, class... TCallArgs>
		[[nodiscard]]
		constexpr auto operator ()
		(
			TFunctionsTuple&& functionsTuple,
			const TCallArgs&... callArgs
		) const
		noexcept(functional::detail::is_type_list_nothrow_invokable_v<TFunctionsTuple, const TCallArgs&...>)
		{
			return std::apply(
				[&]<std::predicate<const TCallArgs&...>... TFunctions>(TFunctions&&... functions)
				{
					return (std::invoke(std::forward<TFunctions>(functions), callArgs...) && ...);
				},
				std::forward<TFunctionsTuple>(functionsTuple)
			);
		}
	};

	struct conjunction_base_tag
	{};
}

namespace sl::functional::operators
{
	/**
	 * \addtogroup GROUP_FUNCTIONAL_OPERATORS
	 * \ingroup GROUP_FUNCTIONAL
	 * @{
	 */
	/**
	 * \brief Tag type which enables conjunctive composing of two functional objects via operator &&.
	 * \relatesalso sl::functional::enable_operation
	 */
	struct conjunction
	{};

	/**
	 * \brief Specialized traits for \ref conjunction.
	 * \relatesalso conjunction
	 */
	template <>
	struct tag_traits<conjunction>
	{
		using operation_t = detail::conjunction_caller_fn;
		inline static constexpr composition_strategy_t composition_strategy{ composition_strategy_t::prefer_join };
	};

	/**
	 * \brief Composes both functional objects as conjunction.
	 * \relatesalso conjunction
	 * \tparam TLhs The left-hand-side functional type.
	 * \tparam TRhs The right-hand-side functional type.
	 * \param lhs The left-hand-side functional object.
	 * \param rhs The right-hand-side functional object.
	 * \return The conjunctive composition of both functional objects as new functional object.
	 */
	template <class TLhs, class TRhs>
		requires (std::derived_from<std::remove_cvref_t<TLhs>, conjunction>
				&& derived_from_unified_base<TLhs, functional::detail::enable_operators_base_tag>)
				|| (std::derived_from<std::remove_cvref_t<TRhs>, conjunction>
					&& derived_from_unified_base<TRhs, functional::detail::enable_operators_base_tag>)
	[[nodiscard]]
	constexpr auto operator &&
	(
		TLhs&& lhs,
		TRhs&& rhs
	)
	noexcept(is_nothrow_composable_v<conjunction, TLhs, TRhs>)
	{
		return functional::detail::make_composition_from_tag<conjunction>(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs));
	}

	/** @} */
}

#endif
