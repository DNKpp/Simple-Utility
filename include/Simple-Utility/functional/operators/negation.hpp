//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_OPERATORS_NEGATION_HPP
#define SL_UTILITY_FUNCTIONAL_OPERATORS_NEGATION_HPP

#pragma once

#include "Simple-Utility/functional/base.hpp"
#include "Simple-Utility/functional/operators/pipe.hpp"

#include <functional>

namespace sl::functional::operators
{
	/**
	 * \addtogroup GROUP_FUNCTIONAL_OPERATORS
	 * \ingroup GROUP_FUNCTIONAL
	 * @{
	 */

	/**
	 * \brief Tag type which enables negation on functionals via operator !.
	 * \relatesalso sl::functional::enable_operation
	 */
	struct negate
	{};

	/**
	 * \brief Specialized traits for \ref negate.
	 * \relatesalso negate
	 */
	template <>
	struct tag_traits<negate>
	{
		using operation_t = detail::nested_invoke_caller_fn;
		inline static constexpr composition_strategy_t composition_strategy{ composition_strategy_t::nested_only };
	};

	/**
	 * \brief Negates the the functional object.
	 * \relatesalso negate
	 * \tparam TFunc The functional type.
	 * \param func The functional object.
	 * \return The negated functional, as a new functional object.
	 */
	template <derived_from_unified_base<functional::detail::enable_operators_base_tag> TFunc>
		requires std::derived_from<std::remove_cvref_t<TFunc>, negate>
	[[nodiscard]]
	constexpr auto operator !
	(
		TFunc&& func
	)
	noexcept(is_nothrow_composable_v<negate, TFunc, decltype(std::bind_front(std::equal_to{}, false))&&>)
	{
		return functional::detail::make_composition_from_tag<negate>(
			std::forward<TFunc>(func),
			std::bind_front(std::equal_to{}, false)
		);
	}

	/** @} */
}

#endif
