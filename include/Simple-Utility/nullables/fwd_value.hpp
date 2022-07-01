//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_NULLABLES_FWD_VALUE_HPP
#define SL_UTILITY_NULLABLES_FWD_VALUE_HPP

#pragma once

#include "Simple-Utility/nullables/base.hpp"
#include "Simple-Utility/functional/transform.hpp"

namespace sl::nullables::detail
{
	template <input_nullable TInputNullable, std::invocable<dereference_result_t<TInputNullable>> TFunc>
	constexpr void fwd_value(TInputNullable&& inputNullable, TFunc&& func)
	{
		if (inputNullable != null_v<TInputNullable>)
		{
			std::invoke(std::forward<TFunc>(func), unwrap(std::forward<TInputNullable>(inputNullable)));
		}
	}

	class fwd_value_caller_fn
	{
	public:
		template <input_nullable TInputNullable, std::invocable<dereference_result_t<TInputNullable>> TFunc>
		constexpr void operator ()
		(
			TInputNullable&& inputNullable,
			TFunc&& func
		) const
		noexcept(noexcept(fwd_value(std::declval<TInputNullable>(), std::declval<TFunc>())))
		{
			fwd_value(
				std::forward<TInputNullable>(inputNullable),
				std::forward<TFunc>(func)
			);
		}
	};
}

namespace sl::nullables
{
	/**
	 * \brief Passes the value of the \ref sl::nullables::input_nullable "input_nullable" to the function if it's not equal to its ``null``-object.
	 * \tparam TFunc The type of the passed function. The function should be invokable with types returned by \ref sl::nullables::unwrap
	 * ``unwrap``.
	 *
	 * \details This algorithm uses the actual value of a \ref sl::nullables::input_nullable "input_nullable" and passes it to the given functional.
	 * If the \ref sl::nullables::input_nullable "input_nullable" compares equal to its ``null``-object, nothing happens.
	 *
	 * \note In the following examples the outcome is always presented within the ``REQUIRE()`` statement.
	 *
	 * This example shows the outcome when a valid \ref sl::nullables::input_nullable "input_nullable" is used in a ``fwd_value`` expression.
	 * \snippet algorithm.cpp fwd_value valid
	 * \---
	 *
	 * This example shows the outcome when an invalid \ref sl::nullables::input_nullable "input_nullable" is used in a ``fwd_value`` expression.
	 * \snippet algorithm.cpp fwd_value invalid
	 */
	template <class TFunc>
	[[nodiscard]]
	constexpr auto fwd_value(TFunc&& func)
	{
		return algorithm_fn{ detail::fwd_value_caller_fn{} } >> std::forward<TFunc>(func);
	}

	template <class TFunc>
	[[nodiscard]]
	constexpr auto fwd_value_fn(TFunc&& func)
	{
		return functional::transform_fn{ fwd_value(std::forward<TFunc>(func)) };
	}
}

#endif
