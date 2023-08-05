//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_TRANSFORM_HPP
#define SL_UTILITY_FUNCTIONAL_TRANSFORM_HPP

#pragma once

#include "Simple-Utility/functional/BasicClosure.hpp"
#include "Simple-Utility/functional/mixins/InvokePolicies.hpp"
#include "Simple-Utility/functional/mixins/Pipe.hpp"

namespace sl::functional
{
	/**
	 * \defgroup GROUP_FUNCTIONAL_TRANSFORM transform
	 * \ingroup GROUP_FUNCTIONAL
	 * \brief Contains the ``Transform`` closure template.
	 * \details Transform types aim to simplify the composition of multiple transformations on objects and are therefore composable via
	 * pipe operator (operator |). Transforms also aim to be flat as possible, which means, if users chain multiple transforms, instead of simply
	 * building a tree like structure, the functional objects will be combined into one ``composition_fn``. This keeps the calling-hierarchy as
	 * flat as possible and also supports easier debugging.
	 * 
	 * \{
	 */

	/**
	 * \brief Closure template for variadic transform like functional types, which enables piping.
	 * \tparam Fn The functional type.
	 */
	template <function Fn>
	using Transform = BasicClosure<Fn, NodiscardInvokePolicy, PipeOperator>;

	/**
	 * \}
	 */
}

#endif
