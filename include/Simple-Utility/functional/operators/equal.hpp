//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_OPERATORS_EQUAL_HPP
#define SL_UTILITY_FUNCTIONAL_OPERATORS_EQUAL_HPP

#pragma once

#include "Simple-Utility/unified_base.hpp"
#include "Simple-Utility/functional/base.hpp"

namespace sl::functional::operators::detail
{
	struct equal_caller_fn
	{
		static constexpr composition_strategy_t composition_strategy{ composition_strategy_t::nested_only };

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
					return (std::invoke(std::forward<TFunctions>(functions), callArgs...) == ...);
				},
				std::forward<TFunctionsTuple>(functionsTuple)
			);
		}
	};

	struct equal_compare_base_tag
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
	 * \brief Helper type which enables equal compare composition of two functional objects via operator ==.
	 * \tparam TDerived The most derived class type.
	 * \tparam TClosureBase The base closure type (with one template argument left to be specified).
	 */
	template <class TDerived, template <class> class TClosureBase>
		requires std::is_class_v<TDerived> && std::same_as<TDerived, std::remove_cvref_t<TDerived>>
	struct equal_compare
		: private unified_base<detail::equal_compare_base_tag>
	{
	private:
		using composer_t = detail::compose_helper_t<TClosureBase, detail::equal_caller_fn>;

	public:
		/**
		 * \brief Composes both functional objects via equal comparison.
		 * \tparam TRhs The right-hand-side functional type.
		 * \param lhs The left-hand-side functional object.
		 * \param rhs The right-hand-side functional object.
		 * \return The equal comparison of both functional objects as new functional object.
		 */
		template <class TRhs>
			requires (!derived_from_unified_base<TRhs, detail::equal_compare_base_tag>)
		[[nodiscard]]
		friend constexpr auto operator ==
		(
			equal_compare&& lhs,
			TRhs&& rhs
		) noexcept(std::is_nothrow_invocable_v<composer_t, TDerived&&, TRhs>)
		{
			return composer_t{}(static_cast<TDerived&&>(lhs), std::forward<TRhs>(rhs));
		}

		/**
		 * \copydoc operator==(equal_compare&&, TRhs&&)
		 */
		template <class TRhs>
			requires (!derived_from_unified_base<TRhs, detail::equal_compare_base_tag>)
		[[nodiscard]]
		friend constexpr auto operator ==
		(
			const equal_compare& lhs,
			TRhs&& rhs
		) noexcept(std::is_nothrow_invocable_v<composer_t, const TDerived&, TRhs>)
		{
			return composer_t{}(static_cast<const TDerived&>(lhs), std::forward<TRhs>(rhs));
		}

		/**
		 * \brief Composes both functional objects via equal comparison.
		 * \tparam TLhs The left-hand-side functional type.
		 * \param lhs The left-hand-side functional object.
		 * \param rhs The right-hand-side functional object.
		 * \return The equal comparison of both functional objects as new functional object.
		 */
		template <class TLhs>
			requires (!derived_from_unified_base<TLhs, detail::equal_compare_base_tag>)
		[[nodiscard]]
		friend constexpr auto operator ==
		(
			TLhs&& lhs,
			equal_compare&& rhs
		) noexcept(std::is_nothrow_invocable_v<composer_t, TLhs, TDerived&&>)
		{
			return composer_t{}(std::forward<TLhs>(lhs), static_cast<TDerived&&>(rhs));
		}

		/**
		 * \copydoc operator==(TLhs&&, equal_compare&&)
		 */
		template <class TLhs>
			requires (!derived_from_unified_base<TLhs, detail::equal_compare_base_tag>)
		[[nodiscard]]
		friend constexpr auto operator ==
		(
			TLhs&& lhs,
			const equal_compare& rhs
		) noexcept(std::is_nothrow_invocable_v<composer_t, TLhs, const TDerived&>)
		{
			return composer_t{}(std::forward<TLhs>(lhs), static_cast<const TDerived&>(rhs));
		}

		/**
		 * \brief Composes both functional objects via equal comparison.
		 * \tparam UDerived The derived class of right-hand-side.
		 * \tparam UBaseDerived The base derived class of right-hand-side.
		 * \param lhs The left-hand-side functional object.
		 * \param rhs The right-hand-side functional object.
		 * \return The equal comparison of both functional objects as new functional object.
		 */
		template <class UDerived, template <class> class UBaseDerived>
		[[nodiscard]]
		friend constexpr auto operator ==
		(
			const equal_compare& lhs,
			const equal_compare<UDerived, UBaseDerived>& rhs
		)
		noexcept(std::is_nothrow_invocable_v<composer_t, const TDerived&, const UDerived&>)
		{
			return composer_t{}(static_cast<const TDerived&>(lhs), static_cast<const UDerived&>(rhs));
		}

		/**
		 * \copydoc operator==(const equal_compare&, const equal_compare<UDerived, UBaseDerived>&)
		 */
		template <class UDerived, template <class> class UBaseDerived>
		[[nodiscard]]
		friend constexpr auto operator ==
		(
			equal_compare&& lhs,
			const equal_compare<UDerived, UBaseDerived>& rhs
		)
		noexcept(std::is_nothrow_invocable_v<composer_t, TDerived&&, const UDerived&>)
		{
			return composer_t{}(static_cast<TDerived&&>(lhs), static_cast<const UDerived&>(rhs));
		}

		/**
		 * \copydoc operator==(const equal_compare&, const equal_compare<UDerived, UBaseDerived>&)
		 */
		template <class UDerived, template <class> class UBaseDerived>
		[[nodiscard]]
		friend constexpr auto operator ==
		(
			const equal_compare& lhs,
			equal_compare<UDerived, UBaseDerived>&& rhs
		)
		noexcept(std::is_nothrow_invocable_v<composer_t, const TDerived&, UDerived&&>)
		{
			return composer_t{}(static_cast<const TDerived&>(lhs), static_cast<UDerived&&>(rhs));
		}

		/**
		 * \copydoc operator==(const equal_compare&, const equal_compare<UDerived, UBaseDerived>&)
		 */
		template <class UDerived, template <class> class UBaseDerived>
		[[nodiscard]]
		friend constexpr auto operator ==
		(
			equal_compare&& lhs,
			equal_compare<UDerived, UBaseDerived>&& rhs
		)
		noexcept(std::is_nothrow_invocable_v<composer_t, TDerived&&, UDerived&&>)
		{
			return composer_t{}(static_cast<TDerived&&>(lhs), static_cast<UDerived&&>(rhs));
		}
	};

	/** @} */
}

#endif
