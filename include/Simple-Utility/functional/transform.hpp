//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_TRANSFORM_HPP
#define SL_UTILITY_FUNCTIONAL_TRANSFORM_HPP

#pragma once

#include "Simple-Utility/unified_base.hpp"
#include "Simple-Utility/functional/base.hpp"
#include "Simple-Utility/functional/operators/pipe.hpp"
#include "Simple-Utility/functional/operators/bind.hpp"

namespace sl::functional
{
	/**
	 * \addtogroup GROUP_FUNCTIONAL
	 *
	 * @{
	 */

	/**
	 * \brief Helper type which accepts a functional type and enables pipe chaining.
	 * \tparam TFunc The functional type.
	 */
	template <class TFunc>
		requires std::same_as<TFunc, std::remove_cvref_t<TFunc>>
	class transform_fn
		: public closure_base_fn<TFunc>,
		public operators::pipe<transform_fn<TFunc>, transform_fn>,
		public operators::bind_front<transform_fn<TFunc>, transform_fn>,
		public operators::bind_back<transform_fn<TFunc>, transform_fn>
	{
		using closure_t = closure_base_fn<TFunc>;
	public:
		using closure_t::closure_t;
	};

	/**
	 * \brief Deduction guide.
	 * \tparam TFunc Type of the given functional.
	 */
	template <class TFunc>
	transform_fn(TFunc) -> transform_fn<std::remove_cvref_t<TFunc>>;

	/**
	 * \defgroup GROUP_FUNCTIONAL_TRANSFORM transform
	 * @{
	 */

	/**
	 * \brief Functional object which static_cast the given argument to the target type on invocation.
	 * \tparam TTarget The target type.
	 */
	template <class TTarget>
	inline constexpr transform_fn as{
		[]<class T>(T&& v) -> TTarget { return static_cast<TTarget>(std::forward<T>(v)); }
	};

	/** @} */

	/** @} */
}

#endif
