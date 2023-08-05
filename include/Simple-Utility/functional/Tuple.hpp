//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_TUPLE_HPP
#define SL_UTILITY_FUNCTIONAL_TUPLE_HPP

#pragma once

#include "Simple-Utility/concepts/utility.hpp"
#include "Simple-Utility/functional/Transform.hpp"
#include "Simple-Utility/tuple/General.hpp"

#include <tuple>
#include <utility>

namespace sl::functional::tuple
{
	/**
	 * \defgroup GROUP_FUNCTIONAL_TUPLE tuple
	 * \ingroup GROUP_FUNCTIONAL GROUP_TUPLE_UTILITY
	 * \{
	 */

	/**
	 * \brief Functional object which retrieves an object of a specific type from a tuple-like argument.
	 * \tparam T The type to be retrieved.
	 */
	template <class T>
	inline constexpr auto get = envelop<Transform>(
		[]<class Tuple>(Tuple&& v) -> decltype(auto)requires concepts::tuple<std::remove_cvref_t<Tuple>>
		{
			using std::get;
			return get<T>(std::forward<Tuple>(v));
		});

	/**
	 * \brief Functional object which retrieves an object at a specific index from a tuple-like argument.
	 * \tparam index The index of type to be retrieved.
	 */
	template <std::size_t index>
	inline constexpr auto get_at = envelop<Transform>(
		[]<class Tuple>(Tuple&& v) -> decltype(auto)requires concepts::tuple<std::remove_cvref_t<Tuple>>
		{
			using std::get;
			return get<index>(std::forward<Tuple>(v));
		});
}

namespace sl::functional::tuple::detail
{
	// workaround
	// as of MSVC version 19.32.31328.0 the following workaround became necessary.
	template <class... Args, class Tuple>
		requires concepts::tuple<std::remove_cvref_t<Tuple>>
	constexpr std::tuple<Args...> reduce(Tuple&& tuple)
	{
		return {tuple::get<Args>(std::forward<Tuple>(tuple))...};
	}

	// workaround
	// as of MSVC version 19.32.31328.0 the following workaround became necessary.
	template <class... Tuples>
		requires (... && concepts::tuple<std::remove_cvref_t<Tuples>>)
	constexpr auto concat(Tuples&&... tuples)
	{
		return std::tuple_cat(std::forward<Tuples>(tuples)...);
	}
}

namespace sl::functional::tuple
{
	/**
	 * \brief Reduces (or permutes) the components of a tuple and returns a them as new tuple.
	 * \tparam Args The component types of the returned tuple.
	 * \remark The source and target tuples must consist of unique types.
	 */
	template <class... Args>
		requires concepts::unique_types<Args...>
	inline constexpr auto reduce = envelop<Transform>(
		[]<class Tuple>(Tuple&& t)
			requires concepts::tuple<std::remove_cvref_t<Tuple>>
		{
			return detail::reduce<Args...>(std::forward<Tuple>(t));
		});

	/**
	 * \brief Combines all elements from each given tuple into one tuple.
	 */
	inline constexpr auto concat = envelop<Transform>(
		[]<class... Tuples>(Tuples&&... tuples)
			requires (... && concepts::tuple<std::remove_cvref_t<Tuples>>)
		{
			return detail::concat(std::forward<Tuples>(tuples)...);
		});

	/**
	 * \brief Combines all given lvalue references into one tuple.
	 */
	inline constexpr auto tie = envelop<Transform>(
		[](auto&... args) { return std::tie(args...); });

	/**
	 * \brief Constructs an object with elements from the source tuple as constructor arguments.
	 */
	template <class To>
	inline constexpr auto make_from = envelop<Transform>(
		[]<class Tuple>(Tuple&& t) noexcept(noexcept(std::make_from_tuple<To>(std::forward<Tuple>(t))))
			requires concepts::tuple<std::remove_cvref_t<Tuple>>
		{
			// Due to a bug in gcc-10, we must check this requirement in two steps
			static_assert(
				requires { { std::make_from_tuple<To>(std::forward<Tuple>(t)) }; },
				"The object is not constructible with the elements of the given tuple.");
			static_assert(
				std::same_as<To, decltype(std::make_from_tuple<To>(std::declval<Tuple>()))>,
				"The object is not constructible with the elements of the given tuple.");

			return std::make_from_tuple<To>(std::forward<Tuple>(t));
		});

	/**
	 * \}
	 */
}

namespace sl::functional
{
	/**
	* \brief Stores a functional and forwards elements from provided tuples as params on invocation.
	* \ingroup GROUP_FUNCTIONAL
	*/
	template <function Fn>
	using Apply = BasicClosure<Fn, NodiscardApplyPolicy, PipeOperator>;
}

#endif
