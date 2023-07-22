//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_TUPLE_GENERAL_HPP
#define SL_UTILITY_TUPLE_GENERAL_HPP

#pragma once

#include <concepts>
#include <tuple>
#include <type_traits>
#include <utility>

#include "Simple-Utility/TypeList.hpp"

namespace sl::tuple
{
	/**
	 * \defgroup GROUP_TUPLE tuple
	 */
}

namespace sl::tuple::detail
{
	// use an explicit type here, because a simple lambda type seems to confuse resharper a lot!
	template <std::size_t index>
	struct get_fn
	{
		template <class Tuple>
		[[nodiscard]]
		constexpr decltype(auto) operator ()(Tuple&& tuple) const noexcept
		{
			using std::get;
			return get<index>(std::forward<decltype(tuple)>(tuple));
		}
	};

	template <std::size_t index>
	inline constexpr get_fn<index> get{};

	template <std::size_t index, class Tuple>
	struct get_result
	{
		using type = decltype(get<index>(std::declval<Tuple>()));
	};

	template <std::size_t index, class Tuple>
	using get_result_t = typename get_result<index, Tuple>::type;
}

namespace sl::concepts::detail
{
	template <std::size_t index, class Tuple>
	concept tuple_index = std::convertible_to<
			tuple::detail::get_result_t<index, Tuple&>,
			const std::tuple_element_t<index, Tuple>&>
		&& std::convertible_to<
			tuple::detail::get_result_t<index, const Tuple&>,
			const std::tuple_element_t<index, Tuple>&>
		&& std::convertible_to<
			tuple::detail::get_result_t<index, Tuple&&>,
			const std::tuple_element_t<index, Tuple>&&>
		&& std::convertible_to<
			tuple::detail::get_result_t<index, const Tuple&&>,
			const std::tuple_element_t<index, Tuple>&&>;

	template <class Tuple>
	inline constexpr bool tuple_indices_v = []<std::size_t... indices>([[maybe_unused]] std::index_sequence<indices...>)
	{
		return (... && tuple_index<indices, Tuple>);
	}(std::make_index_sequence<std::tuple_size_v<Tuple>>{});

	template <class Func, class Tuple>
	inline constexpr bool applicable_v = []<std::size_t... indices>([[maybe_unused]] std::index_sequence<indices...>)
	{
		return std::is_invocable_v<
			Func,
			tuple::detail::get_result_t<indices, Tuple>...>;
	}(std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<Tuple>>>{});

	template <class Func, class Tuple>
	inline constexpr bool nothrow_applicable_v = []<std::size_t... indices>([[maybe_unused]] std::index_sequence<indices...>)
	{
		return std::is_nothrow_invocable_v<
			Func,
			tuple::detail::get_result_t<indices, Tuple>...>;
	}(std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<Tuple>>>{});
}

namespace sl::concepts
{
	/**
	 * \brief Determines whether a type can be used as a tuple-like.
	 * \details Requires the type-traits ``std::tuple_size`` to be defined for the given type and its member ``value`` denoting the the correct tuple size.
	 * The ``std::tuple_element`` trait and the ``get`` function must to be defined for each index in the interval ``[0, N)``, where ``N`` is the tuple size of the given type.
	 * \concept tuple
	 * \ingroup GROUP_TUPLE GROUP_UTILITY_CONCEPTS
	 * \see https://en.cppreference.com/w/cpp/utility/tuple_element
	 * \see https://en.cppreference.com/w/cpp/utility/tuple_size
	 * \tparam Tuple Type to check.
	 */
	template <class Tuple>
	concept tuple = type_list<Tuple>
					&& detail::tuple_indices_v<Tuple>;

	/**  
	 * \brief Determines whether the function is invocable with the elements of the given tuple.
	 * \see https://en.cppreference.com/w/cpp/utility/apply
	 * \ingroup GROUP_TUPLE_STL_EXT_APPLY GROUP_UTILITY_CONCEPTS
	 * \tparam Func Type to check.
	 * \tparam Tuple Provided tuple argument type.
	 */
	template <class Func, class Tuple>
	concept applicable = tuple<std::remove_cvref_t<Tuple>>
						&& detail::applicable_v<Func, Tuple>;

	/**
	 * \brief Determines whether the function is invocable with the elements of the given tuple without throwing.
	 * \see https://en.cppreference.com/w/cpp/utility/apply
	 * \ingroup GROUP_TUPLE_STL_EXT_APPLY GROUP_UTILITY_CONCEPTS
	 * \tparam Func Type to check.
	 * \tparam Tuple Provided tuple argument type.
	 */
	template <class Func, class Tuple>
	concept nothrow_applicable = tuple<std::remove_cvref_t<Tuple>>
								&& applicable<Func, Tuple>
								&& detail::nothrow_applicable_v<Func, Tuple>;
}

namespace sl::tuple
{
	/**
	 * \defgroup GROUP_TUPLE_STL_EXT stl extension
	 * \ingroup GROUP_TUPLE
	 */

	/**
	 * \defgroup GROUP_TUPLE_STL_EXT_APPLY apply
	 * \ingroup GROUP_TUPLE_STL_EXT
	 * @{
	 */

	/**
	 * \brief Trait type determining the result of a ``std::apply`` call.
	 * \see https://en.cppreference.com/w/cpp/utility/apply
	 * \tparam Func Type to check.
	 * \tparam Tuple Provided tuple argument type.
	 */
	template <class Func, class Tuple>
		requires concepts::applicable<Func, Tuple>
	struct apply_result
	{
		using type = decltype(std::apply(std::declval<Func>(), std::declval<Tuple>()));
	};

	/**
	 * \brief Alias type determining the result of a ``std::apply`` call.
	 * \see https://en.cppreference.com/w/cpp/utility/apply
	 * \tparam Func Type to check.
	 * \tparam Tuple Provided tuple argument type.
	 */
	template <class Func, class Tuple>
	using apply_result_t = typename apply_result<Func, Tuple>::type;

	/** @} */

	/**
	 * \defgroup GROUP_TUPLE_STL_EXT_CAT cat
	 * \ingroup GROUP_TUPLE_STL_EXT
	 * @{
	 */

	/**
	 * \brief Trait type determining the result of a ``std::tuple_cat`` call.
	 */
	template <class Tuple, class... Others>
		requires concepts::tuple<std::remove_cvref_t<Tuple>>
				&& (... && concepts::tuple<std::remove_cvref_t<Others>>)
	struct cat_result
	{
		using type = decltype(std::tuple_cat(
			std::declval<Tuple>(),
			std::declval<Others>()...
		));
	};

	/**
	 * \brief Alias type determining the result of a ``std::tuple_cat`` call.
	 * \see https://en.cppreference.com/w/cpp/utility/tuple/tuple_cat
	 */
	template <class... Tuples>
	using tuple_cat_result_t = typename cat_result<Tuples...>::type;

	/** @} */
}

#endif
