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
	 * \defgroup GROUP_FUNCTIONAL_TRANSFORM transform
	 * \brief Contains the base ``transform_fn`` and several pre-defined transform objects.
	 * \ingroup GROUP_FUNCTIONAL
	 * \details Transform types aim to simplify the composition of multiple transformations on objects and are therefore composable via
	 * pipe operator (operator |). Transforms also aim to be flat as possible, which means, if users chain multiple transforms, instead of simply
	 * building a tree like structure, the functional objects will be combined into one ``composition_fn``. This keeps the calling-hierarchy as
	 * flat as possible and also supports easier debugging. Operations following this strategy are:
	 * - pipe
	 * - bind_front
	 * - bind_back
	 * @{
	 */

	/**
	 * \brief Adapter type which accepts a functional type and enables pipe chaining and currying.
	 * \tparam TFunc The functional type.
	 */
	template <class TFunc>
		requires std::same_as<TFunc, std::remove_cvref_t<TFunc>>
	class transform_fn final
		: public closure_base_fn<TFunc>,
		public enable_operation<transform_fn,
								operators::pipe,
								operators::bind_front,
								operators::bind_back
		>
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
	transform_fn(TFunc) -> transform_fn<TFunc>;

	namespace detail
	{
		template <class TTarget>
		inline constexpr auto as_impl = []<std::convertible_to<TTarget> T>
		(
			T&& v
		)
		noexcept(std::is_nothrow_convertible_v<T, TTarget>)
		-> TTarget
		{
			return static_cast<TTarget>(std::forward<T>(v));
		};
	}

	/**
	 * \brief Functional object which static_cast the given argument to the target type on invocation.
	 * \tparam TTarget The target type.
	 */
	template <class TTarget>
	inline constexpr transform_fn as{ detail::as_impl<TTarget> };

	/** @} */
}

#endif
