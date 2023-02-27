//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_TUPLE_ALGORITHM_HPP
#define SL_UTILITY_TUPLE_ALGORITHM_HPP

#pragma once

#include <algorithm>

#include "Simple-Utility/tuple/general.hpp"

namespace sl::tuple::detail
{
	template <class TTuple>
		requires concepts::tuple_like<std::remove_cvref_t<TTuple>>
	[[nodiscard]]
	constexpr auto envelop_elements(TTuple&& tuple)
	{
		return std::apply(
			[]<class... TElement>(TElement&&... element)
			{
				return std::make_tuple(
					std::make_tuple(std::forward<TElement>(element))...
				);
			},
			std::forward<TTuple>(tuple)
		);
	}
}

namespace sl::tuple
{
	/**
	 * \addtogroup GROUP_TUPLE
	 * @{
	 */

	/**
	 * \defgroup GROUP_TUPLE_ALGORITHM tuple algorithm
	 * @{
	 */
	/** @} */

	/**
	 * \brief Trait type determining the result of a ``tuple_envelop_elements`` call.
	 */
	template <class TTuple>
		requires concepts::tuple_like<std::remove_cvref_t<TTuple>>
	struct envelop_elements_result
	{
		using type = decltype(detail::envelop_elements(std::declval<TTuple>()));
	};

	/**
	 * \brief Alias type determining the result of a ``tuple_envelop_elements`` call.
	 */
	template <class TTuple>
		requires concepts::tuple_like<std::remove_cvref_t<TTuple>>
	using envelop_elements_result_t = typename envelop_elements_result<TTuple>::type;

	/**
	 * \brief Envelops all elements of the given tuple into their own ``std::tuple`` and creates a tuple of tuples.
	 * \tparam TTuple The tuple type.
	 * \param tuple The tuple.
	 * \return A new tuple which elements are tuples.
	 *
	 * \details Provided a tuple ``t0 = (e0, e1, ..., en)`` then the resulting tuple is built as follows:
	 * \code{.unparsed}
	 * (	(e0), (e1), ..., (en)	)
	 * \endcode
	 *
	 * \ingroup GROUP_TUPLE_ALGORITHM
	 */
	template <class TTuple>
		requires concepts::tuple_like<std::remove_cvref_t<TTuple>>
	[[nodiscard]]
	constexpr envelop_elements_result_t<TTuple> envelop_elements(
		TTuple&& tuple
	) noexcept(std::is_nothrow_constructible_v<std::remove_cvref_t<TTuple>, TTuple>)
	{
		return detail::envelop_elements(std::forward<TTuple>(tuple));
	}

	/** @} */
}

namespace sl::tuple::detail
{
	template <class... TTuples>
	[[nodiscard]]
	constexpr auto zip(TTuples&&... tuples)
	{
		return [&]<std::size_t... VIndices>([[maybe_unused]] std::index_sequence<VIndices...>)
		{
			return std::make_tuple(
				[&]<std::size_t VIndex>()
				{
					return std::make_tuple(std::get<VIndex>(std::forward<TTuples>(tuples))...);
				}.template operator()<VIndices>()...
			);
		}(std::make_index_sequence<std::min({std::tuple_size_v<TTuples>...})>{});
	}
}

namespace sl::tuple
{
	/**
	 * \addtogroup GROUP_TUPLE
	 * @{
	 */

	/**
	 * \brief Trait type determining the result of a ``tuple_zip`` call.
	 */
	template <class... TTuples>
		requires (2 <= sizeof...(TTuples))
				&& (concepts::tuple_like<std::remove_cvref_t<TTuples>> && ...)
	struct zip_result
	{
		using type = decltype(detail::zip(std::declval<TTuples>()...));
	};

	/**
	 * \brief Alias type determining the result of a ``tuple_zip`` call.
	 */
	template <class... TTuples>
		requires (2 <= sizeof...(TTuples))
				&& (concepts::tuple_like<std::remove_cvref_t<TTuples>> && ...)
	using zip_result_t = typename zip_result<TTuples...>::type;

	/**
	 * \brief Zips elements of all provided source tuples and creates a tuple of tuples.
	 * \tparam TFirst The first tuple type.
	 * \tparam TSecond The second tuple type.
	 * \tparam TOthers Other tuple types.
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
	 *
	 * \ingroup GROUP_TUPLE_ALGORITHM
	 */
	template <class TFirst, class TSecond, class... TOthers>
		requires concepts::tuple_like<std::remove_cvref_t<TFirst>>
				&& concepts::tuple_like<std::remove_cvref_t<TSecond>>
				&& (concepts::tuple_like<std::remove_cvref_t<TOthers>> && ...)
	[[nodiscard]]
	constexpr zip_result_t<TFirst, TSecond, TOthers...> zip(TFirst&& first, TSecond&& second, TOthers&&... others)
	{
		return detail::zip(
			std::forward<TFirst>(first),
			std::forward<TSecond>(second),
			std::forward<TOthers>(others)...
		);
	}

	/** @} */
}

namespace sl::tuple::detail
{
	// idea taken from: https://stackoverflow.com/questions/70404549/cartesian-product-of-stdtuple/70405807#70405807
	[[nodiscard]]
	constexpr auto cartesian_product(const concepts::tuple_like auto& first)
	{
		return envelop_elements(first);
	}

	[[nodiscard]]
	constexpr auto cartesian_product(const concepts::tuple_like auto& first, const concepts::tuple_like auto&... others)
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
	 * \addtogroup GROUP_TUPLE
	 * @{
	 */

	/**
	 * \brief Trait type determining the result of a ``cartesian_product`` call.
	 */
	template <class... TTuples>
		requires (2 <= sizeof...(TTuples))
				&& (concepts::tuple_like<std::remove_cvref_t<TTuples>> && ...)
	struct cartesian_product_result
	{
		using type = decltype(detail::cartesian_product(std::declval<TTuples>()...));
	};

	/**
	 * \brief Alias type determining the result of a ``cartesian_product`` call.
	 */
	template <class... TTuples>
		requires (2 <= sizeof...(TTuples))
				&& (concepts::tuple_like<std::remove_cvref_t<TTuples>> && ...)
	using cartesian_product_result_t = typename cartesian_product_result<TTuples...>::type;

	/**
	 * \brief Creates the cartesian product of the given tuples.
	 * \tparam TFirst The first tuple type.
	 * \tparam TSecond The second tuple type.
	 * \tparam TOthers Other tuple types.
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
	 *
	 * \ingroup GROUP_TUPLE_ALGORITHM
	 */
	template <concepts::tuple_like TFirst, concepts::tuple_like TSecond, concepts::tuple_like... TOthers>
	[[nodiscard]]
	constexpr cartesian_product_result_t<TFirst, TSecond, TOthers...> cartesian_product(
		const TFirst& first,
		const TSecond& second,
		const TOthers&... others
	) noexcept(std::is_nothrow_copy_constructible_v<TFirst>
				&& std::is_nothrow_copy_constructible_v<TSecond>
				&& (std::is_nothrow_copy_constructible_v<TOthers> && ...))
	{
		return detail::cartesian_product(first, second, others...);
	}

	/** @} */
}

#endif
