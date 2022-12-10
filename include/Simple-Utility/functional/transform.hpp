//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_TRANSFORM_HPP
#define SL_UTILITY_FUNCTIONAL_TRANSFORM_HPP

#pragma once

#include "Simple-Utility/unified_base.hpp"
#include "Simple-Utility/concepts/stl_extensions.hpp"
#include "Simple-Utility/concepts/operators.hpp"
#include "Simple-Utility/functional/base.hpp"
#include "Simple-Utility/functional/operators/bind.hpp"
#include "Simple-Utility/functional/operators/pipe.hpp"

namespace sl::functional
{
	/**
	 * \defgroup GROUP_FUNCTIONAL_TRANSFORM transform
	 * \brief Contains the base ``transform_fn`` and several pre-defined transform objects.
	 * \ingroup GROUP_FUNCTIONAL
	 * \details Transform types aim to simplify the composition of multiple transformations on objects and are therefore composable via
	 * pipe operator (operator |). Transforms also aim to be flat as possible, which means, if users chain multiple transforms, instead of simply
	 * building a tree like structure, the functional objects will be combined into one ``composition_fn``. This keeps the calling-hierarchy as
	 * flat as possible and also supports easier debugging. Operations following this strategy are:
	 * - pipe
	 * - bind_front
	 * - bind_back
	 * @{
	 */

	/**
	 * \brief Adapter type which accepts a functional type and enables pipe chaining and currying.
	 * \tparam TFunc The functional type.
	 */
	template <class TFunc>
		requires std::same_as<TFunc, std::remove_cvref_t<TFunc>>
	class transform_fn final
		: public closure_base_fn<TFunc>,
		public enable_operation<transform_fn,
								operators::pipe,
								operators::bind_front,
								operators::bind_back
		>
	{
		using closure_t = closure_base_fn<TFunc>;

	public:
		/**
		 * \brief Explicitly created forwarding constructor.
		 * \tparam TArgs Argument types.
		 * \param args Arguments, forwarded to the base class constructor.
		 * \note This is only here, to keep clang <= 11 happy, when creating constexpr functional objects.
		 */
		template <class... TArgs>
			requires std::constructible_from<closure_t, TArgs...>
		[[nodiscard]]
		constexpr
		/**\cond conditional-explicit*/
		explicit(detail::force_explicit_constructor_v<closure_t, TArgs...>)
		/**\endcond*/
		transform_fn
		(
			TArgs&&... args
		)
			noexcept(std::is_nothrow_constructible_v<closure_t, TArgs...>)
			: closure_t{ std::forward<TArgs>(args)... }
		{}
	};

	/**
	 * \brief Deduction guide.
	 * \tparam TFunc Type of the given functional.
	 */
	template <class TFunc>
	transform_fn(TFunc) -> transform_fn<TFunc>;

	namespace detail
	{
		template <class TTarget>
		struct as_fn
		{
			template <concepts::explicitly_convertible_to<TTarget> TFrom>
			[[nodiscard]]
			constexpr TTarget operator ()
			(
				TFrom&& v
			) const
				noexcept(concepts::nothrow_explicitly_convertible_to<TFrom, TTarget>)
			{
				return static_cast<TTarget>(std::forward<TFrom>(v));
			}
		};
	}

	/**
	 * \brief Functional object which static_cast the given argument to the target type on invocation.
	 * \tparam TTarget The target type.
	 */
	template <class TTarget>
	inline constexpr transform_fn as{ detail::as_fn<TTarget>{} };

	/** @} */

	/**
	 * \defgroup GROUP_FUNCTIONAL_ARITHMETIC arithmetic
	 * \brief Contains functional objects, implementing several arithmetically operations.
	 * \ingroup GROUP_FUNCTIONAL
	 * @{
	 */

	/**
	 * \brief Functional object which forwards the params to binary operator + and returns the result.
	 */
	inline constexpr transform_fn plus{
		[]<class TLhs, class TRhs>(TLhs&& lhs, TRhs&& rhs) -> decltype(auto)
		{
			static_assert(concepts::plus_with<TLhs, TRhs>, "Arguments are not usable as operands of binary operator +.");

			return std::forward<TLhs>(lhs) + std::forward<TRhs>(rhs);
		}
	};

	/**
	 * \brief Functional object which forwards the params to binary operator - and returns the result.
	 */
	inline constexpr transform_fn minus{
		[]<class TLhs, class TRhs>(TLhs&& lhs, TRhs&& rhs) -> decltype(auto)
		{
			static_assert(concepts::minus_with<TLhs, TRhs>, "Arguments are not usable as operands of binary operator -.");

			return std::forward<TLhs>(lhs) - std::forward<TRhs>(rhs);
		}
	};

	/**
	 * \brief Functional object which forwards the params to binary operator * and returns the result.
	 */
	inline constexpr transform_fn multiplies{
		[]<class TLhs, class TRhs>(TLhs&& lhs, TRhs&& rhs) -> decltype(auto)
		{
			static_assert(concepts::multiplies_with<TLhs, TRhs>, "Arguments are not usable as operands of binary operator *.");

			return std::forward<TLhs>(lhs) * std::forward<TRhs>(rhs);
		}
	};

	/**
	 * \brief Functional object which forwards the params to binary operator / and returns the result.
	 */
	inline constexpr transform_fn divides{
		[]<class TLhs, class TRhs>(TLhs&& lhs, TRhs&& rhs) -> decltype(auto)
		{
			static_assert(concepts::divides_with<TLhs, TRhs>, "Arguments are not usable as operands of binary operator /.");

			return std::forward<TLhs>(lhs) / std::forward<TRhs>(rhs);
		}
	};

	/**
	 * \brief Functional object which forwards the params to binary operator % and returns the result.
	 */
	inline constexpr transform_fn modulus{
		[]<class TLhs, class TRhs>(TLhs&& lhs, TRhs&& rhs) -> decltype(auto)
		{
			static_assert(concepts::modulus_with<TLhs, TRhs>, "Arguments are not usable as operands of binary operator %.");

			return std::forward<TLhs>(lhs) % std::forward<TRhs>(rhs);
		}
	};

	/**
	 * \brief Functional object which forwards the param to unary operator - and returns the result.
	 */
	inline constexpr transform_fn negate{
		[]<class T>(T&& value) -> decltype(auto)
		{
			static_assert(concepts::negate<T>, "Argument is not usable as operand of unary operator -.");

			return -std::forward<T>(value);
		}
	};

	/** @} */
}

#endif
