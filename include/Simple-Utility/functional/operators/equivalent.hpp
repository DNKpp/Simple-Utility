//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_OPERATORS_EQUIVALENT_HPP
#define SL_UTILITY_FUNCTIONAL_OPERATORS_EQUIVALENT_HPP

#pragma once

#include "Simple-Utility/unified_base.hpp"
#include "Simple-Utility/functional/base.hpp"

namespace sl::functional::operators::detail
{
	struct equivalent_caller_fn
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
				[&]<std::predicate<const TCallArgs&...> TInitFunction, std::predicate<const TCallArgs&...>... TOtherFunctions>
			(TInitFunction&& initFunction, TOtherFunctions&&... otherFunctions)
				{
					const bool init = std::invoke(std::forward<TInitFunction>(initFunction), callArgs...);
					return ((init == std::invoke(std::forward<TOtherFunctions>(otherFunctions), callArgs...)) && ...);
				},
				std::forward<TFunctionsTuple>(functionsTuple)
			);
		}
	};

	struct equivalent_compare_base_tag
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
	 * \brief Helper type which enables equivalence compare composition of two functional objects via operator <=>.
	 * \tparam TDerived The most derived class type.
	 * \tparam TClosureBase The base closure type (with one template argument left to be specified).
	 */
	template <class TDerived, template <class> class TClosureBase>
		requires std::is_class_v<TDerived> && std::same_as<TDerived, std::remove_cvref_t<TDerived>>
	struct equivalent_compare
		: private unified_base<detail::equivalent_compare_base_tag>
	{
	private:
		using composer_t = detail::compose_helper_t<TClosureBase, detail::equivalent_caller_fn>;

	public:
		/**
		 * \brief Composes both functional objects via equivalent comparison.
		 * \tparam TRhs The right-hand-side functional type.
		 * \param lhs The left-hand-side functional object.
		 * \param rhs The right-hand-side functional object.
		 * \return The equivalent comparison of both functional objects as new functional object.
		 */
		template <class TRhs>
			requires (!derived_from_unified_base<TRhs, detail::equivalent_compare_base_tag>)
		[[nodiscard]]
		friend constexpr auto operator <=>
		(
			equivalent_compare&& lhs,
			TRhs&& rhs
		) noexcept(std::is_nothrow_invocable_v<composer_t, TDerived&&, TRhs>)
		{
			return composer_t{}(static_cast<TDerived&&>(lhs), std::forward<TRhs>(rhs));
		}

		/**
		 * \copydoc operator<=>(equivalent_compare&&, TRhs&&)
		 */
		template <class TRhs>
			requires (!derived_from_unified_base<TRhs, detail::equivalent_compare_base_tag>)
		[[nodiscard]]
		friend constexpr auto operator <=>
		(
			const equivalent_compare& lhs,
			TRhs&& rhs
		) noexcept(std::is_nothrow_invocable_v<composer_t, const TDerived&, TRhs>)
		{
			return composer_t{}(static_cast<const TDerived&>(lhs), std::forward<TRhs>(rhs));
		}

		/**
		 * \brief Composes both functional objects via equivalent comparison.
		 * \tparam TLhs The left-hand-side functional type.
		 * \param lhs The left-hand-side functional object.
		 * \param rhs The right-hand-side functional object.
		 * \return The equivalent comparison of both functional objects as new functional object.
		 */
		template <class TLhs>
			requires (!derived_from_unified_base<TLhs, detail::equivalent_compare_base_tag>)
		[[nodiscard]]
		friend constexpr auto operator <=>
		(
			TLhs&& lhs,
			equivalent_compare&& rhs
		) noexcept(std::is_nothrow_invocable_v<composer_t, TLhs, TDerived&&>)
		{
			return composer_t{}(std::forward<TLhs>(lhs), static_cast<TDerived&&>(rhs));
		}

		/**
		 * \copydoc operator<=>(TLhs&&, equivalent_compare&&)
		 */
		template <class TLhs>
			requires (!derived_from_unified_base<TLhs, detail::equivalent_compare_base_tag>)
		[[nodiscard]]
		friend constexpr auto operator <=>
		(
			TLhs&& lhs,
			const equivalent_compare& rhs
		) noexcept(std::is_nothrow_invocable_v<composer_t, TLhs, const TDerived&>)
		{
			return composer_t{}(std::forward<TLhs>(lhs), static_cast<const TDerived&>(rhs));
		}

		/**
		 * \brief Composes both functional objects via equivalent comparison.
		 * \tparam UDerived The derived class of right-hand-side.
		 * \tparam UBaseDerived The base derived class of right-hand-side.
		 * \param lhs The left-hand-side functional object.
		 * \param rhs The right-hand-side functional object.
		 * \return The equivalent comparison of both functional objects as new functional object.
		 */
		template <class UDerived, template <class> class UBaseDerived>
		[[nodiscard]]
		friend constexpr auto operator <=>
		(
			const equivalent_compare& lhs,
			const equivalent_compare<UDerived, UBaseDerived>& rhs
		)
		noexcept(std::is_nothrow_invocable_v<composer_t, const TDerived&, const UDerived&>)
		{
			return composer_t{}(static_cast<const TDerived&>(lhs), static_cast<const UDerived&>(rhs));
		}

		/**
		 * \copydoc operator<=>(const equivalent_compare&, const equivalent_compare<UDerived, UBaseDerived>&)
		 */
		template <class UDerived, template <class> class UBaseDerived>
		[[nodiscard]]
		friend constexpr auto operator <=>
		(
			equivalent_compare&& lhs,
			const equivalent_compare<UDerived, UBaseDerived>& rhs
		)
		noexcept(std::is_nothrow_invocable_v<composer_t, TDerived&&, const UDerived&>)
		{
			return composer_t{}(static_cast<TDerived&&>(lhs), static_cast<const UDerived&>(rhs));
		}

		/**
		 * \copydoc operator<=>(const equivalent_compare&, const equivalent_compare<UDerived, UBaseDerived>&)
		 */
		template <class UDerived, template <class> class UBaseDerived>
		[[nodiscard]]
		friend constexpr auto operator <=>
		(
			const equivalent_compare& lhs,
			equivalent_compare<UDerived, UBaseDerived>&& rhs
		)
		noexcept(std::is_nothrow_invocable_v<composer_t, const TDerived&, UDerived&&>)
		{
			return composer_t{}(static_cast<const TDerived&>(lhs), static_cast<UDerived&&>(rhs));
		}

		/**
		 * \copydoc operator<=>(const equivalent_compare&, const equivalent_compare<UDerived, UBaseDerived>&)
		 */
		template <class UDerived, template <class> class UBaseDerived>
		[[nodiscard]]
		friend constexpr auto operator <=>
		(
			equivalent_compare&& lhs,
			equivalent_compare<UDerived, UBaseDerived>&& rhs
		)
		noexcept(std::is_nothrow_invocable_v<composer_t, TDerived&&, UDerived&&>)
		{
			return composer_t{}(static_cast<TDerived&&>(lhs), static_cast<UDerived&&>(rhs));
		}
	};

	/** @} */
}

#endif
