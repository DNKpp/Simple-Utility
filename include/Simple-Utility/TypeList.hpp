//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_TYPE_LIST_HPP
#define SL_UTILITY_TYPE_LIST_HPP

#pragma once

#include <concepts>
#include <type_traits>
#include <utility>

namespace sl::type_list
{
	template <class... Types>
	struct TypeList
	{
	};
}

namespace sl::type_list::detail
{
	template <std::size_t index, class... Types>
	struct element;

	template <std::size_t index, class First, class... Others>
	struct element<index, TypeList<First, Others...>>
		: element<index - 1u, TypeList<Others...>>
	{
	};

	template <class First, class... Types>
	struct element<0, TypeList<First, Types...>>
	{
		using type = First;
	};
}

template <class... Types>
struct std::tuple_size<sl::type_list::TypeList<Types...>> // NOLINT(cert-dcl58-cpp)
	: public std::integral_constant<std::size_t, sizeof...(Types)>
{
};

template <std::size_t index, class... Types>
struct std::tuple_element<index, sl::type_list::TypeList<Types...>> // NOLINT(cert-dcl58-cpp)
	: public sl::type_list::detail::element<index, sl::type_list::TypeList<Types...>>
{
};

namespace sl::type_list::detail
{
	template <class List, std::size_t index>
	// ReSharper disable once CppUseTypeTraitAlias
	constexpr bool validIndex = requires { typename std::tuple_element<index, List>::type; };

	template <class List>
	constexpr bool check_indices = []<std::size_t... indices>([[maybe_unused]] std::index_sequence<indices...>)
	{
		return (validIndex<List, indices> && ...);
	}(std::make_index_sequence<std::tuple_size_v<List>>{});
}

namespace sl::concepts
{
	template <class T>
	concept type_list_like = requires
							{
								typename std::tuple_size<T>::type;
								{ std::tuple_size_v<T> } -> std::convertible_to<std::size_t>;
							}
							&& std::cmp_less_equal(0, std::tuple_size_v<T>)
							&& type_list::detail::check_indices<T>;
}

namespace sl::type_list
{
	template <template <class...> class TargetContainer, concepts::type_list_like List>
	struct populate_from;

	template <template <class...> class TargetContainer, class... Elements, template <class...> class SourceContainer>
	struct populate_from<TargetContainer, SourceContainer<Elements...>>
	{
		using type = TargetContainer<Elements...>;
	};

	template <template <class...> class TargetContainer, concepts::type_list_like List>
	using populated_from_t = typename populate_from<TargetContainer, List>::type;
}

#endif
