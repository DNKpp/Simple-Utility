//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_TYPE_LIST_HPP
#define SL_UTILITY_TYPE_LIST_HPP

#pragma once

#include <algorithm>
#include <concepts>
#include <tuple>
#include <type_traits>
#include <utility>

namespace sl::type_list
{
	/**
	 * \defgroup GROUP_TYPE_LIST type-list
	 * \{
	 */

	/**
	 * \brief A variadic template type, which's only serves as a container for an arbitrary amount of types.
	 * Neither of the given types will be actually instantiated.
	 * \tparam Types The provided elements.
	 */
	template <class... Types>
	struct TypeList
	{
	};

	/**
	 * \}
	 */
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

/**
 * \brief Specialization of ``std::tuple_size`` for \ref sl::type_list::TypeList "TypeList".
 * \ingroup GROUP_TYPE_LIST
 * \see https://en.cppreference.com/w/cpp/utility/tuple_size
 */
template <class... Types>
struct std::tuple_size<sl::type_list::TypeList<Types...>> // NOLINT(cert-dcl58-cpp)
	: public std::integral_constant<std::size_t, sizeof...(Types)>
{
};

/**
 * \brief Specialization of ``std::tuple_element`` for \ref sl::type_list::TypeList "TypeList".
 * \ingroup GROUP_TYPE_LIST
 * \see https://en.cppreference.com/w/cpp/utility/tuple_element
 */
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
	constexpr bool checkIndices = []<std::size_t... indices>([[maybe_unused]] std::index_sequence<indices...>)
	{
		return (validIndex<List, indices> && ...);
	}(std::make_index_sequence<std::tuple_size_v<List>>{});
}

namespace sl::concepts
{
	/**
	 * \brief Determines whether a type satisfies the requirements of a type-list.
	 * \details Requires the traits ``std::tuple_size`` to be specialized for the given type and its member ``value`` denoting the the correct tuple size.
	 * The ``std::tuple_element`` trait must be defined for each index in the interval ``[0, N)``, where ``N`` is the size of the given type-list.
	 * \concept type_list_like
	 * \ingroup GROUP_TYPE_LIST GROUP_UTILITY_CONCEPTS
	 * \see https://en.cppreference.com/w/cpp/utility/tuple_element
	 * \see https://en.cppreference.com/w/cpp/utility/tuple_size
	 */
	template <class T>
	concept type_list_like = requires
							{
								typename std::tuple_size<T>::type;
								{ std::tuple_size_v<T> } -> std::convertible_to<std::size_t>;
							}
							&& std::cmp_less_equal(0, std::tuple_size_v<T>)
							&& type_list::detail::checkIndices<T>;
}

namespace sl::type_list
{
	/**
	 * \defgroup GROUP_TYPE_LIST_COMMON_CONTAINER common_container
	 * \ingroup GROUP_TYPE_LIST
	 * \brief Trait determining whether some type-lists have the container type in common.
	 * \details The container is the actual template of a type-list. For example, given the type-list ``A<B, C>``
	 * ``A`` denotes the container. Two (or more) type-lists have a common container, if their containers are exactly the same,
	 * thus no conversions will be applied. If only one type-list is provided, its container is used.
	 *
	 * If no such common container can be determined, the member alias ``type`` will be omitted.
	 * \{
	 */

	/**
	 * \brief Primary template isn't defined on purpose.
	 * \tparam Lists The provided type-lists.
	 */
	template <concepts::type_list_like... Lists>
	struct common_container;

	/**
	 * \brief Unary trait defining the ``type`` member-alias.
	 * \tparam Container The container template.
	 * \tparam Elements The elements of the type-list.
	 */
	template <template <class...> class Container, class... Elements>
	struct common_container<Container<Elements...>>
	{
		template <class... Ts>
		using type = Container<Ts...>;
	};

