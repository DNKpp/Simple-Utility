//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_NULLABLES_OR_ELSE_HPP
#define SL_UTILITY_NULLABLES_OR_ELSE_HPP

#pragma once

#include "Simple-Utility/nullables/base.hpp"
#include "Simple-Utility/functional/transform.hpp"

namespace sl::nullables::detail
{
	template <nullable TNullable, std::invocable TFunc>
		requires std::is_void_v<std::invoke_result_t<TFunc>>
				|| std::constructible_from<std::remove_cvref_t<TNullable>, std::invoke_result_t<TFunc>>
	[[nodiscard]]
	constexpr std::remove_cvref_t<TNullable> or_else(TNullable&& nullableObj, TFunc&& func)
	{
		using result_t = std::remove_cvref_t<TNullable>;

		if (nullableObj == null_v<TNullable>)
		{
			if constexpr (std::is_void_v<std::invoke_result_t<TFunc>>)
			{
				std::invoke(std::forward<TFunc>(func));
				return result_t{ null_v<TNullable> };
			}
			else
			{
				return result_t{ std::invoke(std::forward<TFunc>(func)) };
			}
		}
		return std::forward<TNullable>(nullableObj);
	}

	class or_else_caller_fn
	{
	public:
		template <nullable TNullable, std::invocable TFunc>
		[[nodiscard]]
		constexpr std::remove_cvref_t<TNullable> operator ()
		(
			TNullable&& nullableObj,
			TFunc&& func
		) const
		noexcept(noexcept(or_else(std::declval<TNullable>(), std::declval<TFunc>())))
		{
			return or_else(
				std::forward<TNullable>(nullableObj),
				std::forward<TFunc>(func)
			);
		}
	};
}

namespace sl::nullables
{
	/**
	 * \addtogroup GROUP_NULLABLES_ALGORITHMS
	 * @{
	 */

	/**
	 * \brief Returns the \ref sl::nullables::nullable "nullable" if it's not equal to its ''null''-object. Executes the passed function otherwise.
	 * \tparam TFunc The type of the passed function. The return type must be either ```void`` or such that it can be
	 * used to initialize a new nullable object with the same type as used on.
	 *
	 * \details This algorithm simply forwards the \ref sl::nullables::nullable "nullable" if it compares unequal to its ``null``-object.
	 * Otherwise the passed function will be called and the returned value initializes a new \ref sl::nullables::nullable "nullable"-object of the same type.
	 * If the functions return type is ``void``, the ``null``-object of that \ref sl::nullables::nullable "nullable"-type will be returned instead.
	 *
	 * \note In the following examples the outcome is always presented within the ``REQUIRE()`` statement.
	 * 
	 * This example shows the outcome when a valid \ref sl::nullables::nullable "nullable" is used in a ``or_else`` expression.
	 * \snippet algorithm.cpp or_else valid non-void return
	 * \---
	 * 
	 * This example shows the outcome when an invalid \ref sl::nullables::nullable "nullable" is used in a ``or_else`` expression and the functional has a
	 * return type other than ``void``.
	 * \snippet algorithm.cpp or_else invalid non-void return
	 * \---
	 * 
	 * This example shows the outcome when an invalid \ref sl::nullables::nullable "nullable" is used in a ``or_else`` expression and the functional does not
	 * return anything.
	 * \snippet algorithm.cpp or_else invalid void return
	 */
	template <std::invocable TFunc>
	[[nodiscard]]
	constexpr auto or_else(TFunc&& func)
	{
		return algorithm_fn{ detail::or_else_caller_fn{} } >> std::forward<TFunc>(func);
	}

	/** @} */
}

#endif
