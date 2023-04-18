//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_TYPE_LIST_HPP
#define SL_UTILITY_TYPE_LIST_HPP

#pragma once

#include <concepts>
#include <tuple>
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
	template <class Query, concepts::type_list_like List>
	struct index_of;

	template <class Query, template <class...> class Container, class... Others>
	struct index_of<Query, Container<Query, Others...>>
		: public std::integral_constant<std::size_t, 0>
	{
	};

	template <class Query, template <class...> class Container, class First, class... Others>
	struct index_of<Query, Container<First, Others...>>
		: public std::integral_constant<
			std::size_t,
			1u + index_of<Query, Container<Others...>>::value>
	{
	};

	template <class Query, concepts::type_list_like List>
	inline constexpr std::size_t index_of_v = index_of<Query, List>::value;

	template <class Query, concepts::type_list_like List>
	struct contained_by;

	template <class Query, template <class...> class Container, class... Elements>
	struct contained_by<Query, Container<Elements...>>
		: public std::disjunction<std::is_same<Query, Elements>...>
	{
	};

	template <class Query, concepts::type_list_like List>
	inline constexpr bool contained_by_v = contained_by<Query, List>::value;

	template <template <class> class UnaryOperation, concepts::type_list_like List>
	struct transform;

	template <template <class> class UnaryOperation, template <class...> class Container, class... Elements>
	struct transform<UnaryOperation, Container<Elements...>>
	{
		using type = Container<typename UnaryOperation<Elements>::type...>;
	};

	template <template <class> class UnaryOperation, concepts::type_list_like List>
	using transform_t = typename transform<UnaryOperation, List>::type;

	template <template <class...> class TargetContainer, concepts::type_list_like List>
	struct populated_from;

	template <template <class...> class TargetContainer, class... Elements, template <class...> class SourceContainer>
	struct populated_from<TargetContainer, SourceContainer<Elements...>>
	{
		using type = TargetContainer<Elements...>;
	};

	template <template <class...> class TargetContainer, concepts::type_list_like List>
	using populated_from_t = typename populated_from<TargetContainer, List>::type;

	template <concepts::type_list_like List>
	struct tail;

	template <template <class...> class Container>
	struct tail<Container<>>
	{
		using type = Container<>;
	};

	template <template <class...> class Container, class First, class... Others>
	struct tail<Container<First, Others...>>
	{
		using type = Container<Others...>;
	};

	template <concepts::type_list_like List>
	using tail_t = typename tail<List>::type;

	template <concepts::type_list_like List>
	struct front;

	template <template <class...> class Container, class First, class... Others>
	struct front<Container<First, Others...>>
	{
		using type = First;
	};

	template <concepts::type_list_like List>
	using front_t = typename front<List>::type;

	template <concepts::type_list_like List>
	struct back;

	template <template <class...> class Container, class First, class... Others>
	struct back<Container<First, Others...>>
		: public back<Container<Others...>>
	{
	};

	template <template <class...> class Container, class Element>
	struct back<Container<Element>>
	{
		using type = Element;
	};

	template <concepts::type_list_like List>
	using back_t = typename back<List>::type;

	template <concepts::type_list_like... Lists>
	struct concat;

	template <concepts::type_list_like List>
	struct concat<List>
	{
		using type = List;
	};

	template <
		template <class...> class LhsContainer,
		class... LhsElements,
		template <class...> class RhsContainer,
		class... RhsElements,
		concepts::type_list_like... Others>
	struct concat<LhsContainer<LhsElements...>, RhsContainer<RhsElements...>, Others...>
		: public concat<LhsContainer<LhsElements..., RhsElements...>, Others...>
	{
	};

	template <concepts::type_list_like... Lists>
	using concat_t = typename concat<Lists...>::type;
}

#endif
