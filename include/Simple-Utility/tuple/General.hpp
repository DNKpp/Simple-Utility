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

namespace sl::concepts::detail
{
	template <std::size_t index, class Tuple>
	inline constexpr bool check_tuple_index_v = []
	{
		using std::get;
		return requires
		{
			{ get<index>(std::declval<Tuple&>()) } -> std::common_reference_with<std::tuple_element_t<index, Tuple>>;
			{ get<index>(std::declval<const Tuple&>()) } -> std::common_reference_with<std::tuple_element_t<index, Tuple>>;
			{ get<index>(std::declval<Tuple&&>()) } -> std::common_reference_with<std::tuple_element_t<index, Tuple>>;
			{ get<index>(std::declval<const Tuple&&>()) } -> std::common_reference_with<std::tuple_element_t<index, Tuple>>;
		};
	}();

	template <class Tuple>
	inline constexpr bool check_tuple_indices_v = []<std::size_t... indices>([[maybe_unused]] std::index_sequence<indices...>)
	{
		return (check_tuple_index_v<indices, Tuple> && ...);
	}(std::make_index_sequence<std::tuple_size_v<Tuple>>{});
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
					&& detail::check_tuple_indices_v<Tuple>;

	/**  
	 * \brief Determines whether the function is invocable with the elements of the given tuple.
	 * \see https://en.cppreference.com/w/cpp/utility/apply
	 * \ingroup GROUP_TUPLE_STL_EXT_APPLY GROUP_UTILITY_CONCEPTS
	 * \tparam Func Type to check.
	 * \tparam Tuple Provided tuple argument type.
	 */
	template <class Func, class Tuple>
	concept applicable = tuple<std::remove_cvref_t<Tuple>>
						&& []<std::size_t... indices>(std::index_sequence<indices...>)
						{
							using std::get;
							return std::invocable<Func, decltype(get<indices>(std::declval<Tuple>()))...>;
						}(std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<Tuple>>>{});

	/**
	 * \brief Determines whether the function is invocable with the elements of the given tuple without throwing.
	 * \see https://en.cppreference.com/w/cpp/utility/apply
	 * \ingroup GROUP_TUPLE_STL_EXT_APPLY GROUP_UTILITY_CONCEPTS
	 * \tparam Func Type to check.
	 * \tparam Tuple Provided tuple argument type.
	 */
	template <class Func, class Tuple>
	concept nothrow_applicable = tuple<std::remove_cvref_t<Tuple>>
								&& []<std::size_t... indices>(std::index_sequence<indices...>)
								{
									using std::get;
									return std::is_nothrow_invocable_v<Func, decltype(get<indices>(std::declval<Tuple>()))...>;
								}(std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<Tuple>>>{});
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
	 * \defgroup GROUP_TUPLE_STL_EXT_TUPLE_CAT tuple cat
	 * \ingroup GROUP_TUPLE_STL_EXT
	 * @{
	 */

	/**
	 * \brief Trait type determining the result of a ``std::tuple_cat`` call.
	 */
	template <class... TTuples>
		requires (concepts::tuple<std::remove_cvref_t<TTuples>> && ...)
	struct tuple_cat_result
	{
		using type = decltype(std::tuple_cat(std::declval<TTuples>()...));
	};

	/**
	 * \brief Alias type determining the result of a ``std::tuple_cat`` call.
	 * \see https://en.cppreference.com/w/cpp/utility/tuple/tuple_cat
	 */
	template <class... TTuples>
	using tuple_cat_result_t = typename tuple_cat_result<TTuples...>::type;

	/** @} */
}

#endif
