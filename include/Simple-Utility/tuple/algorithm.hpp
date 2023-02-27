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
	 */
	template <class TTuple>
		requires concepts::tuple_like<std::remove_cvref_t<TTuple>>
	constexpr envelop_elements_result_t<TTuple> envelop_elements(TTuple&& tuple)
	{
		return detail::envelop_elements(std::forward<TTuple>(tuple));
	}

	/** @} */
}

namespace sl::tuple::detail
{
	template <class... TTuples>
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
	constexpr auto cartesian_product(const concepts::tuple_like auto& first)
	{
		return envelop_elements(first);
	}

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

	template <concepts::tuple_like TFirst, concepts::tuple_like TSecond, concepts::tuple_like... TOthers>
	[[nodiscard]]
	constexpr cartesian_product_result_t<TFirst, TSecond, TOthers...> cartesian_product(
		const TFirst& first,
		const TSecond& second,
		const TOthers&... others
	)
	{
		return detail::cartesian_product(first, second, others...);
	}

	/** @} */
}

#endif
