//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_TUPLE_HPP
#define SL_UTILITY_FUNCTIONAL_TUPLE_HPP

#pragma once

#include "Simple-Utility/functional/transform.hpp"

#include <utility>
#include <tuple>

namespace sl::functional::tuple
{
	/**
	 * \defgroup GROUP_FUNCTIONAL_TUPLE tuple
	 * @{
	 */

	/**
	 * \brief Functional object which retrieves an object of a specific type from a tuple-like argument.
	 * \tparam T The type to be retrieved.
	 */
	template <class T>
	inline constexpr transform_fn get{
		[]<class TTuple>(TTuple&& v) -> decltype(auto)
		{
			using std::get;
			return get<T>(std::forward<TTuple>(v));
		}
	};

	/**
	 * \brief Functional object which retrieves an object at a specific index from a tuple-like argument.
	 * \tparam VIndex The index of type to be retrieved.
	 */
	template <std::size_t VIndex>
	inline constexpr transform_fn get_at{
		[]<class TTuple>(TTuple&& v) -> decltype(auto)
		{
			using std::get;
			return get<VIndex>(std::forward<TTuple>(v));
		}
	};

	/** @} */
}

#endif
