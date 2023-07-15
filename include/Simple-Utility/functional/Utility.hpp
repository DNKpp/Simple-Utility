//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_UTILITY_HPP
#define SL_UTILITY_FUNCTIONAL_UTILITY_HPP

#pragma once

#include "Simple-Utility/concepts/operators.hpp"
#include "Simple-Utility/concepts/stl_extensions.hpp"
#include "Simple-Utility/functional/Transform.hpp"

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
	 * \tparam To The target type.
	 */
	template <class To>
	inline constexpr auto as = envelop<Transform>(
		[]<class From>(From&& arg) constexpr noexcept(concepts::nothrow_explicitly_convertible_to<From, To>) -> To
		{
			static_assert(concepts::explicitly_convertible_to<From, To>, "Argument is not convertible to target type.");

			return static_cast<To>(std::forward<From>(arg));
		});

	/**
	 * \brief Functional object which dereferences the given argument and returns the result.
	 */
	inline constexpr auto dereference = envelop<Transform>(
		[]<class T>(T&& arg) constexpr noexcept(noexcept(*std::declval<T>())) -> decltype(auto)
		{
			static_assert(concepts::dereferencable<T>, "Argument is not usable as operand of unary operator *.");

			return *std::forward<T>(arg);
		});

	/**
	 * \brief Functional object which returns the address of the given argument.
	 * \see https://en.cppreference.com/w/cpp/memory/addressof
	 */
	inline constexpr auto addressof = envelop<Transform>(
		[](auto& arg) constexpr noexcept
		{
			static_assert(requires { std::addressof(arg); }, "Argument is not usable for std::addressof.");

			return std::addressof(arg);
		});

	/** @} */
}

#endif
