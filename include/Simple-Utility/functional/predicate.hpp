//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_PREDICATE_HPP
#define SL_UTILITY_FUNCTIONAL_PREDICATE_HPP

#pragma once

#include "Simple-Utility/functional/base.hpp"
#include "Simple-Utility/functional/operators/pipe.hpp"
#include "Simple-Utility/functional/operators/bind.hpp"
#include "Simple-Utility/functional/operators/conjunction.hpp"
#include "Simple-Utility/functional/operators/disjunction.hpp"
#include "Simple-Utility/functional/operators/negation.hpp"
#include "Simple-Utility/functional/operators/equal.hpp"

namespace sl::functional
{
	/**
	 * \defgroup GROUP_FUNCTIONAL_PREDICATE predicate
	 * \ingroup GROUP_FUNCTIONAL
	 * \brief Contains the base ``predicate_fn`` and several pre-defined predicate objects.
	 * \details Predicate types aim to simplify the composition of multiple conditions and are even fully compatible with the transformer
	 * functions. Predicates are therefore pipe-able with any other functional type (the params and return types still have to match, which can't
	 * be checked before the actual invocation) and offer many more composing operators.
	 * Predicates also aim to be flat as possible, which means, if users chain multiple predicates via supported operators, instead of simply
	 * building a tree like structure, the functional objects will be combined into one ``composition_fn``. This keeps the calling-hierarchy as
	 * flat as possible and also supports easier debugging. Operations following this strategy are:
	 * - pipe
	 * - conjunction
	 * - disjunction
	 * - bind_front
	 * - bind_back
	 * @{
	 */

	/**
	 * \brief Base type for predicate functionals, which accepts a functional type and enables pipe, conjunctive and disjunctive chaining,
	 * and front and back binding.
	 * \tparam TFunc The functional type.
	 */
	template <class TFunc>
		requires std::same_as<TFunc, std::remove_cvref_t<TFunc>>
	class predicate_fn
		: public closure_base_fn<TFunc>,
		public operators::pipe<predicate_fn<TFunc>, predicate_fn>,
		public operators::conjunction<predicate_fn<TFunc>, predicate_fn>,
		public operators::disjunction<predicate_fn<TFunc>, predicate_fn>,
		public operators::negation<predicate_fn<TFunc>, predicate_fn>,
		public operators::bind_front<predicate_fn<TFunc>, predicate_fn>,
		public operators::bind_back<predicate_fn<TFunc>, predicate_fn>,
		public operators::equal_compare<predicate_fn<TFunc>, predicate_fn>
	{
		using closure_t = closure_base_fn<TFunc>;
	public:
		using closure_t::closure_t;
	};

	/**
	 * \brief Deduction guide.
	 * \tparam TFunc Type of the given functional.
	 */
	template <class TFunc>
	predicate_fn(TFunc) -> predicate_fn<TFunc>;

	/**
	 * \brief Functional object, which compares its two operands less.
	 * \note If both operands satisfy the ``std::integral`` concept, ``std::cmp_less`` is used instead.
	 */
	inline constexpr predicate_fn less{
		[]<class TLhs, class TRhs>(TLhs&& lhs, TRhs&& rhs)
		{
			if constexpr (std::integral<TLhs> && std::integral<TRhs>)
			{
				return std::cmp_less(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs));
			}
			else
				return std::forward<TLhs>(lhs) < std::forward<TRhs>(rhs);
		}
	};

	/**
	 * \brief Functional object, which compares its two operands less-equal.
	 * \note If both operands satisfy the ``std::integral`` concept, ``std::cmp_less_equal`` is used instead.
	 */
	inline constexpr predicate_fn less_equal{
		[]<class TLhs, class TRhs>(TLhs&& lhs, TRhs&& rhs)
		{
			if constexpr (std::integral<TLhs> && std::integral<TRhs>)
			{
				return std::cmp_less_equal(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs));
			}
			else
				return std::forward<TLhs>(lhs) <= std::forward<TRhs>(rhs);
		}
	};

	/**
	 * \brief Functional object, which compares its two operands greater.
	 * \note If both operands satisfy the ``std::integral`` concept, ``std::cmp_greater`` is used instead.
	 */
	inline constexpr predicate_fn greater{
		[]<class TLhs, class TRhs>(TLhs&& lhs, TRhs&& rhs)
		{
			if constexpr (std::integral<TLhs> && std::integral<TRhs>)
			{
				return std::cmp_greater(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs));
			}
			else
				return std::forward<TLhs>(lhs) > std::forward<TRhs>(rhs);
		}
	};

	/**
	 * \brief Functional object, which compares its two operands greater-equal.
	 * \note If both operands satisfy the ``std::integral`` concept, ``std::cmp_greater_equal`` is used instead.
	 */
	inline constexpr predicate_fn greater_equal{
		[]<class TLhs, class TRhs>(TLhs&& lhs, TRhs&& rhs)
		{
			if constexpr (std::integral<TLhs> && std::integral<TRhs>)
			{
				return std::cmp_greater_equal(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs));
			}
			else
				return std::forward<TLhs>(lhs) >= std::forward<TRhs>(rhs);
		}
	};

	/**
	 * \brief Functional object, which compares its two operands equal.
	 * \note If both operands satisfy the ``std::integral`` concept, ``std::cmp_equal`` is used instead.
	 */
	inline constexpr predicate_fn equal{
		[]<class TLhs, class TRhs>(TLhs&& lhs, TRhs&& rhs)
		{
			if constexpr (std::integral<TLhs> && std::integral<TRhs>)
			{
				return std::cmp_equal(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs));
			}
			else
				return std::forward<TLhs>(lhs) == std::forward<TRhs>(rhs);
		}
	};

	/**
	 * \brief Functional object, which compares its two operands not-equal.
	 * \note If both operands satisfy the ``std::integral`` concept, ``std::cmp_not_equal`` is used instead.
	 */
	inline constexpr predicate_fn not_equal{
		[]<class TLhs, class TRhs>(TLhs&& lhs, TRhs&& rhs)
		{
			if constexpr (std::integral<TLhs> && std::integral<TRhs>)
			{
				return std::cmp_not_equal(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs));
			}
			else
				return std::forward<TLhs>(lhs) != std::forward<TRhs>(rhs);
		}
	};

	/** @} */
}

#endif
