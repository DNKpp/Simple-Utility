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

/**
 * \defgroup GROUP_TUPLE_UTILITY tuple utility
 * @{
 */
/** @} */

namespace sl::concepts::detail
{
	template <std::size_t VIndex, class TTuple>
	static constexpr bool check_tuple_index = []
	{
		using std::get;

		return requires{ typename std::tuple_element<VIndex, TTuple>::type; }
			&& requires(TTuple t)
			{
				{ get<VIndex>(t) } -> std::common_with<std::tuple_element_t<VIndex, TTuple>>;
				{ get<VIndex>(std::as_const(t)) } -> std::common_with<std::tuple_element_t<VIndex, TTuple>>;
				{ get<VIndex>(std::move(t)) } -> std::common_with<std::tuple_element_t<VIndex, TTuple>>;
				{ get<VIndex>(static_cast<const TTuple&&>(t)) } -> std::common_with<std::tuple_element_t<VIndex, TTuple>>;
			};
	}();

	template <class TTuple>
	static constexpr bool check_tuple_indices = []<std::size_t... VIndices>([[maybe_unused]] std::index_sequence<VIndices...>)
	{
		return (check_tuple_index<VIndices, TTuple> && ...);
	}(std::make_index_sequence<std::tuple_size_v<TTuple>>{});
}

namespace sl::concepts
{
	/**
	 * \brief Determines whether a type can be used as a tuple-like.
	 * \details Requires the type-traits ``std::tuple_size`` to be defined for the given type and its member ``value`` denoting the the correct tuple size.
	 * The ``std::tuple_element`` trait and the ``get`` function must to be defined for each index in the interval ``[0, N)``, where ``N`` is the tuple size of the given type.
	 * \concept tuple_like
	 * \ingroup GROUP_TUPLE_UTILITY GROUP_UTILITY_CONCEPTS
	 */
	template <class TTuple>
	concept tuple_like = requires
						{
							typename std::tuple_size<TTuple>::type;
							{ std::tuple_size_v<TTuple> } -> std::convertible_to<std::size_t>;
						}
						&& std::cmp_less_equal(0, std::tuple_size_v<TTuple>)
						&& detail::check_tuple_indices<TTuple>;
}

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
	 * \addtogroup GROUP_TUPLE_UTILITY
	 * @{
	 */

	/**
	 * \brief Trait type determining whether the function is invocable with elements of the given tuple.
	 */
	template <class TFunc, class TTuple>
		requires concepts::tuple_like<std::remove_cvref_t<TTuple>>
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
		requires concepts::tuple_like<std::remove_cvref_t<TTuple>>
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
		requires (concepts::tuple_like<std::remove_cvref_t<TTuples>> && ...)
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
		requires concepts::tuple_like<std::remove_cvref_t<TTuple>>
	struct apply_invoke_result
	{
		using type = decltype(std::apply(std::declval<TFunc>(), std::declval<TTuple>()));
	};

	/**
	 * \brief Alias type determining the result of a ``std::apply`` call.
	 */
	template <class TFunc, class TTuple>
	using apply_invoke_result_t = typename apply_invoke_result<TFunc, TTuple>::type;

	/** @} */
}

namespace sl::concepts
{
	/**
	 * \brief Determines whether the function is invocable with the elements of the given tuple.
	 * \ingroup GROUP_TUPLE_UTILITY GROUP_UTILITY_CONCEPTS
	 */
	template <class TFunc, class TTuple>
	concept apply_invocable = is_apply_invocable_v<TFunc, std::remove_reference_t<TTuple>>;

	/**
	 * \brief Determines whether the function is invocable with the elements of the given tuple without throwing.
	 * \ingroup GROUP_TUPLE_UTILITY GROUP_UTILITY_CONCEPTS
	 */
	template <class TFunc, class TTuple>
	concept nothrow_apply_invocable = apply_invocable<TFunc, TTuple>
									&& is_nothrow_apply_invocable_v<TFunc, std::remove_reference_t<TTuple>>;

	/** @} */
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
	template <class... TTuples>
		requires (2 <= sizeof...(TTuples))
			&& (concepts::tuple_like<std::remove_cvref_t<TTuples>> && ...)
	struct tuple_zip_result
	{
		using type = decltype(detail::zip_tuple_element<0>(std::declval<TTuples>()...));
	};

	/**
	 * \brief Alias type determining the result of a ``tuple_zip`` call.
	 */
	template <class... TTuples>
		requires (2 <= sizeof...(TTuples))
			&& (concepts::tuple_like<std::remove_cvref_t<TTuples>> && ...)
	using tuple_zip_result_t = typename tuple_zip_result<TTuples...>::type;

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

namespace sl::detail
{
	// idea taken from: https://stackoverflow.com/questions/70404549/cartesian-product-of-stdtuple/70405807#70405807
	constexpr auto tuple_cartesian_product(const concepts::tuple_like auto& first)
	{
		return std::apply(
			[](auto&... elements) { return std::make_tuple(std::make_tuple(elements)...); },
			first
		);
	}

	constexpr auto tuple_cartesian_product(const concepts::tuple_like auto& first, const concepts::tuple_like auto&... others)
	{
		auto trailers = tuple_cartesian_product(others...);
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

namespace sl
{
	/**
	 * \addtogroup GROUP_TUPLE_UTILITY
	 * @{
	 */

	/**
	 * \brief Trait type determining the result of a ``tuple_cartesian_product`` call.
	 */
	template <class... TTuples>
		requires (2 <= sizeof...(TTuples))
			&& (concepts::tuple_like<std::remove_cvref_t<TTuples>> && ...)
	struct tuple_cartesian_product_result
	{
		using type = decltype(detail::tuple_cartesian_product(std::declval<TTuples>()...));
	};

	/**
	 * \brief Alias type determining the result of a ``tuple_cartesian_product`` call.
	 */
	template <class... TTuples>
		requires (2 <= sizeof...(TTuples))
			&& (concepts::tuple_like<std::remove_cvref_t<TTuples>> && ...)
	using tuple_cartesian_product_result_t = typename tuple_cartesian_product_result<TTuples...>::type;

	template <concepts::tuple_like TFirst, concepts::tuple_like TSecond, concepts::tuple_like... TOthers>
	[[nodiscard]]
	constexpr tuple_cartesian_product_result_t<TFirst, TSecond, TOthers...> tuple_cartesian_product(
		const TFirst& first,
		const TSecond& second,
		const TOthers&... others
	)
	{
		return detail::tuple_cartesian_product(first, second, others...);
	}

	/** @} */
}

#endif
