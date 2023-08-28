//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_UTILITY_HPP
#define SIMPLE_UTILITY_UTILITY_HPP

#pragma once

#include "Simple-Utility/concepts/stl_extensions.hpp"

#include <concepts>
#include <cstddef>
#include <tuple>

namespace sl
{
	/**
	 * \defgroup GROUP_PRIORITY_TAG priority_tag
	 * \brief Helper type, useful when prioritize overloads of an overload set.
	 * \see Got the idea from here: https://quuxplusone.github.io/blog/2021/07/09/priority-tag/
	* \{
	*/

	/**
	 * \brief Primary template, inheriting from the other specializations with lesser priority.
	 */
	template <std::size_t priority>
	struct priority_tag
		: public priority_tag<priority - 1>
	{
	};

	/**
	 * \brief Specialization, denoting the least likely alternative.
	 */
	template <>
	struct priority_tag<0>
	{
	};

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_IN_PLACE in_place
	 * \brief Provides convenient option, to construct arguments in-place.
	* \{
	*/

	/**
	 * \brief Helper type, enabling deferred construction, which will then (thanks to copy-elision) in-place construct the ``Type`` instance.
	 * \tparam Type The type to be constructed.
	 * \tparam Args The constructor argument types.
	 * \param args The constructor arguments.
	 * \note Instances of this type are not intended to be materialized anywhere. They should only be used during one expression.
	 */
	template <concepts::unqualified Type, class... Args>
		requires std::constructible_from<Type, Args...>
	struct in_place_constructor
	{
	public:
		in_place_constructor(const in_place_constructor&) = delete;
		in_place_constructor& operator =(const in_place_constructor&) = delete;
		in_place_constructor(in_place_constructor&&) = delete;
		in_place_constructor& operator =(in_place_constructor&&) = delete;

		~in_place_constructor() = default;

		[[nodiscard]]
		in_place_constructor() = default;

		[[nodiscard]]
		explicit constexpr operator Type() && noexcept(std::is_nothrow_constructible_v<Type, Args...>)
		{
			return std::make_from_tuple<Type>(std::move(m_Args));
		}

		template <class T, class... Ts>
		friend constexpr in_place_constructor<T, Ts&&...> in_place(Ts&&... args) noexcept;

	private:
		std::tuple<Args...> m_Args;

		[[nodiscard]]
		explicit constexpr in_place_constructor(std::tuple<Args...>&& args) noexcept
			: m_Args{std::move(args)}
		{
		}
	};

	/**
	 * \brief Forwards the given arguments as tuple into the internal storage.
	 * \tparam Type The type to be constructed.
	 * \tparam Args The constructor argument types.
	 * \param args The constructor arguments.
	 * \return in_place_constructor instance, storing the arguments as forwarding references.
	 */
	template <class Type, class... Args>
	[[nodiscard]]
	constexpr in_place_constructor<Type, Args&&...> in_place(Args&&... args) noexcept
	{
		return in_place_constructor<Type, Args&&...>{std::forward_as_tuple(std::forward<Args>(args)...)};
	}

	/**
	 * \}
	 */
}

#endif
