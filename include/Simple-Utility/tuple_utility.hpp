//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_TUPLE_UTILITY_HPP
#define SL_UTILITY_TUPLE_UTILITY_HPP

#pragma once

#include <concepts>
#include <tuple>
#include <type_traits>
#include <utility>

namespace sl::detail
{
	template <class...>
	struct is_apply_invocable
		: std::false_type
	{
	};

	template <class TFunc, class TTuple, std::size_t... VIndices>
	struct is_apply_invocable<TFunc, TTuple, std::index_sequence<VIndices...>>
		: std::bool_constant<std::invocable<TFunc, std::tuple_element_t<VIndices, TTuple>...>>
	{
	};

	template <class...>
	struct is_nothrow_apply_invocable
		: std::false_type
	{
	};

	template <class TFunc, class TTuple, std::size_t... VIndices>
	struct is_nothrow_apply_invocable<TFunc, TTuple, std::index_sequence<VIndices...>>
		: std::bool_constant<std::is_nothrow_invocable_v<TFunc, std::tuple_element_t<VIndices, TTuple>...>>
	{
	};
}

namespace sl
{
	/**
	 * \defgroup GROUP_TUPLE_UTILITY tuple utility
	 * @{
	 */

	/**
	 * \brief Trait type determining whether the function is invocable with elements of the given tuple.
	 */
	template <class TFunc, class TTuple>
	struct is_apply_invocable
		: detail::is_apply_invocable<TFunc, TTuple, std::make_index_sequence<std::tuple_size_v<TTuple>>>
	{
	};

	/**
	 * \brief Convenience constant, which determines whether the function is invocable with elements of the given tuple.
	 * \relates is_apply_invocable
	 */
	template <class TFunc, class TTuple>
	inline constexpr bool is_apply_invocable_v{is_apply_invocable<TFunc, TTuple>::value};

	/**
	 * \brief Trait type determining whether the function is invocable with elements of the given tuple without throwing.
	 */
	template <class TFunc, class TTuple>
	struct is_nothrow_apply_invocable
		: detail::is_nothrow_apply_invocable<TFunc, TTuple, std::make_index_sequence<std::tuple_size_v<TTuple>>>
	{
	};

	/**
	 * \brief Convenience constant, which determines whether the function is invocable with elements of the given tuple
	 * without throwing.
	 * \relates is_nothrow_apply_invocable
	 */
	template <class TFunc, class TTuple>
	inline constexpr bool is_nothrow_apply_invocable_v{is_nothrow_apply_invocable<TFunc, TTuple>::value};

	/**
	 * \brief Trait type determining the result of a ``std::tuple_cat`` call.
	 */
	template <class... TTuples>
	struct tuple_cat_result
	{
		using type = decltype(std::tuple_cat(std::declval<TTuples>()...));
	};

	/**
	 * \brief Alias type determining the result of a ``std::tuple_cat`` call.
	 */
	template <class... TTuples>
	using tuple_cat_result_t = typename tuple_cat_result<TTuples...>::type;

	/**
	 * \brief Trait type determining the result of a ``std::apply`` call.
	 */
	template <class TFunc, class TTuple>
	struct apply_invoke_result
	{
		using type = decltype(std::apply(std::declval<TFunc>(), std::declval<TTuple>()));
	};

	/**
	 * \brief Alias type determining the result of a ``std::apply`` call.
	 */
	template <class TFunc, class TTuple>
	using apply_invoke_result_t = typename apply_invoke_result<TFunc, TTuple>::type;

	//template <class TFirst, class TSecond, class... TOthers>
	//using tuple_zip_result_t =
}

namespace sl::detail
{
	template <std::size_t VIndex, class... TTuples>
		requires ((std::tuple_size_v<std::remove_cvref_t<TTuples>> == 0) || ...)
	constexpr auto zip_tuple_element(TTuples&&...)
	{
		return std::tuple{};
	}

	template <std::size_t VIndex, class... TTuples>
	constexpr auto zip_tuple_element(TTuples&&... tuples)
	{
		using zipped_t = std::tuple<std::tuple_element_t<VIndex, std::remove_cvref_t<TTuples>>...>;

		if constexpr (((VIndex + 1 < std::tuple_size_v<std::remove_cvref_t<TTuples>>) && ...))
		{
			return std::tuple_cat(
				std::make_tuple(zipped_t{std::get<VIndex>(std::forward<TTuples>(tuples))...}),
				zip_tuple_element<VIndex + 1>(std::forward<TTuples>(tuples)...)
			);
		}
		else
		{
			return std::make_tuple(zipped_t{std::get<VIndex>(std::forward<TTuples>(tuples))...});
		}
	}

	/** @} */
}

namespace sl
{
	/**
	 * \addtogroup GROUP_TUPLE_UTILITY
	 * @{
	 */

	/**
	 * \brief Trait type determining the result of a ``tuple_zip`` call.
	 */
	template <class TFirst, class TSecond, class... TOthers>
	struct tuple_zip_result
	{
		using type = decltype(detail::zip_tuple_element<0>(
			std::declval<TFirst>(),
			std::declval<TSecond>(),
			std::declval<TOthers>()...
		));
	};

	/**
	 * \brief Alias type determining the result of a ``tuple_zip`` call.
	 */
	template <class TFirst, class TSecond, class... TOthers>
	using tuple_zip_result_t = typename tuple_zip_result<TFirst, TSecond, TOthers...>::type;

	/**
	 * \brief Zips elements of all provided source tuples and creates a tuple of tuples.
	 * \details Combines all given tuples into one tuple with tuples as elements. Each element tuples have equal size, which is
	 * the amount of provide source tuples. The minimal amount of elements of the given source tuples determine the amount of
	 * resulting tuple elements. If any of the given tuples are empty, then the resulting tuple will contain no elements.
	 * \tparam TFirst The first tuple type.
	 * \tparam TSecond The second tuple type.
	 * \tparam TOthers Other tuple types.
	 * \param first The first tuple.
	 * \param second The second tuple.
	 * \param others Other tuples.
	 * \return A new tuple which elements are tuples.
	 */
	template <class TFirst, class TSecond, class... TOthers>
	[[nodiscard]]
	constexpr tuple_zip_result_t<TFirst, TSecond, TOthers...> tuple_zip(TFirst&& first, TSecond&& second, TOthers&&... others)
	{
		return detail::zip_tuple_element<0>(
			std::forward<TFirst>(first),
			std::forward<TSecond>(second),
			std::forward<TOthers>(others)...
		);
	}

	/** @} */
}

#endif
