//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_TUPLE_ALGORITHM_HPP
#define SL_UTILITY_TUPLE_ALGORITHM_HPP

#pragma once

#include <algorithm>
#include <functional>
#include <type_traits>

#include "Simple-Utility/tuple/General.hpp"

namespace sl::tuple
{
	/**
	 * \defgroup GROUP_TUPLE_ALGORITHM algorithm
	 * \ingroup GROUP_TUPLE
	 */

	/**
	 * \defgroup GROUP_TUPLE_ALGORITHM_TRANSFORM_ELEMENTS transform_elements
	 * \ingroup GROUP_TUPLE_ALGORITHM
	 * @{
	 */

	/**
	 * \brief Applies the transform on each element of the given source tuple and returns the results as a new tuple.
	 * \tparam Tuple The tuple type.
	 * \tparam Transform The transform type.
	 * \param tuple The source tuple.
	 * \param transform The transformation to be applied on each element.
	 * \return A newly created tuple with the transformed elements of the source tuple.
	 *
	 * \details The elements will be stored as they are retrieved from the transformation, thus no automatically
	 * conversion will be applied.
	 */
	template <class Tuple, class Transform>
		requires concepts::tuple<std::remove_cvref_t<Tuple>>
	[[nodiscard]]
	constexpr auto transform_elements(Tuple&& tuple, Transform transform)
	{
		return std::apply(
			[&]<class... Elements>(Elements&&... elements) -> std::tuple<std::invoke_result_t<Transform, Elements>...>
			{
				return {std::invoke(transform, std::forward<Elements>(elements))...};
			},
			std::forward<Tuple>(tuple));
	}

	/**
	 * \}
	 */
}

namespace sl::tuple::detail
{
	template <class Tuple>
		requires concepts::tuple<std::remove_cvref_t<Tuple>>
	[[nodiscard]]
	constexpr auto envelop_elements(Tuple&& tuple)
	{
		return transform_elements(
			std::forward<Tuple>(tuple),
			[]<class Element>(Element&& el) { return std::make_tuple(std::forward<Element>(el)); });
	}
}

namespace sl::tuple
{
	/**
	 * \defgroup GROUP_TUPLE_ALGORITHM_ENVELOP_ELEMENTS envelop elements
	 * \ingroup GROUP_TUPLE_ALGORITHM
	 * @{
	 */

	/**
	 * \brief Trait type determining the result of a ``tuple_envelop_elements`` call.
	 */
	template <class Tuple>
		requires concepts::tuple<std::remove_cvref_t<Tuple>>
	struct envelop_elements_result
	{
		using type = decltype(detail::envelop_elements(std::declval<Tuple>()));
	};

	/**
	 * \brief Alias type determining the result of a ``tuple_envelop_elements`` call.
	 */
	template <class Tuple>
		requires concepts::tuple<std::remove_cvref_t<Tuple>>
	using envelop_elements_result_t = typename envelop_elements_result<Tuple>::type;

	/**
	 * \brief Envelops all elements of the given tuple into their own ``std::tuple`` and creates a tuple of tuples.
	 * \tparam Tuple The tuple type.
	 * \param tuple The tuple.
	 * \return A new tuple which elements are tuples.
	 *
	 * \details Provided a tuple ``t0 = (e0, e1, ..., en)`` then the resulting tuple is built as follows:
	 * \code{.unparsed}
	 * (	(e0), (e1), ..., (en)	)
	 * \endcode
	 */
	template <class Tuple>
		requires concepts::tuple<std::remove_cvref_t<Tuple>>
	[[nodiscard]]
	constexpr envelop_elements_result_t<Tuple> envelop_elements(
		Tuple&& tuple
	) noexcept(std::is_nothrow_constructible_v<std::remove_cvref_t<Tuple>, Tuple>)
	{
		return detail::envelop_elements(std::forward<Tuple>(tuple));
	}

	/** @} */
}

namespace sl::tuple::detail
{
	template <class... Tuples>
	[[nodiscard]]
	constexpr auto zip(Tuples&&... tuples)
	{
		return [&]<std::size_t... indices>([[maybe_unused]] std::index_sequence<indices...>)
		{
			return std::make_tuple(
				[&]<std::size_t index>()
				{
					return std::make_tuple(std::get<index>(std::forward<Tuples>(tuples))...);
				}.template operator()<indices>()...
			);
		}(std::make_index_sequence<std::min({std::tuple_size_v<Tuples>...})>{});
	}
}

namespace sl::tuple
{
	/**
	 * \defgroup GROUP_TUPLE_ALGORITHM_ZIP zip
	 * \ingroup GROUP_TUPLE_ALGORITHM
	 * @{
	 */

	/**
	 * \brief Trait type determining the result of a ``zip`` call.
	 */
	template <class... Tuples>
		requires (2 <= sizeof...(Tuples))
				&& (concepts::tuple<std::remove_cvref_t<Tuples>> && ...)
	struct zip_result
	{
		using type = decltype(detail::zip(std::declval<Tuples>()...));
	};

	/**
	 * \brief Alias type determining the result of a ``zip`` call.
	 */
	template <class... Tuples>
		requires (2 <= sizeof...(Tuples))
				&& (concepts::tuple<std::remove_cvref_t<Tuples>> && ...)
	using zip_result_t = typename zip_result<Tuples...>::type;

