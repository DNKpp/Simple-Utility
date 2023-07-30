//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_PREDICATE_HPP
#define SL_UTILITY_FUNCTIONAL_PREDICATE_HPP

#pragma once

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
	 * \details Predicate types aim to simplify the composition of multiple conditions.
	 * Predicates are therefore pipe-able with any other functional type (the params and return types still have to match, which can't
	 * be checked before the actual invocation) and offer many more composing operators.
	 * Predicates also aim to be flat as possible, which means, if users chain multiple predicates via supported operators, instead of simply
	 * building a tree like structure, the functional objects will be combined into one ``Composition``. This keeps the calling-hierarchy as
	 * flat as possible and also supports easier debugging.
	 * 
	 * \{
	 */

	/**
	 * \brief Closure template for predicate like types, which accepts a functional type and enables pipe, negation, conjunctive and disjunctive chaining,
	 * and equality comparison.
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

	/**
	 * \}
	 */
}

#endif
