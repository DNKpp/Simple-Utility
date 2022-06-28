//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_OPERATORS_EQUAL_HPP
#define SL_UTILITY_FUNCTIONAL_OPERATORS_EQUAL_HPP

#pragma once

#include "Simple-Utility/concepts/utility.hpp"
#include "Simple-Utility/functional/base.hpp"

namespace sl::functional::operators::detail
{
	struct equal_caller_fn
	{
		template <class TCallArgsTuple, concepts::apply_invocable<TCallArgsTuple>... TFunctions>
		[[nodiscard]]
		constexpr auto operator ()
		(
			TCallArgsTuple&& callArgsTuple,
			TFunctions&&... functions
		) const
		noexcept((concepts::nothrow_apply_invocable<TFunctions, TCallArgsTuple> && ...))
		{
			return (std::apply(std::forward<TFunctions>(functions), callArgsTuple) == ...);
		}
	};

	struct not_equal_caller_fn
	{
		template <class TCallArgsTuple, concepts::apply_invocable<TCallArgsTuple>... TFunctions>
		[[nodiscard]]
		constexpr auto operator ()
		(
			TCallArgsTuple&& callArgsTuple,
			TFunctions&&... functions
		) const
		noexcept((concepts::nothrow_apply_invocable<TFunctions, TCallArgsTuple> && ...))
		{
			return (std::apply(std::forward<TFunctions>(functions), callArgsTuple) != ...);
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
	 * \brief Tag type which enables equal compare composition of two functional objects via operator ==.
	 * \relatesalso sl::functional::enable_operation
	 */
	struct equal
	{};

	/**
	 * \brief Specialized traits for \ref equal.
	 * \relatesalso equal
	 */
	template <>
	struct tag_traits<equal>
	{
		using operation_t = detail::equal_caller_fn;
		inline static constexpr composition_strategy_t composition_strategy{ composition_strategy_t::nested };
	};

	/**
	 * \brief Tag type which enables unequal compare composition of two functional objects via operator !=.
	 * \relatesalso sl::functional::enable_operation
	 */
	struct not_equal
	{};

	/**
	 * \brief Specialized traits for \ref not_equal.
	 * \relatesalso not_equal
	 */
	template <>
	struct tag_traits<not_equal>
	{
		using operation_t = detail::not_equal_caller_fn;
		inline static constexpr composition_strategy_t composition_strategy{ composition_strategy_t::nested };
	};

	/**
	 * \brief Composes both functional objects as equal comparison.
	 * \relates equal
	 * \tparam TLhs The left-hand-side functional type.
	 * \tparam TRhs The right-hand-side functional type.
	 * \param lhs The left-hand-side functional object.
	 * \param rhs The right-hand-side functional object.
	 * \return The equality compare composition of both functional objects as new functional object.
	 */
	template <class TLhs, class TRhs>
		requires (std::derived_from<std::remove_cvref_t<TLhs>, equal>
				&& derived_from_unified_base<TLhs, functional::detail::enable_operators_base_tag>)
				|| (std::derived_from<std::remove_cvref_t<TRhs>, equal>
					&& derived_from_unified_base<TRhs, functional::detail::enable_operators_base_tag>)
	[[nodiscard]]
	constexpr auto operator ==
	(
		TLhs&& lhs,
		TRhs&& rhs
	)
	noexcept(is_nothrow_composable_v<equal, TLhs, TRhs>)
	{
		return functional::detail::make_composition_from_tag<equal>(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs));
	}

	/**
	 * \brief Composes both functional objects as unequal comparison.
	 * \relates not_equal
	 * \tparam TLhs The left-hand-side functional type.
	 * \tparam TRhs The right-hand-side functional type.
	 * \param lhs The left-hand-side functional object.
	 * \param rhs The right-hand-side functional object.
	 * \return The inequality compare composition of both functional objects as new functional object.
	 */
	template <class TLhs, class TRhs>
		requires (std::derived_from<std::remove_cvref_t<TLhs>, not_equal>
				&& derived_from_unified_base<TLhs, functional::detail::enable_operators_base_tag>)
				|| (std::derived_from<std::remove_cvref_t<TRhs>, not_equal>
					&& derived_from_unified_base<TRhs, functional::detail::enable_operators_base_tag>)
	[[nodiscard]]
	constexpr auto operator !=
	(
		TLhs&& lhs,
		TRhs&& rhs
	)
	noexcept(is_nothrow_composable_v<not_equal, TLhs, TRhs>)
	{
		return functional::detail::make_composition_from_tag<not_equal>(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs));
	}

	/** @} */
}

#endif