	/**
	 * \brief Zips elements of all provided source tuples and creates a tuple of tuples.
	 * \tparam First The first tuple type.
	 * \tparam Second The second tuple type.
	 * \tparam Others Other tuple types.
	 * \param first The first tuple.
	 * \param second The second tuple.
	 * \param others Other tuples.
	 * \return A new tuple which elements are tuples.
	 *
	 * \details Combines all given tuples into one tuple with tuples as elements. All resulting tuple element have equal dimensions,
	 * which is the amount of provide source tuples (= ``N```).
	 * The minimal dimension of all given source tuple determines the amount of the resulting tuple elements (= ``M``).
	 * The resulting tuple will then contain ``M`` tuple elements, each consisting of exactly ``N`` elements.
	 * Provided the tuples ``t0 = (e0, e1, ..., em)`` and ``t1 = (f0, f1, ... fr)`` where ``m <= r``, then the zip tuple is built
	 * as follows:
	 * \code{.unparsed}
	 * (	(e0, f0),
	 *		(e1, f1),
	 *		...,
	 *		(em, fm)	)
	 * \endcode
	 *
	 * \note If any of the given tuples are empty, then the resulting tuple will contain no elements.
	 */
	template <class First, class Second, class... Others>
		requires concepts::tuple<std::remove_cvref_t<First>>
				&& concepts::tuple<std::remove_cvref_t<Second>>
				&& (concepts::tuple<std::remove_cvref_t<Others>> && ...)
	[[nodiscard]]
	constexpr zip_result_t<First, Second, Others...> zip(First&& first, Second&& second, Others&&... others)
	{
		return detail::zip(
			std::forward<First>(first),
			std::forward<Second>(second),
			std::forward<Others>(others)...);
	}

	/** @} */
}

namespace sl::tuple::detail
{
	// idea taken from: https://stackoverflow.com/questions/70404549/cartesian-product-of-stdtuple/70405807#70405807
	[[nodiscard]]
	constexpr auto cartesian_product(const concepts::tuple auto& first)
	{
		return envelop_elements(first);
	}

	[[nodiscard]]
	constexpr auto cartesian_product(const concepts::tuple auto& first, const concepts::tuple auto&... others)
	{
		auto trailers = cartesian_product(others...);
		return std::apply(
			[&](auto&... firstElements)
			{
				return std::tuple_cat(
					[&](auto& currentFirst)
					{
						return std::apply(
							[&](auto&... trailerElements)
							{
								return std::make_tuple(std::tuple_cat(std::make_tuple(currentFirst), trailerElements)...);
							},
							trailers
						);
					}(firstElements)...
				);
			},
			first
		);
	}
}

namespace sl::tuple
{
	/**
	 * \defgroup GROUP_TUPLE_ALGORITHM_CARTESIAN_PRODUCT cartesian product
	 * \ingroup GROUP_TUPLE_ALGORITHM
	 * @{
	 */

	/**
	 * \brief Trait type determining the result of a ``cartesian_product`` call.
	 */
	template <class... Tuples>
		requires (2 <= sizeof...(Tuples))
				&& (concepts::tuple<std::remove_cvref_t<Tuples>> && ...)
	struct cartesian_product_result
	{
		using type = decltype(detail::cartesian_product(std::declval<Tuples>()...));
	};

	/**
	 * \brief Alias type determining the result of a ``cartesian_product`` call.
	 */
	template <class... Tuples>
		requires (2 <= sizeof...(Tuples))
				&& (concepts::tuple<std::remove_cvref_t<Tuples>> && ...)
	using cartesian_product_result_t = typename cartesian_product_result<Tuples...>::type;

	/**
	 * \brief Creates the cartesian product of the given tuples.
	 * \tparam First The first tuple type.
	 * \tparam Second The second tuple type.
	 * \tparam Others Other tuple types.
	 * \param first The first tuple.
	 * \param second The second tuple.
	 * \param others Other tuples.
	 * \return A new tuple which elements are tuples.
	 *
	 * \details A cartesian product is built by combining all element of one tuple with all elements of another tuple
	 * (potentially recursive). Provided the tuples ``t0 = (e0, e1, ..., en)`` and ``t1 = (f0, f1, ... fm)`` the cartesian
	 * the product is built as follows:
	 * \code{.unparsed}
	 * (	(e0, f0), (e0, f1), ..., (e0, fm),
	 *		(e1, f0), (e1, f1), ..., (e1, fm),
	 *		...,
	 *		(en, f0), (en, f1), ..., (en, fm)	)
	 * \endcode
	 */
	template <concepts::tuple First, concepts::tuple Second, concepts::tuple... Others>
	[[nodiscard]]
	constexpr cartesian_product_result_t<First, Second, Others...> cartesian_product(
		const First& first,
		const Second& second,
		const Others&... others
	) noexcept(std::is_nothrow_copy_constructible_v<First>
				&& std::is_nothrow_copy_constructible_v<Second>
				&& (std::is_nothrow_copy_constructible_v<Others> && ...))
	{
		return detail::cartesian_product(first, second, others...);
	}

	/** @} */
}

#endif
