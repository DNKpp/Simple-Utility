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

namespace sl::tuple
{
	/**
	 * \defgroup GROUP_TUPLE tuple
	 * @{
	 */
	/** @} */
}

namespace sl::concepts::detail
{
	template <std::size_t VIndex, class TTuple>
	static constexpr bool check_tuple_index = []
	{
		using std::get;

		// ReSharper disable once CppUseTypeTraitAlias
		return requires { typename std::tuple_element<VIndex, TTuple>::type; }
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
	 * \ingroup GROUP_TUPLE GROUP_UTILITY_CONCEPTS
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

namespace sl::tuple::detail
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

namespace sl::tuple
{
	/**
	 * \addtogroup GROUP_TUPLE
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
	 * \ingroup GROUP_TUPLE GROUP_UTILITY_CONCEPTS
	 */
	template <class TFunc, class TTuple>
	concept apply_invocable = tuple::is_apply_invocable_v<TFunc, std::remove_reference_t<TTuple>>;

	/**
	 * \brief Determines whether the function is invocable with the elements of the given tuple without throwing.
	 * \ingroup GROUP_TUPLE GROUP_UTILITY_CONCEPTS
	 */
	template <class TFunc, class TTuple>
	concept nothrow_apply_invocable = apply_invocable<TFunc, TTuple>
									&& tuple::is_nothrow_apply_invocable_v<TFunc, std::remove_reference_t<TTuple>>;
}

#endif
