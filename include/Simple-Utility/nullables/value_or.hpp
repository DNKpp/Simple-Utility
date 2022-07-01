//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_NULLABLES_VALUE_OR_HPP
#define SL_UTILITY_NULLABLES_VALUE_OR_HPP

#pragma once

#include "Simple-Utility/nullables/base.hpp"
#include "Simple-Utility/functional/transform.hpp"

#include "Simple-Utility/bind_back.hpp"

namespace sl::nullables::detail
{
	template <input_nullable TInputNullable, std::convertible_to<value_t<TInputNullable>> TAlternative>
	[[nodiscard]]
	constexpr value_t<TInputNullable> value_or(TInputNullable&& inputNullable, TAlternative&& alternative)
	{
		if (inputNullable != null_v<TInputNullable>)
		{
			return value_t<TInputNullable>{ unwrap(std::forward<TInputNullable>(inputNullable)) };
		}
		return value_t<TInputNullable>{ std::forward<TAlternative>(alternative) };
	}

	class value_or_caller_fn
	{
	public:
		template <input_nullable TInputNullable, std::convertible_to<value_t<TInputNullable>> TAlternative>
		[[nodiscard]]
		constexpr value_t<TInputNullable> operator ()
		(
			TInputNullable&& inputNullable,
			TAlternative&& alternative
		) const
		noexcept(noexcept(value_or(std::declval<TInputNullable>(), std::declval<TAlternative>())))
		{
			return value_or(
				std::forward<TInputNullable>(inputNullable),
				std::forward<TAlternative>(alternative)
			);
		}
	};
}

namespace sl::nullables
{
	/**
	 * \brief Retrieves the value of a \ref sl::nullables::input_nullable "input_nullable" if it's not equal to its ''null''-object.
	 * Returns the alternative otherwise.
	 * \ingroup GROUP_NULLABLES_ALGORITHMS
	 * \tparam TValue Type of alternative. Must initialize ``nullable_value_t<TNullable>`` objects.
	 *
	 * \details This algorithm returns the alternative if the \ref sl::nullables::input_nullable "input_nullable" compares equal to its
	 * ``null``-object. Otherwise the value of the \ref sl::nullables::input_nullable "input_nullable" is returned.
	 *
	 * \note In the following examples the outcome is always presented within the ``REQUIRE()`` statement.
	 *
	 * This example shows the outcome when a valid \ref sl::nullables::input_nullable "input_nullable" is used in a ``value_or`` expression.
	 * \snippet algorithm.cpp value_or valid
	 * \---
	 *
	 * This example shows the outcome  when an invalid \ref sl::nullables::input_nullable "input_nullable" is used in a ``value_or`` expression.
	 * \snippet algorithm.cpp value_or invalid
	 */
	template <class TValue>
	[[nodiscard]]
	constexpr auto value_or(TValue&& value)
	{
		return algorithm_fn{ detail::value_or_caller_fn{} } >> std::forward<TValue>(value);
	}

	template <class TValue>
	[[nodiscard]]
	constexpr auto value_or_fn(TValue&& value)
	{
		return functional::transform_fn{ value_or(std::forward<TValue>(value)) };
	}
}

#endif
