//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstddef>

namespace sl
{
	/**
	 * \defgroup GROUP_PRIORITY_TAG priority_tag
	 * \brief Helper type, useful when prioritize overloads of an overload set.
	 * \see Got the idea from here: https://quuxplusone.github.io/blog/2021/07/09/priority-tag/
	* \{
	*/

	/**
	 * \brief Primary template, inheriting from the other specializations with lesser priority.
	 */
	template <std::size_t priority>
	struct priority_tag
		: public priority_tag<priority - 1>
	{
	};


	/**
	 * \brief Specialization, denoting the least likely alternative.
	 */
	template <>
	struct priority_tag<0>
	{
	};

	/**
	 * \}
	 */
}