	/**
	 * \brief Trait reducing the amount of participating type-lists, if the first two have the container in common.
	 * \tparam Container The current common container template.
	 * \tparam LhsElements The elements of the first given type-list.
	 * \tparam RhsElements The elements of the second given type-list.
	 * \tparam Others All other type-lists, not yet investigated.
	 */
	template <
		template <class...> class Container,
		class... LhsElements,
		class... RhsElements,
		concepts::type_list_like... Others>
	struct common_container<Container<LhsElements...>, Container<RhsElements...>, Others...>
		: public common_container<Container<LhsElements...>, Others...>
	{
	};

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_LIST_INDEX_OF index_of
	 * \ingroup GROUP_TYPE_LIST
	 * \brief Queries the given type-list for a specific type.
	 * \details If the query type is contained in the given type-list, the ``value`` member contains the index
	 * at which the query type appears inside the type-list. If it's not contained, no such member is defined.
	 * \note The query type may appear multiple times, but the algorithm finds only returns the index of the
	 * first one found (the least index).
	 * \{
	 */

	/**
	 * \brief Primary template isn't defined on purpose.
	 * \tparam Query The type to be queried for.
	 * \tparam List The provided type-list.
	 */
	template <class Query, concepts::type_list_like List>
	struct index_of;

	/**
	 * \brief Specialization if the queried type appears at the beginning of the type-list.
	 * \tparam Query The type to be queried for.
	 * \tparam Container The container type.
	 * \tparam Others All other type-lists elements, not yet investigated.
	 */
	template <class Query, template <class...> class Container, class... Others>
	struct index_of<Query, Container<Query, Others...>>
		: public std::integral_constant<std::size_t, 0>
	{
	};

	/**
	 * \brief Specialization if the queried type doesn't appear at the beginning of the type-list.
	 * \tparam Query The type to be queried for.
	 * \tparam Container The container type.
	 * \tparam First The current first element.
	 * \tparam Others All other type-lists elements, not yet investigated.
	 */
	template <class Query, template <class...> class Container, class First, class... Others>
	struct index_of<Query, Container<First, Others...>>
		: public std::integral_constant<
			std::size_t,
			1u + index_of<Query, Container<Others...>>::value>
	{
	};

	/**
	 * \brief Convenience constant, exposing the ``value`` member of the \ref sl::type_list::index_of "index_of" trait.
	 * \tparam Query The type to be queried for.
	 * \tparam List The provided type-list.
	 */
	template <class Query, concepts::type_list_like List>
	inline constexpr std::size_t index_of_v = index_of<Query, List>::value;

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_LIST_CONTAINED_BY contained_by
	 * \ingroup GROUP_TYPE_LIST
	 * \{
	 */

	/**
	 * \brief Primary template isn't defined on purpose.
	 * \tparam Query The type to be queried for.
	 * \tparam List The provided type-list.
	 */
	template <class Query, concepts::type_list_like List>
	struct contained_by;

	/**
	 * \brief Specialization determining the presence of the query type, due to comparing it with each element of the type-list.
	 * \tparam Query The type to be queried for.
	 * \tparam Container The container type.
	 * \tparam Elements The element types.
	 */
	template <class Query, template <class...> class Container, class... Elements>
	struct contained_by<Query, Container<Elements...>>
		: public std::bool_constant<(std::same_as<Query, Elements> || ...)>
	{
	};

	/**
	 * \brief Convenience constant, exposing the ``value`` member of the \ref sl::type_list::contained_by "contained_by" trait.
	 * \tparam Query The type to be queried for.
	 * \tparam List The provided type-list.
	 */
	template <class Query, concepts::type_list_like List>
	inline constexpr bool contained_by_v = contained_by<Query, List>::value;

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_LIST_TRANSFORM transform
	 * \ingroup GROUP_TYPE_LIST
	 * \brief Applies the given unary operation on each element of the source type-list and stores them into a new type-list.
	 * \details The unary operation must accept a template argument and store its result in a member alias named ``type``.
	 * For example: \snippet TypeList.cpp transformation definition
	 * \{
	 */

	/**
	 * \brief Primary template isn't defined on purpose.
	 * \tparam UnaryOperation The operation to be applied on each element.
	 * \tparam List The provided type-list.
	 */
	template <template <class> class UnaryOperation, concepts::type_list_like List>
	struct transform;

	/**
	 * \brief Specialization, which applies the UnaryOperation on each element of the given type-list.
	 * \tparam UnaryOperation The operation to be applied on each element.
	 * \tparam Container The container type.
	 * \tparam Elements The element types.
	 */
	template <template <class> class UnaryOperation, template <class...> class Container, class... Elements>
	struct transform<UnaryOperation, Container<Elements...>>
	{
		using type = Container<typename UnaryOperation<Elements>::type...>;
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::type_list::transform "transform" trait.
	 * \tparam UnaryOperation The operation to be applied on each element.
	 * \tparam List The provided type-list.
	 */
	template <template <class> class UnaryOperation, concepts::type_list_like List>
	using transform_t = typename transform<UnaryOperation, List>::type;

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_LIST_POPULATED_FROM populated_from
	 * \ingroup GROUP_TYPE_LIST
	 * \brief Applies the source type-list-elements as template arguments of the TargetContainer.
	 * \{
	 */

	/**
	 * \brief Primary template isn't defined on purpose.
	 * \tparam TargetContainer The target container template.
	 * \tparam List The provided type-list.
	 */
	template <template <class...> class TargetContainer, concepts::type_list_like List>
	struct populated_from;

	/**
	 * \brief Specialization applying each element of the source container as template argument of the target container.
	 * \tparam TargetContainer The target container template
	 * \tparam Elements The provided elements.
	 * \tparam SourceContainer The source container template.
	 */
	template <template <class...> class TargetContainer, class... Elements, template <class...> class SourceContainer>
	struct populated_from<TargetContainer, SourceContainer<Elements...>>
	{
		using type = TargetContainer<Elements...>;
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::type_list::populated_from "populated_from" trait.
	 * \tparam TargetContainer The target container template.
	 * \tparam List The provided type-list.
	 */
	template <template <class...> class TargetContainer, concepts::type_list_like List>
	using populated_from_t = typename populated_from<TargetContainer, List>::type;

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_LIST_TAIL tail
	 * \ingroup GROUP_TYPE_LIST
	 * \brief Skips the first element of the given type-list and defines a ``type`` member-alias containing the rest of the elements.
	 * \note If the provided type-list contains no elements, the result is an empty type-list, too.
	 * \{
	 */

	/**
	 * \brief Primary template isn't defined on purpose.
	 * \tparam List The provided type-list.
	 */
	template <concepts::type_list_like List>
	struct tail;

	/**
	 * \brief Specialization for empty source type-lists.
	 * \tparam Container The target container template.
	 */
	template <template <class...> class Container>
	struct tail<Container<>>
	{
		using type = Container<>;
	};

	/**
	 * \brief Specialization for non-empty source type-lists.
	 * \tparam Container The target container template.
	 * \tparam First The element to be omitted.
	 * \tparam Others The rest of the elements.
	 */
	template <template <class...> class Container, class First, class... Others>
	struct tail<Container<First, Others...>>
	{
		using type = Container<Others...>;
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::type_list::tail "tail" trait.
	 * \tparam List The provided type-list.
	 */
	template <concepts::type_list_like List>
	using tail_t = typename tail<List>::type;

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_LIST_FRONT front
	 * \ingroup GROUP_TYPE_LIST
	 * \brief Defines the ``type`` member-alias containing the first element of the given type-list.
	 * \{
	 */

	/**
	 * \brief Primary template isn't defined on purpose.
	 * \tparam List The provided type-list.
	 */
	template <concepts::type_list_like List>
	struct front;

	/**
	 * \brief Specialization retrieving the first element of the source type-list.
	 * \tparam Container The target container template.
	 * \tparam First The element.
	 * \tparam Others The elements to be omitted.
	 */
	template <template <class...> class Container, class First, class... Others>
	struct front<Container<First, Others...>>
	{
		using type = First;
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::type_list::front "front" trait.
	 * \tparam List The provided type-list.
	 */
	template <concepts::type_list_like List>
	using front_t = typename front<List>::type;

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_LIST_BACK back
	 * \ingroup GROUP_TYPE_LIST
	 * \brief Defines the ``type`` member-alias containing the last element of the given type-list.
	 * \{
	 */

	/**
	 * \brief Primary template isn't defined on purpose.
	 * \tparam List The provided type-list.
	 */
	template <concepts::type_list_like List>
	struct back;

	/**
	 * \brief Specialization investigating the current element of the source type-list.
	 * \tparam Container The target container template.
	 * \tparam First The current element to be omitted.
	 * \tparam Others The elements to be further investigated.
	 */
	template <template <class...> class Container, class First, class... Others>
	struct back<Container<First, Others...>>
		: public back<Container<Others...>>
	{
	};

	/**
	 * \brief Specialization retrieving the last element of the source type-list. 
	 * \tparam Container The target container template.
	 * \tparam Element The last element of the source list.
	 */
	template <template <class...> class Container, class Element>
	struct back<Container<Element>>
	{
		using type = Element;
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::type_list::back "back" trait.
	 * \tparam List The provided type-list.
	 */
	template <concepts::type_list_like List>
	using back_t = typename back<List>::type;

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_LIST_CONCAT concat
	 * \ingroup GROUP_TYPE_LIST
	 * \brief Appends at the end of the first type-list all elements of the other type-lists. 
	 * \{
	 */

	/**
	 * \brief Primary template isn't defined on purpose.
	 * \tparam Lists The provided type-lists.
	 */
	template <concepts::type_list_like... Lists>
	struct concat;

	/**
	 * \brief Unary specialization, simply using the given type-list as result.
	 * \tparam List The provided type-list.
	 */
	template <concepts::type_list_like List>
	struct concat<List>
	{
		using type = List;
	};

	/**
	 * \brief Specialization appending all elements of the second type-list at the back of the first type-list and
	 * repeating it for any other not yet investigated type-lists.
	 * \tparam LhsContainer The container of the first type-list.
	 * \tparam LhsElements The elements of the first type-list.
	 * \tparam RhsContainer The container of the second type-list.
	 * \tparam RhsElements The elements of the second type-list.
	 * \tparam Others  All other type-lists, not yet investigated.
	 */
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

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::type_list::concat "concat" trait.
	 * \tparam Lists The provided type-lists.
	 */
	template <concepts::type_list_like... Lists>
	using concat_t = typename concat<Lists...>::type;

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_LIST_ZIP_ELEMENT zip_element
	 * \ingroup GROUP_TYPE_LIST
	 * \{
	 */

	template <template <class...> class TargetContainer, std::size_t index, concepts::type_list_like... Lists>
		requires (sizeof...(Lists) == 0)
				|| (index < std::min({std::tuple_size_v<Lists>...}))
	struct zip_elements_as
	{
		using type = TargetContainer<std::tuple_element_t<index, Lists>...>;
	};

	template <template <class...> class TargetContainer, std::size_t index, concepts::type_list_like... Lists>
	using zip_elements_as_t = typename zip_elements_as<TargetContainer, index, Lists...>::type;

	template <std::size_t index, concepts::type_list_like... Lists>
	using zip_elements = zip_elements_as<common_container<Lists...>::template type, index, Lists...>;

	template <std::size_t index, concepts::type_list_like... Lists>
	using zip_elements_t = typename zip_elements<index, Lists...>::type;

	/**
	 * \}
	 */
}

namespace sl::type_list::detail
{
	template <class, template <class...> class TargetContainer, concepts::type_list_like... Lists>
	struct zip_as;

	template <std::size_t... indices, template <class...> class TargetContainer, concepts::type_list_like... Lists>
	struct zip_as<std::index_sequence<indices...>, TargetContainer, Lists...>
	{
		using type = TargetContainer<zip_elements_as_t<TargetContainer, indices, Lists...>...>;
	};

	template <template <class...> class TargetContainer, concepts::type_list_like... Lists>
	using zip_as_t = typename zip_as<
		// handles zero Lists case correctly
		std::make_index_sequence<std::min({std::max<std::size_t>({0u, std::tuple_size_v<Lists>...}), std::tuple_size_v<Lists>...})>,
		TargetContainer,
		Lists...
	>::type;
}

namespace sl::type_list
{
	/**
	 * \defgroup GROUP_TYPE_LIST_ZIP zip
	 * \ingroup GROUP_TYPE_LIST
	 * \{
	 */

	template <template <class...> class TargetContainer, concepts::type_list_like... Lists>
	struct zip_as
	{
		using type = detail::zip_as_t<TargetContainer, Lists...>;
	};

	template <template <class...> class TargetContainer, concepts::type_list_like... Lists>
	using zip_as_t = typename zip_as<TargetContainer, Lists...>::type;

	template <concepts::type_list_like... Lists>
	using zip = zip_as<common_container<Lists...>::template type, Lists...>;

	template <concepts::type_list_like... Lists>
	using zip_t = typename zip<Lists...>::type;

	/**
	 * \}
	 */
}

#endif
