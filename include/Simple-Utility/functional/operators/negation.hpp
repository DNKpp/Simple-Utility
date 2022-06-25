//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_OPERATORS_NEGATION_HPP
#define SL_UTILITY_FUNCTIONAL_OPERATORS_NEGATION_HPP

#pragma once

#include "Simple-Utility/functional/base.hpp"

namespace sl::functional::operators
{
	/**
	 * \addtogroup GROUP_FUNCTIONAL_OPERATORS operators
	 * \ingroup GROUP_FUNCTIONAL
	 * @{
	 */

	/**
	 * \brief Helper type which enables negation on functionals via operator !.
	 * \tparam TDerived The most derived class type.
	 * \tparam TClosureBase The base closure type (with one template argument left to be specified).
	 */
	template <class TDerived, template <class> class TClosureBase>
		requires std::is_class_v<TDerived> && std::same_as<TDerived, std::remove_cvref_t<TDerived>>
	struct negation
	{
	private:
		template <class TFunc>
		using negated_fn = std::remove_cvref_t<decltype(std::not_fn(std::declval<TFunc>()))>;

	public:
		/**
		 * \brief Negates this functional object.
		 * \return The negation of this functional, as a new functional object.
		 */
		[[nodiscard]]
		constexpr auto operator !() && noexcept(
			std::is_nothrow_constructible_v<TClosureBase<negated_fn<TDerived&&>>, negated_fn<TDerived&&>>
		)
		{
			return TClosureBase<negated_fn<TDerived&&>>{ std::not_fn(static_cast<TDerived&&>(*this)) };
		}

		/**
		 * \copydoc operator!()
		 */
		[[nodiscard]]
		constexpr auto operator !() const & noexcept(
			std::is_nothrow_constructible_v<TClosureBase<negated_fn<const TDerived&>>, negated_fn<const TDerived&>>
		)
		{
			return TClosureBase<negated_fn<const TDerived&>>{ std::not_fn(static_cast<const TDerived&>(*this)) };
		}
	};

	/** @} */
}

#endif
