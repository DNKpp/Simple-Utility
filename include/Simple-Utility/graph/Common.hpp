//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_COMMON_HPP
#define SIMPLE_UTILITY_GRAPH_COMMON_HPP

#pragma once

#include "Simple-Utility/TypeList.hpp"
#include "Simple-Utility/concepts/operators.hpp"

#include <algorithm>
#include <concepts>

namespace sl::graph
{
	struct basic_feature_category
	{
	};

	struct ranked_feature_category
	{
	};
}

namespace sl::graph::detail
{
	using feature_category_list = type_list::TypeList<basic_feature_category, ranked_feature_category>;
}

namespace sl::graph::concepts
{
	template <class T>
	concept feature_category = type_list::contains_v<detail::feature_category_list, T>;

	template <class T>
	concept vertex = std::same_as<T, std::remove_cvref_t<T>>
					&& std::equality_comparable<T>
					&& std::copyable<T>;

	template <class T>
	concept rank = std::same_as<T, std::remove_cvref_t<T>>
					&& std::totally_ordered<T>
					&& std::regular<T>
					&& sl::concepts::plus<T>
					&& sl::concepts::minus<T>
					&& sl::concepts::plus_assign<T>
					&& sl::concepts::minus_assign<T>;

	template <class T>
	concept readable_vertex_type = requires { typename T::vertex_type; }
									&& vertex<typename T::vertex_type>;

	template <class T>
	concept readable_rank_type = requires { typename T::rank_type; }
								&& vertex<typename T::rank_type>;
}

namespace sl::graph
{
	template <concepts::feature_category T, concepts::feature_category... Others>
	struct common_feature_category
	{
		using type = std::tuple_element_t<
			std::ranges::min(
				{
					type_list::index_of_v<detail::feature_category_list, T>,
					type_list::index_of_v<detail::feature_category_list, Others>...
				}),
			detail::feature_category_list>;
	};

	template <class T, class... Others>
	using common_feature_category_t = typename common_feature_category<T, Others...>::type;

	template <class T>
	struct feature_traits;

	template <class T>
	using feature_category_t = typename feature_traits<T>::category_type;

	template <class T>
	using feature_vertex_t = typename feature_traits<T>::vertex_type;

	template <class T>
	using feature_rank_t = typename feature_traits<T>::rank_type;

	template <class T>
		requires concepts::readable_vertex_type<T>
	struct feature_traits<T>
	{
		using category_type = basic_feature_category;
		using vertex_type = typename T::vertex_type;
	};

	template <class T>
		requires concepts::readable_vertex_type<T>
				&& concepts::readable_rank_type<T>
	struct feature_traits<T>
	{
		using category_type = ranked_feature_category;
		using vertex_type = typename T::vertex_type;
		using rank_type = typename T::rank_type;
	};
}

#endif
