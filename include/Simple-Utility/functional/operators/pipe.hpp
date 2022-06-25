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

	struct pipe_base_tag
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
	 * \brief Helper type which enables pipe composing of two functional objects via operator |.
	 * \tparam TDerived The most derived class type.
	 * \tparam TClosureBase The base closure type (with one template argument left to be specified).
	 */
	template <class TDerived, template <class> class TClosureBase>
		requires std::is_class_v<TDerived> && std::same_as<TDerived, std::remove_cvref_t<TDerived>>
	struct pipe
		: private unified_base<detail::pipe_base_tag>
	{
	private:
		using composer_t = detail::compose_helper_t<TClosureBase, detail::nested_invoke_caller_fn>;

	public:
		/**
		 * \brief Composes this and the other functional object as nested function
		 * (result of left-hand-side as argument for right-hand-side).
		 * \tparam TOther The type of the other functional object.
		 * \param other The right-hand-side functional object.
		 * \return The nested composition of this and other as new functional object.
		 */
		template <class TOther>
		[[nodiscard]]
		constexpr auto operator |
		(
			TOther&& other
		) const & noexcept(std::is_nothrow_invocable_v<composer_t, const TDerived&, TOther>)
		{
			return composer_t{}(static_cast<const TDerived&>(*this), std::forward<TOther>(other));
		}

		/**
		 * \copydoc operator|()
		 */
		template <class TOther>
		[[nodiscard]]
		constexpr auto operator |
		(
			TOther&& other
		) && noexcept(std::is_nothrow_invocable_v<composer_t, TDerived&&, TOther>)
		{
			return composer_t{}(static_cast<TDerived&&>(*this), std::forward<TOther>(other));
		}

		/**
		 * \brief Composes both functional objects as nested function (result of left-hand-side as argument for right-hand-side).
		 * \tparam TLhs The left-hand-side functional type.
		 * \param lhs The left-hand-side functional object.
		 * \param rhs The right-hand-side functional object.
		 * \return The nested composition of both functional objects as new functional object.
		 */
		template <class TLhs>
			requires (!derived_from_unified_base<TLhs, detail::pipe_base_tag>)
		[[nodiscard]]
		friend constexpr auto operator |
		(
			TLhs&& lhs,
			pipe&& rhs
		)
		noexcept(std::is_nothrow_invocable_v<composer_t, TLhs, TDerived&&>)
		{
			return composer_t{}(std::forward<TLhs>(lhs), static_cast<TDerived&&>(rhs));
		}

		/**
		 * \copydoc operator|(TLhs&&, pipe&&)
		 */
		template <class TLhs>
			requires (!derived_from_unified_base<TLhs, detail::pipe_base_tag>)
		[[nodiscard]]
		friend constexpr auto operator |
		(
			TLhs&& lhs,
			const pipe& rhs
		)
		noexcept(std::is_nothrow_invocable_v<composer_t, TLhs, const TDerived&>)
		{
			return composer_t{}(std::forward<TLhs>(lhs), static_cast<const TDerived&>(rhs));
		}
	};

	/** @} */
}

#endif
