//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_UTILITY_HPP
#define SL_UTILITY_FUNCTIONAL_UTILITY_HPP

#pragma once

#include "Simple-Utility/concepts/operators.hpp"
#include "Simple-Utility/concepts/stl_extensions.hpp"
#include "Simple-Utility/functional/transform.hpp"

#include <memory>
#include <utility>

namespace sl::functional::util
{
	/**
	 * \defgroup GROUP_FUNCTIONAL_UTILITY utility
	 * \brief Contains functional objects, implementing several utility operations.
	 * \ingroup GROUP_FUNCTIONAL
	 * @{
	 */

	/**
	 * \brief Functional object which converts the given argument to the target type via static_cast.
	 * \tparam TTo The target type.
	 */
	template <class TTo>
	inline constexpr transform_fn as{
		[]<class TFrom>(TFrom&& arg) constexpr noexcept(concepts::nothrow_explicitly_convertible_to<TFrom, TTo>) -> TTo
		{
			static_assert(concepts::explicitly_convertible_to<TFrom, TTo>, "Argument is not convertible to target type.");

			return static_cast<TTo>(std::forward<TFrom>(arg));
		}
	};

	/**
	 * \brief Functional object which dereferences the given argument and returns the result.
	 */
	inline constexpr transform_fn dereference{
		[]<class T>(T&& arg) constexpr noexcept(noexcept(*std::forward<T>(arg))) -> decltype(auto)
		{
			static_assert(concepts::dereferencable<T>, "Argument is not usable as operand of unary operator *.");

			return *std::forward<T>(arg);
		}
	};

	/**
	 * \brief Functional object which returns the address of the given argument.
	 * \see https://en.cppreference.com/w/cpp/memory/addressof
	 */
	inline constexpr transform_fn addressof{
		[](auto& arg) constexpr noexcept
		{
			static_assert(requires { std::addressof(arg); }, "Argument is not usable for std::addressof.");

			return std::addressof(arg);
		}
	};

	/** @} */
}

#endif
