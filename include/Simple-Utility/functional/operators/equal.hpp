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

	struct not_equal_caller_fn
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
					return (std::invoke(std::forward<TFunctions>(functions), callArgs...) != ...);
				},
				std::forward<TFunctionsTuple>(functionsTuple)
			);
		}
	};

	struct equal_compare_base_tag
	{};

	struct not_equal_compare_base_tag
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
		using composer_t = detail::compose_helper_t<TClosureBase, detail::equal_caller_fn>;
	};

	/**
	 * \brief Helper type which enables unequal compare composition of two functional objects via operator !=.
	 * \tparam TDerived The most derived class type.
	 * \tparam TClosureBase The base closure type (with one template argument left to be specified).
	 */
	template <class TDerived, template <class> class TClosureBase>
		requires std::is_class_v<TDerived> && std::same_as<TDerived, std::remove_cvref_t<TDerived>>
	struct not_equal_compare
		: private unified_base<detail::not_equal_compare_base_tag>
	{
		using composer_t = detail::compose_helper_t<TClosureBase, detail::not_equal_caller_fn>;
	};

	/** @} */
}

namespace sl::functional::operators::detail
{
	template <class TDerived, template <class> class TClosureBase>
	constexpr auto make_equal_compare_composer_impl([[maybe_unused]] const equal_compare<TDerived, TClosureBase>&) noexcept
	{
		return typename equal_compare<TDerived, TClosureBase>::composer_t{};
	}

	template <class TLhs, class TRhs>
		requires derived_from_unified_base<TLhs, equal_compare_base_tag>
				|| derived_from_unified_base<TRhs, equal_compare_base_tag>
	constexpr auto make_equal_compare_composer([[maybe_unused]] const TLhs& lhs, [[maybe_unused]] const TRhs& rhs) noexcept
	{
		if constexpr (derived_from_unified_base<TLhs, equal_compare_base_tag>)
		{
			return make_equal_compare_composer_impl(lhs);
		}
		else
			return make_equal_compare_composer_impl(rhs);
	}

	template <class TDerived, template <class> class TClosureBase>
	constexpr auto make_not_equal_compare_composer_impl([[maybe_unused]] const not_equal_compare<TDerived, TClosureBase>&) noexcept
	{
		return typename not_equal_compare<TDerived, TClosureBase>::composer_t{};
	}

	template <class TLhs, class TRhs>
		requires derived_from_unified_base<TLhs, not_equal_compare_base_tag>
				|| derived_from_unified_base<TRhs, not_equal_compare_base_tag>
	constexpr auto make_not_equal_compare_composer([[maybe_unused]] const TLhs& lhs, [[maybe_unused]] const TRhs& rhs) noexcept
	{
		if constexpr (derived_from_unified_base<TLhs, not_equal_compare_base_tag>)
		{
			return make_not_equal_compare_composer_impl(lhs);
		}
		else
			return make_not_equal_compare_composer_impl(rhs);
	}
}

namespace sl::functional::operators
{
	/**
	 * \addtogroup GROUP_FUNCTIONAL_OPERATORS
	 * @{
	 */

	/**
	 * \brief Composes both functional objects as equal comparison.
	 * \relatesto equal_compare
	 * \tparam TLhs The left-hand-side functional type.
	 * \tparam TRhs The right-hand-side functional type.
	 * \param lhs The left-hand-side functional object.
	 * \param rhs The right-hand-side functional object.
	 * \return The equality compare composition of both functional objects as new functional object.
	 */
	template <class TLhs, class TRhs>
		requires derived_from_unified_base<TLhs, detail::equal_compare_base_tag>
				|| derived_from_unified_base<TRhs, detail::equal_compare_base_tag>
	constexpr auto operator ==(TLhs&& lhs, TRhs&& rhs)
	{
		return detail::make_equal_compare_composer(lhs, rhs)(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs));
	}

	/**
	 * \brief Composes both functional objects as unequal comparison.
	 * \relatesto not_equal_compare
	 * \tparam TLhs The left-hand-side functional type.
	 * \tparam TRhs The right-hand-side functional type.
	 * \param lhs The left-hand-side functional object.
	 * \param rhs The right-hand-side functional object.
	 * \return The inequality compare composition of both functional objects as new functional object.
	 */
	template <class TLhs, class TRhs>
		requires derived_from_unified_base<TLhs, detail::not_equal_compare_base_tag>
				|| derived_from_unified_base<TRhs, detail::not_equal_compare_base_tag>
	constexpr auto operator !=(TLhs&& lhs, TRhs&& rhs)
	{
		return detail::make_not_equal_compare_composer(lhs, rhs)(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs));
	}

	/** @} */
}

#endif
