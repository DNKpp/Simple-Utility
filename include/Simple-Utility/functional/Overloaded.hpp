//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_OVERLOADED_HPP
#define SL_UTILITY_FUNCTIONAL_OVERLOADED_HPP

namespace sl::functional
{
	/**
	 * \addtogroup GROUP_FUNCTIONAL
	 * \{
	 */

	/**
	 * \brief Helper invocable type which can hold an arbitrary amount of functions,
	 * from which the best match will be selected during invocation.
	 * \tparam Fns The function types.
	 *
	 * \note All parameter sets must be distinguishable from each other.
	 */
	template <class... Fns>
	struct Overloaded
		: public Fns...
	{
		using Fns::operator()...;
	};

	/**
	 * \brief Deduction guide of overloaded type.
	 * \tparam Fns The function types.
	 */
	template <class... Fns>
	Overloaded(Fns...) -> Overloaded<Fns...>;

	/**
	 * \}
	 */
}

#endif
