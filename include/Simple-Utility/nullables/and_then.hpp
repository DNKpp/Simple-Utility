//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_NULLABLES_AND_THEN_HPP
#define SL_UTILITY_NULLABLES_AND_THEN_HPP

#pragma once

#include "Simple-Utility/nullables/base.hpp"
#include "Simple-Utility/functional/transform.hpp"

namespace sl::nullables::detail
{
	template <input_nullable TInputNullable, std::invocable<dereference_result_t<TInputNullable>> TFunc>
		requires nullable<std::invoke_result_t<TFunc, dereference_result_t<TInputNullable>>>
	[[nodiscard]]
	constexpr std::invoke_result_t<TFunc, dereference_result_t<TInputNullable>> and_then(TInputNullable&& inputNullable, TFunc&& func)
	{
		using result_nullable_t = std::invoke_result_t<TFunc, dereference_result_t<TInputNullable>>;

		if (inputNullable != null_v<TInputNullable>)
		{
			return std::invoke(std::forward<TFunc>(func), unwrap(std::forward<TInputNullable>(inputNullable)));
		}
		return result_nullable_t{ null_v<result_nullable_t> };
	}

	class and_then_caller_fn
	{
	public:
		template <input_nullable TInputNullable, std::invocable<dereference_result_t<TInputNullable>> TFunc>
			requires nullable<std::invoke_result_t<TFunc, dereference_result_t<TInputNullable>>>
		[[nodiscard]]
		constexpr auto operator ()
		(
			TInputNullable&& inputNullable,
			TFunc&& func
		) const
		noexcept(noexcept(and_then(std::declval<TInputNullable>(), std::declval<TFunc>())))
		{
			return and_then(
				std::forward<TInputNullable>(inputNullable),
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
	 * \brief Passes the value of the \ref sl::nullables::input_nullable "input_nullable" to the function if it's not equal to its ``null``-object.
	 * Returns the ``null``-object otherwise.
	 * \tparam TFunc The type of the passed function. The function should be invokable with types returned by \ref sl::nullables::unwrap
	 * "unwrap" and return a type which satisfies the \ref sl::nullables::nullable "nullable" concept (this type does not have to be
	 * the same as the type already used in the expression).
	 * \param func The given functional object.
	 *
	 * \details This algorithm uses the actual value of a \ref sl::nullables::input_nullable "input_nullable" and passes it to the given functional.
	 * The functional should then return \b any \ref sl::nullables::nullable "nullable" \b compatible \b type as desired.
	 * Otherwise, if the \ref sl::nullables::input_nullable "input_nullable" compares equal to its ``null``-object, the ``null``-object is returned instead.
	 *
	 * \note In the following examples the outcome is always presented within the ``REQUIRE()`` statement.
	 *
	 * This example shows the outcome when a valid \ref sl::nullables::input_nullable "input_nullable" is used in a ``and_then`` expression.
	 * Note that the resulting \ref sl::nullables::nullable "nullable" type differs from the starting one.
	 * \snippet algorithm.cpp and_then valid
	 * \---
	 *
	 * This example shows the outcome when an invalid \ref sl::nullables::input_nullable "input_nullable" is used in a ``and_then`` expression.
	 * Note that the resulting \ref sl::nullables::nullable "nullable" type differs from the starting one.
	 * \snippet algorithm.cpp and_then invalid
	 */
	template <class TFunc>
	[[nodiscard]]
	constexpr auto and_then(TFunc&& func)
	{
		return algorithm_fn{ detail::and_then_caller_fn{} } >> std::forward<TFunc>(func);
	}

	/** @} */
}

#endif
