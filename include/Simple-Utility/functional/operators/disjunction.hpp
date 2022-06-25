//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_OPERATORS_DISJUNCTION_HPP
#define SL_UTILITY_FUNCTIONAL_OPERATORS_DISJUNCTION_HPP

#pragma once

#include "Simple-Utility/unified_base.hpp"
#include "Simple-Utility/functional/base.hpp"

namespace sl::functional::operators::detail
{
	struct disjunction_caller_fn
	{
		static constexpr composition_strategy_t composition_strategy{ composition_strategy_t::prefer_join };

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
					return (std::invoke(std::forward<TFunctions>(functions), callArgs...) || ...);
				},
				std::forward<TFunctionsTuple>(functionsTuple)
			);
		}
	};

	struct disjunction_base_tag
	{};
}

namespace sl::functional::operators
{
	/**
	 * \addtogroup GROUP_FUNCTIONAL_OPERATORS operators
	 * \ingroup GROUP_FUNCTIONAL
	 * @{
	 */

	/**
	 * \brief Helper type which enables disjunctive composing of two functional objects via operator ||.
	 * \tparam TDerived The most derived class type.
	 * \tparam TClosureBase The base closure type (with one template argument left to be specified).
	 */
	template <class TDerived, template <class> class TClosureBase>
		requires std::is_class_v<TDerived> && std::same_as<TDerived, std::remove_cvref_t<TDerived>>
	struct disjunction
		: private unified_base<detail::disjunction_base_tag>
	{
	private:
		using composer_t = detail::compose_helper_t<TClosureBase, detail::disjunction_caller_fn>;

	public:
		/**
		 * \brief Composes this and the other functional object as disjunction.
		 * \tparam TOther The type of the other functional object.
		 * \param other The right-hand-side functional object.
		 * \return The disjunctive composition of this and other as new functional object.
		 */
		template <class TOther>
		[[nodiscard]]
		constexpr auto operator ||
		(
			TOther&& other
		) && noexcept(std::is_nothrow_invocable_v<composer_t, TDerived&&, TOther>)
		{
			return composer_t{}(static_cast<TDerived&&>(*this), std::forward<TOther>(other));
		}

		/**
		 * \copydoc operator||()
		 */
		template <class TOther>
		[[nodiscard]]
		constexpr auto operator ||
		(
			TOther&& other
		) const & noexcept(std::is_nothrow_invocable_v<composer_t, const TDerived&, TOther>)
		{
			return composer_t{}(static_cast<const TDerived&>(*this), std::forward<TOther>(other));
		}

		/**
		 * \brief Composes both functional objects as disjunction.
		 * \tparam TLhs The left-hand-side functional type.
		 * \param lhs The left-hand-side functional object.
		 * \param rhs The right-hand-side functional object.
		 * \return The disjunctive composition of both functional objects as new functional object.
		 */
		template <class TLhs>
			requires (!derived_from_unified_base<TLhs, detail::disjunction_base_tag>)
		[[nodiscard]]
		friend constexpr auto operator ||
		(
			TLhs&& lhs,
			disjunction&& rhs
		) noexcept(std::is_nothrow_invocable_v<composer_t, TLhs, TDerived&&>)
		{
			return composer_t{}(std::forward<TLhs>(lhs), static_cast<TDerived&&>(rhs));
		}

		/**
		 * \copydoc operator||(TLhs&&, disjunction&&)
		 */
		template <class TLhs>
			requires (!derived_from_unified_base<TLhs, detail::disjunction_base_tag>)
		[[nodiscard]]
		friend constexpr auto operator ||
		(
			TLhs&& lhs,
			const disjunction& rhs
		) noexcept(std::is_nothrow_invocable_v<composer_t, TLhs, const TDerived&>)
		{
			return composer_t{}(std::forward<TLhs>(lhs), static_cast<const TDerived&>(rhs));
		}
	};

	/** @} */
}

#endif
