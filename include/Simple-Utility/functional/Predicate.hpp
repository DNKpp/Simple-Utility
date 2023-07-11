//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_PREDICATE_HPP
#define SL_UTILITY_FUNCTIONAL_PREDICATE_HPP

#pragma once

#include "Simple-Utility/functional/base.hpp"
#include "Simple-Utility/functional/operators/bind.hpp"
#include "Simple-Utility/functional/operators/conjunction.hpp"
#include "Simple-Utility/functional/operators/disjunction.hpp"
#include "Simple-Utility/functional/operators/equal.hpp"
#include "Simple-Utility/functional/operators/equivalent.hpp"
#include "Simple-Utility/functional/operators/negation.hpp"
#include "Simple-Utility/functional/operators/pipe.hpp"

#include "Simple-Utility/functional/BasicClosure.hpp"
#include "Simple-Utility/functional/mixins/Conjunction.hpp"
#include "Simple-Utility/functional/mixins/Disjunction.hpp"
#include "Simple-Utility/functional/mixins/Equality.hpp"
#include "Simple-Utility/functional/mixins/InvokePolicies.hpp"
#include "Simple-Utility/functional/mixins/Negation.hpp"
#include "Simple-Utility/functional/mixins/Pipe.hpp"

namespace sl::functional
{
	/**
	 * \defgroup GROUP_FUNCTIONAL_PREDICATE predicate
	 * \ingroup GROUP_FUNCTIONAL
	 * \brief Contains several pre-defined predicate objects.
	 * \details Predicate types aim to simplify the composition of multiple conditions and are even fully compatible with the transform
	 * functions. Predicates are therefore pipe-able with any other functional type (the params and return types still have to match, which can't
	 * be checked before the actual invocation) and offer many more composing operators.
	 * Predicates also aim to be flat as possible, which means, if users chain multiple predicates via supported operators, instead of simply
	 * building a tree like structure, the functional objects will be combined into one ``Composition``. This keeps the calling-hierarchy as
	 * flat as possible and also supports easier debugging. Operations following this strategy are:
	 * - pipe
	 * - conjunction
	 * - disjunction
	 *
	 * \note Predicates are also composable via operator ==, != and <=>, but there is one subtle difference between operator == and <=>, which comes into
	 * play when chaining three or more predicates together. For two predicates the result is equivalent.
	 * Operator == compares predicates in a nested manner, thus
	 * \code{.cpp}
	 * pred1 == pred2 == pred3;
	 * \endcode
	 * is in fact
	 * \code{.cpp}
	 * (pred1 == pred2) == pred3;
	 * \endcode
	 * That results is sometimes unexpected behaviour when ``pred1``, ``pred2`` and ``pred3`` are for example all ``false``. This will result in
	 * ``(false == false) == false`` which is obviously not true. Operator <=> in contrast takes the result of the first function and compares
	 * it to all other predicate results. The above example will then result in the following comparison:
	 * \code{.cpp}
	 * bool init = pred1();
	 * bool result = init == pred2() && init == pred3();
	 * \endcode
	 * and therefore actually check if all results are equal.
	 * @{
	 */

	/**
	 * \brief Closure template for predicate like types, which accepts a functional type and enables pipe, conjunctive and disjunctive chaining,
	 * equal, inequality and equivalence comparison, and front and back binding.
	 * \tparam Fn The functional type.
	 */
	template <function Fn>
	using Predicate = BasicClosure<
		Fn,
		PredicateInvokePolicy,
		PipeOperator,
		EqualityOperator,
		ConjunctionOperator,
		DisjunctionOperator,
		NegationOperator>;

	/** @} */
}

#endif