//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_TUPLE_UTILITY_HPP
#define SL_UTILITY_TUPLE_UTILITY_HPP

#pragma once

#include <tuple>

namespace sl
{
	/**
	 * \defgroup GROUP_TUPLE_UTILITY tuple utility
	 * @{
	 */

	template <class... TTuples>
	struct tuple_cat_result
	{
		using type = decltype(std::tuple_cat(std::declval<TTuples>()...));
	};

	template <class... TTuples>
	using tuple_cat_result_t = typename tuple_cat_result<TTuples...>::type;

	template <class TFunc, class TTuple>
	struct apply_invoke_result
	{
		using type = decltype(std::apply(std::declval<TFunc>(), std::declval<TTuple>()));
	};

	template <class TFunc, class TTuple>
	using apply_invoke_result_t = typename apply_invoke_result<TFunc, TTuple>::type;

	/** @} */
}

#endif
