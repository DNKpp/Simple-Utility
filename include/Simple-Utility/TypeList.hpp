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
							&& 0u <= std::tuple_size_v<T>
							&& type_list::detail::checkIndices<T>;

	/**
	 * \brief Determines whether a type is a type-list and contains elements.
	 * \ingroup GROUP_TYPE_LIST GROUP_UTILITY_CONCEPTS
	 */
	template <class T>
	concept populated_type_list = type_list_like<T>
							&& 0u < std::tuple_size_v<T>;
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
	 * \defgroup GROUP_TYPE_LIST_COUNT count
	 * \ingroup GROUP_TYPE_LIST
	 * \brief Determines how often the query type appears in the source type-list.
	 * \{
	 */

	/**
	 * \brief Primary template isn't defined on purpose.
	 * \tparam Query The type to be queried for.
	 * \tparam List The provided type-list.
	 */
	template <class Query, concepts::type_list_like List>
	struct count;

	/**
	 * \brief Specialization counting the presence of the query type, due to comparing it with each element of the type-list.
	 * \tparam Query The type to be queried for.
	 * \tparam Container The container type.
	 * \tparam Elements The element types.
	 */
	template <class Query, template <class...> class Container, class... Elements>
	struct count<Query, Container<Elements...>>
		: public std::integral_constant<
			std::size_t,
			(0u + ... + static_cast<std::size_t>(std::same_as<Query, Elements>))>
	{
	};

	/**
	 * \brief Convenience constant, exposing the ``value`` member of the \ref sl::type_list::count "count" trait.
	 * \tparam Query The type to be queried for.
	 * \tparam List The provided type-list.
	 */
	template <class Query, concepts::type_list_like List>
	inline constexpr auto count_v = count<Query, List>::value;

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_LIST_EQUAL equal
	 * \ingroup GROUP_TYPE_LIST
	 * \brief Determines whether the source type-lists contain the same elements in the same order.
	 * \{
	 */

	/**
	 * \brief Primary template yields ``false``.
	 * \tparam First The first type-list.
	 * \tparam Others Other type-lists.
	 */
	template <concepts::type_list_like First, concepts::type_list_like... Others>
	struct equal
		: public std::false_type
	{
	};

	/**
	 * \brief Specialization for single type-list.
	 * \tparam First The first type-list.
	 */
	template <concepts::type_list_like First>
	struct equal<First>
		: public std::true_type
	{
	};

	/**
	 * \brief Specialization, determining whether the first two type-lists have equal template arguments.
	 * \tparam FirstContainer The first container template.
	 * \tparam SecondContainer The second container template.
	 * \tparam Elements The elements contained in the first and second container.
	 * \tparam Others Other type-lists.
	 */
	template <
		template <class...> class FirstContainer,
		template <class...> class SecondContainer,
		class... Elements,
		concepts::type_list_like... Others>
	struct equal<FirstContainer<Elements...>, SecondContainer<Elements...>, Others...>
		: public equal<FirstContainer<Elements...>, Others...>
	{
	};

	/**
	 * \brief Convenience constant, exposing the ``value`` member of the \ref sl::type_list::equal "equal" trait.
	 * \tparam First The first type-list.
	 * \tparam Others Other type-lists to be compared to.
	 */
	template <concepts::type_list_like First, concepts::type_list_like... Others>
	inline constexpr bool equal_v = equal<First, Others...>::value;

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
	 * \defgroup GROUP_TYPE_LIST_FRONT front
	 * \ingroup GROUP_TYPE_LIST
	 * \brief Defines the ``type`` member-alias containing the first element of the given type-list.
	 * \{
	 */

	/**
	 * \brief Primary template isn't defined on purpose.
	 * \tparam List The provided type-list.
	 */
	template <concepts::populated_type_list List>
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
	template <concepts::populated_type_list List>
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
	template <concepts::populated_type_list List>
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
	template <concepts::populated_type_list List>
	using back_t = typename back<List>::type;

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_LIST_CONCAT concat
	 * \ingroup GROUP_TYPE_LIST
	 * \brief Appends at the end of the first type-list all elements of the other type-lists.
	 * \note The resulting type-list container will always be the one of the first type-list.
	 * \details Let ``t0, t1, ..., tn`` be type-lists with ``ti[j]`` denoting the ``j-th`` element of the ``i-th`` type-list
	 * and ``li`` the length of the ``i-th`` type-list.
	 * The resulting type-list will then be built like the following pattern:
	 * \code{.unparsed}
	 * <t0[0], t0[1], ..., t0[l0 - 1], t1[0], t1[1], ..., t1[l1 - 1], tn[0], tn[1], ..., tn[ln - 1]>
	 * \endcode
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
	 * \defgroup GROUP_TYPE_LIST_APPEND append
	 * \ingroup GROUP_TYPE_LIST
	 * \brief Adds all of given types at the end of the source type-list.
	 * \{
	 */

	/**
	 * \brief Primary template isn't defined on purpose.
	 * \tparam List The provided type-lists.
	 * \tparam Types The types to be added.
	 */
	template <concepts::type_list_like List, class... Types>
	struct append;

	/**
	 * \brief Adds the given types at the end of the source type-list.
	 * \tparam Container The container type.
	 * \tparam Elements The element types.
	 * \tparam Types The types to be added.
	 */
	template <template <class...> class Container, class... Elements, class... Types>
	struct append<Container<Elements...>, Types...>
	{
		using type = Container<Elements..., Types...>;
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::type_list::append "append" trait.
	 * \tparam List The provided type-lists.
	 * \tparam Types The types to be added.
	 */
	template <concepts::type_list_like List, class... Types>
	using append_t = typename append<List, Types...>::type;

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_LIST_PREPEND prepend
	 * \ingroup GROUP_TYPE_LIST
	 * \brief Adds all of given types at the begin of the source type-list.
	 * \{
	 */

	/**
	 * \brief Primary template isn't defined on purpose.
	 * \tparam List The provided type-lists.
	 * \tparam Types The types to be added.
	 */
	template <concepts::type_list_like List, class... Types>
	struct prepend;

	/**
	 * \brief Adds the given types at the begin of the source type-list.
	 * \tparam Container The container type.
	 * \tparam Elements The element types.
	 * \tparam Types The types to be added.
	 */
	template <template <class...> class Container, class... Elements, class... Types>
	struct prepend<Container<Elements...>, Types...>
	{
		using type = Container<Types..., Elements...>;
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::type_list::prepend "prepend" trait.
	 * \tparam List The provided type-lists.
	 * \tparam Types The types to be added.
	 */
	template <concepts::type_list_like List, class... Types>
	using prepend_t = typename prepend<List, Types...>::type;

	/**
	 * \}
	 */
}

namespace sl::type_list
{
	/**
	 * \defgroup GROUP_TYPE_LIST_FILTER filter
	 * \ingroup GROUP_TYPE_LIST
	 * \brief Applies the given filter trait an each element and omits it from the resulting type-list if the filter yields ``false``.
	 * \details The filter trait must be a template template type accepting one template type argument and defining the
	 * ``static constexpr bool value`` member.
	 * \{
	 */

	/**
	 * \brief Primary template isn't defined on purpose.
	 * \tparam Filter The provided filter trait.
	 * \tparam List The provided type-lists.
	 * \tparam Filter The provided filter trait.
	 */
	template <concepts::type_list_like List, template <class> class Filter>
	struct filter;

	/**
	 * \brief Applies the filter trait on each element of the source type-list.
	 * \tparam Filter The provided filter trait.
	 * \tparam Container The container type.
	 * \tparam Elements The element types.
	 * \tparam Filter The provided filter trait.
	 */
	template <template <class...> class Container, class... Elements, template <class> class Filter>
	struct filter<Container<Elements...>, Filter>
	{
		using type = concat_t<
			Container<>,
			std::conditional_t<
				Filter<Elements>::value,
				Container<Elements>,
				Container<>>...
		>;
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::type_list::filter "filter" trait.
	 * \tparam Filter The provided filter trait.
	 * \tparam List The provided type-lists.
	 * \tparam Filter The provided filter trait.
	 */
	template <concepts::type_list_like List, template <class> class Filter>
	using filter_t = typename filter<List, Filter>::type;

	/**
	 * \}
	 */
}

namespace sl::type_list::detail
{
	template <class Type>
	struct not_same_as_filter
	{
		template <class Other>
		using type = std::negation<std::is_same<Type, Other>>;
	};
}

namespace sl::type_list
{
	/**
	 * \defgroup GROUP_TYPE_LIST_REMOVE remove
	 * \ingroup GROUP_TYPE_LIST
	 * \brief Removes all appearances of the given type from the source type-list.
	 * \{
	 */

	/**
	 * \brief Applies the filter trait on each element of the source type-list.
	 * \tparam Type The type to be removed.
	 * \tparam List The provided type-lists.
	 * \tparam Type The type to be removed.
	 */
	template <concepts::type_list_like List, class Type>
	struct remove
		: public filter<List, detail::not_same_as_filter<Type>::template type>
	{
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::type_list::remove "remove" trait.
	 * \tparam Type The type to be removed.
	 * \tparam List The provided type-lists.
	 * \tparam Type The type to be removed.
	 */
	template <concepts::type_list_like List, class Type>
	using remove_t = typename remove<List, Type>::type;

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_LIST_REMOVE_AT remove_at
	 * \ingroup GROUP_TYPE_LIST
	 * \brief Removes the element at the given index.
	 * \{
	 */

	/**
	 * \brief Primary template, traversing all elements of the given type-list.
	 * \tparam List The provided type-lists.
	 * \tparam index The index to be removed.
	 */
	template <concepts::populated_type_list List, std::size_t index>
		requires (index < std::tuple_size_v<List>)
	struct remove_at
	{
		using type = prepend_t<
			typename remove_at<tail_t<List>, index - 1u>::type,
			front_t<List>>;
	};

	/**
	 * \brief Root specialization, omitting the first element from the source type-list.
	 * \tparam List The provided type-lists.
	 */
	template <concepts::populated_type_list List>
	struct remove_at<List, 0>
	{
		using type = tail_t<List>;
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::type_list::remove_at "remove_at" trait.
	 * \tparam index The index to be removed.
	 * \tparam List The provided type-lists.
	 */
	template <concepts::populated_type_list List, std::size_t index>
	using remove_at_t = typename remove_at<List, index>::type;

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_LIST_POP_FRONT pop front
	 * \ingroup GROUP_TYPE_LIST
	 * \brief Removes the first element of the given type-list.
	 * \{
	 */

	/**
	 * \brief Removes the first element of the given type-list.
	 * \tparam List The provided type-list.
	 */
	template <concepts::populated_type_list List>
	using pop_front = remove_at<List, 0>;

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::type_list::pop_front "pop_front" trait.
	 * \tparam List The provided type-list.
	 */
	template <concepts::populated_type_list List>
	using pop_front_t = typename pop_front<List>::type;

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_LIST_POP_BACK pop back
	 * \ingroup GROUP_TYPE_LIST
	 * \brief Removes the last element of the given type-list.
	 * \{
	 */

	/**
	 * \brief Removes the last element of the given type-list.
	 * \tparam List The provided type-list.
	 */
	template <concepts::populated_type_list List>
	using pop_back = remove_at<List, std::tuple_size_v<List> - 1u>;

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::type_list::pop_back "pop_back" trait.
	 * \tparam List The provided type-list.
	 */
	template <concepts::populated_type_list List>
	using pop_back_t = typename pop_back<List>::type;

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
	 * \brief Primary template, traversing the elements of the source type-list.
	 * \tparam List The provided type-list.
	 * \tparam Query The type to be queried for.
	 */
	template <concepts::populated_type_list List, class Query>
	struct index_of
		: public std::integral_constant<std::size_t, 1u + index_of<tail_t<List>, Query>::value>
	{
	};

	/**
	 * \brief Specialization if the queried type appears at the beginning of the type-list.
	 * \tparam List The provided type-list.
	 * \tparam Query The type to be queried for.
	 */
	template <concepts::populated_type_list List>
	struct index_of<List, front_t<List>>
		: public std::integral_constant<std::size_t, 0u>
	{
	};

	/**
	 * \brief Convenience constant, exposing the ``value`` member of the \ref sl::type_list::index_of "index_of" trait.
	 * \tparam Query The type to be queried for.
	 * \tparam List The provided type-list.
	 * \tparam Query The type to be queried for.
	 */
	template <concepts::populated_type_list List, class Query>
	inline constexpr std::size_t index_of_v = index_of<List, Query>::value;

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_LIST_CONTAINS contains
	 * \ingroup GROUP_TYPE_LIST
	 * \brief Queries the source type-list whether it contains a specific type.
	 * \{
	 */

	/**
	 * \brief Primary template isn't defined on purpose.
	 * \tparam List The provided type-list.
	 * \tparam Query The type to be queried for.
	 */
	template <concepts::type_list_like List, class Query>
	struct contains;

	/**
	 * \brief Specialization determining the presence of the query type, due to comparing it with each element of the type-list.
	 * \tparam Container The container type.
	 * \tparam Elements The element types.
	 * \tparam Query The type to be queried for.
	 */
	template <template <class...> class Container, class... Elements, class Query>
	struct contains<Container<Elements...>, Query>
		: public std::bool_constant<(std::same_as<Query, Elements> || ...)>
	{
	};

	/**
	 * \brief Convenience constant, exposing the ``value`` member of the \ref sl::type_list::contained_by "contains" trait.
	 * \tparam List The provided type-list.
	 * \tparam Query The type to be queried for.
	 */
	template <concepts::type_list_like List, class Query>
	inline constexpr bool contains_v = contains<List, Query>::value;

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_LIST_REVERSE reverse
	 * \ingroup GROUP_TYPE_LIST
	 * \brief Reverses the order of all elements.
	 * \{
	 */

	/**
	 * \brief Reverses the order of all elements.
	 * \tparam List The provided type-list.
	 */
	template <concepts::type_list_like List>
	struct reverse
	{
		using type = append_t<
			pop_front_t<List>,
			front_t<List>>;
	};

	/**
	 * \brief Specialization for empty type-lists.
	 * \tparam Container The container template.
	 */
	template <template <class...> class Container>
	struct reverse<Container<>>
	{
		using type = Container<>;
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::type_list::reverse "reverse" trait.
	 * \tparam List The provided type-list.
	 */
	template <concepts::type_list_like List>
	using reverse_t = typename reverse<List>::type;

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_LIST_UNORDERED_EQUAL unordered_equal
	 * \ingroup GROUP_TYPE_LIST
	 * \brief Determines whether the source type-lists contain the same elements.
	 * \{
	 */

	/**
	 * \brief Primary template isn't defined on purpose.
	 * \tparam First The first type-list.
	 * \tparam OtherLists Other type-lists to be compared to.
	 */
	template <concepts::type_list_like First, concepts::type_list_like... OtherLists>
	struct unordered_equal;

	/**
	 * \brief Root specialization, determining whether all type-lists are empty.
	 * \tparam FirstContainer The container template of the first type-list.
	 * \tparam OtherLists Other type-lists to be compared to.
	 */
	template <template <class...> class FirstContainer, concepts::type_list_like... OtherLists>
	struct unordered_equal<FirstContainer<>, OtherLists...>
		: public std::bool_constant<((0 == std::tuple_size_v<OtherLists>) && ...)>
	{
	};

	/**
	 * \brief Specialization, determining whether the current element appears equally often in all type-lists.
	 * \tparam FirstContainer The container template of the first type-list.
	 * \tparam CurElement The current element to be checked.
	 * \tparam OtherElements Other elements of the first type-list.
	 * \tparam OtherLists Other type-lists to be compared to.
	 */
	template <
		template <class...> class FirstContainer,
		class CurElement,
		class... OtherElements,
		concepts::type_list_like... OtherLists>
	struct unordered_equal<FirstContainer<CurElement, OtherElements...>, OtherLists...>
		: public std::conditional_t<
			(... && (count_v<CurElement, FirstContainer<CurElement, OtherElements...>> == count_v<CurElement, OtherLists>)),
			unordered_equal<remove_t<FirstContainer<OtherElements...>, CurElement>, remove_t<OtherLists, CurElement>...>,
			std::false_type>
	{
	};

	/**
	 * \brief Convenience constant, exposing the ``value`` member of the \ref sl::type_list::unordered_equal "unordered_equal" trait.
	 * \tparam First The first type-list.
	 * \tparam Others Other type-lists to be compared to.
	 */
	template <concepts::type_list_like First, concepts::type_list_like... Others>
	inline constexpr bool unordered_equal_v = unordered_equal<First, Others...>::value;

	/**
	 * \}
	 */
	 * \defgroup GROUP_TYPE_LIST_ZIP_NTH_ELEMENT zip_nth_element
	 * \ingroup GROUP_TYPE_LIST
	 * \brief Given multiple type-lists and an index ``n`` (where each type-lists has at least the length ``n + 1``) this algorithm
	 * yields a new type-list consisting of the ``n-th`` elements of all source type-lists.
	 * \details Let ``t0, t1, ..., tm`` be type-lists with ``ti[j]`` denoting the ``j-th`` element of the ``i-th`` type-list and ``n``
	 * the given index. The resulting type-list will then be built like the following pattern:
	 * \code{.unparsed}
	 * <t0[n], t1[n], ..., tm[n]>
	 * \endcode
	 * \{
	 */

	/**
	 * \brief Algorithm yielding the result as the ``type`` member alias.
	 * \tparam TargetContainer The resulting container.
	 * \tparam index The elements index.
	 * \tparam Lists The provided type-lists.
	 */
	template <template <class...> class TargetContainer, std::size_t index, concepts::type_list_like... Lists>
		requires (sizeof...(Lists) == 0)
				|| (index < std::min({std::tuple_size_v<Lists>...}))
	struct zip_nth_elements_as
	{
		using type = TargetContainer<std::tuple_element_t<index, Lists>...>;
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::type_list::zip_nth_elements_as "zip_nth_elements_as" trait.
	 * \tparam TargetContainer The resulting container.
	 * \tparam index The elements index.
	 * \tparam Lists The provided type-lists.
	 */
	template <template <class...> class TargetContainer, std::size_t index, concepts::type_list_like... Lists>
	using zip_nth_elements_as_t = typename zip_nth_elements_as<TargetContainer, index, Lists...>::type;

	/**
	 * \brief Alternative algorithm yielding the result as the ``type`` member alias and determining the result container via
	 * \ref sl::type_list::common_container "common_container" trait.
	 * \tparam index The elements index.
	 * \tparam Lists The provided type-lists.
	 */
	template <std::size_t index, concepts::type_list_like... Lists>
	using zip_nth_elements = zip_nth_elements_as<common_container<Lists...>::template type, index, Lists...>;

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::type_list::zip_nth_elements "zip_nth_elements" trait.
	 * \tparam index The elements index.
	 * \tparam Lists The provided type-lists.
	 */
	template <std::size_t index, concepts::type_list_like... Lists>
	using zip_nth_elements_t = typename zip_nth_elements<index, Lists...>::type;

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
		using type = TargetContainer<zip_nth_elements_as_t<TargetContainer, indices, Lists...>...>;
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
	 * \brief Given multiple type-lists this algorithm yields a new type-list consisting of the ``n`` type-lists (where ``n`` is the
	 * minimal size of the source type-lists).
	 * \details Let ``t0, t1, ..., tn`` be type-lists with ``ti[j]`` denoting the ``j-th`` element of the ``i-th`` type-list
	 * and ``m`` the minimum size of the source type-lists.
	 * The resulting type-list will then be built like the following pattern:
	 * \code{.unparsed}
	 * <
	 *		<t0[0], t1[0], ..., tn[0]>,
	 *		<t0[1], t1[1], ..., tn[1]>,
	 *		...,
	 *		<t0[m], t1[m], ..., tn[m]>
	 * >
	 *	\endcode
	 * \{
	 */

	/**
	 * \brief Algorithm yielding the result as the ``type`` member alias.
	 * \tparam TargetContainer The resulting container.
	 * \tparam Lists The provided type-lists.
	 */
	template <template <class...> class TargetContainer, concepts::type_list_like... Lists>
	struct zip_as
	{
		using type = detail::zip_as_t<TargetContainer, Lists...>;
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::type_list::zip_as "zip_as" trait.
	 * \tparam TargetContainer The resulting container.
	 * \tparam Lists The provided type-lists.
	 */
	template <template <class...> class TargetContainer, concepts::type_list_like... Lists>
	using zip_as_t = typename zip_as<TargetContainer, Lists...>::type;

	/**
	 * \brief Alternative algorithm yielding the result as the ``type`` member alias and determining the result container via
	 * \ref sl::type_list::common_container "common_container" trait.
	 * \tparam Lists The provided type-lists.
	 */
	template <concepts::type_list_like... Lists>
	using zip = zip_as<common_container<Lists...>::template type, Lists...>;

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::type_list::zip "zip" trait.
	 * \tparam Lists The provided type-lists.
	 */
	template <concepts::type_list_like... Lists>
	using zip_t = typename zip<Lists...>::type;

	/**
	 * \}
	 */
}

namespace sl::type_list::detail
{
	template <class... Types>
	struct prepend_transform
	{
		template <concepts::type_list_like CurList>
		using type = prepend<CurList, Types...>;
	};
}

namespace sl::type_list
{
	/**
	 * \defgroup GROUP_TYPE_LIST_CARTESIAN_PRODUCT cartesian product
	 * \ingroup GROUP_TYPE_LIST
	 * \brief Given multiple source type-lists this algorithm yields a new type-list combining each element of each list with each other.
	 * \details Let ``t0, t1, ..., tn`` be type-lists with ``ti[j]`` denoting the ``j-th`` element of the ``i-th`` type-list
	 * and ``li`` the size of the ``i-th`` source type-lists.
	 * The resulting type-list will then be built like the following pattern:
	 * \code{.unparsed}
	 * <
	 *		<t0[0], t1[0], ..., tn[0]>,
	 *		...,
	 *		<t0[0], t1[0], ..., tn[ln-1]>,
	 *
	 *		...,
	 *
	 *		<t0[0], t1[l1-1], ..., tn[0]>,
	 *		...,
	 *		<t0[0], t1[l1-1], ..., tn[ln-1]>,
	 *
	 *		...,
	 *
	 *		<t0[l0-1], t1[0], ..., tn[0]>,
	 *		...,
	 *		<t0[l0-1], t1[0], ..., tn[ln-1]>,
	 *
	 *		...,
	 *
	 *		<t0[l0-1], t1[l1-1], ..., tn[0]>,
	 *		...,
	 *		<t0[l0-1], t1[l1-1], ..., tn[ln-1]>
	 * >
	 *	\endcode
	 * \{
	 */

	/**
	 * \brief Primary template isn't defined on purpose.
	 * \tparam TargetContainer The resulting container.
	 * \tparam Lists The provided type-lists.
	 */
	template <template <class...> class TargetContainer, concepts::type_list_like... Lists>
	struct cartesian_product_as;

	/**
	 * \brief Root specialization, enveloping each element of the list in a ``TargetContainer``.
	 * \tparam TargetContainer The resulting container.
	 * \tparam Container The container type.
	 * \tparam Elements The element types.
	 */
	template <template <class...> class TargetContainer, template <class...> class Container, class... Elements>
	struct cartesian_product_as<TargetContainer, Container<Elements...>>
	{
		using type = TargetContainer<
			TargetContainer<Elements>...
		>;
	};

	/**
	 * \brief Specialization, prepending all elements of the current type-list to the already created type-lists. 
	 * \tparam TargetContainer The resulting container.
	 * \tparam Container The container type.
	 * \tparam Elements The element types.
	 * \tparam OtherLists All other type-lists.
	 */
	template <
		template <class...> class TargetContainer,
		template <class...> class Container,
		class... Elements,
		concepts::type_list_like... OtherLists>
	struct cartesian_product_as<TargetContainer, Container<Elements...>, OtherLists...>
	{
		using type = concat_t<
			transform_t<
				detail::prepend_transform<Elements>::template type,
				typename cartesian_product_as<TargetContainer, OtherLists...>::type>...
		>;
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::type_list::cartesian_product_as "cartesian_product_as" trait.
	 * \tparam TargetContainer The resulting container.
	 * \tparam Lists The provided type-lists.
	 */
	template <template <class...> class TargetContainer, concepts::type_list_like... Lists>
	using cartesian_product_as_t = typename cartesian_product_as<TargetContainer, Lists...>::type;

	/**
	 * \brief Alternative algorithm yielding the result as the ``type`` member alias and determining the result container via
	 * \ref sl::type_list::common_container "common_container" trait.
	 * \tparam Lists The provided type-lists.
	 */
	template <concepts::type_list_like... Lists>
	using cartesian_product = cartesian_product_as<common_container<Lists...>::template type, Lists...>;

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::type_list::cartesian_product "cartesian_product" trait.
	 * \tparam Lists The provided type-lists.
	 */
	template <concepts::type_list_like... Lists>
	using cartesian_product_t = typename cartesian_product<Lists...>::type;

	/**
	 * \}
	 */
}

#endif
