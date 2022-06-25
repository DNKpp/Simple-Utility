//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_TRANSFORM_HPP
#define SL_UTILITY_FUNCTIONAL_TRANSFORM_HPP

#pragma once

#include "Simple-Utility/bind_back.hpp"
#include "Simple-Utility/unified_base.hpp"
#include "Simple-Utility/functional/base.hpp"

namespace sl::functional::detail
{
	struct nested_invoke_caller_fn
	{
		static constexpr composition_strategy_t composition_strategy{ composition_strategy_t::prefer_join };

		template <class TFunctionsTuple, class... TCallArgs>
		[[nodiscard]]
		constexpr decltype(auto) operator ()
		(
			TFunctionsTuple&& functionsTuple,
			TCallArgs&&... callArgs
		) const
		{
			return std::apply(
				[&]<std::invocable<TCallArgs...> TFunc, class... TOthers>(TFunc&& func, TOthers&&... otherFuncs) -> decltype(auto)
				{
					return recursive_invoke(
						std::invoke(std::forward<TFunc>(func), std::forward<TCallArgs>(callArgs)...),
						std::forward<TOthers>(otherFuncs)...
					);
				},
				std::forward<TFunctionsTuple>(functionsTuple)
			);
		}

	private:
		template <class TInput, std::invocable<TInput> TFunc, class... TOthers>
		[[nodiscard]]
		static constexpr decltype(auto) recursive_invoke
		(
			TInput&& input,
			TFunc&& func,
			TOthers&&... otherFunctions
		)
		{
			if constexpr (0 == sizeof...(TOthers))
			{
				return std::invoke(std::forward<TFunc>(func), std::forward<TInput>(input));
			}
			else
			{
				return recursive_invoke(
					std::invoke(std::forward<TFunc>(func), std::forward<TInput>(input)),
					std::forward<TOthers>(otherFunctions)...
				);
			}
		}
	};

	struct pipe_base_tag
	{};
}

namespace sl::functional
{
	/**
	 * \addtogroup GROUP_FUNCTIONAL
	 *
	 * @{
	 */

	/**
	 * \brief Helper type which enables pipe composing of two functional objects via operator |.
	 * \tparam TDerived The most derived class type.
	 * \tparam TClosureBase The base closure type (with one template argument left to be specified).
	 */
	template <class TDerived, template <class> class TClosureBase>
		requires std::is_class_v<TDerived> && std::same_as<TDerived, std::remove_cvref_t<TDerived>>
	struct pipe_operator
		: private unified_base<detail::pipe_base_tag>
	{
	private:
		using composer_t = detail::compose_helper_t<TClosureBase, detail::nested_invoke_caller_fn>;

	public:
		/**
		 * \brief Composes this and the other functional object as nested function
		 * (result of left-hand-side as argument for right-hand-side).
		 * \tparam TOther The type of the other functional object.
		 * \param other The right-hand-side functional object.
		 * \return The nested composition of this and other as new functional object.
		 */
		template <class TOther>
		[[nodiscard]]
		constexpr auto operator |
		(
			TOther&& other
		) const & noexcept(std::is_nothrow_invocable_v<composer_t, const TDerived&, TOther>)
		{
			return composer_t{}(static_cast<const TDerived&>(*this), std::forward<TOther>(other));
		}

		/**
		 * \copydoc operator|()
		 */
		template <class TOther>
		[[nodiscard]]
		constexpr auto operator |
		(
			TOther&& other
		) && noexcept(std::is_nothrow_invocable_v<composer_t, TDerived&&, TOther>)
		{
			return composer_t{}(static_cast<TDerived&&>(*this), std::forward<TOther>(other));
		}

		/**
		 * \brief Composes both functional objects as nested function (result of left-hand-side as argument for right-hand-side).
		 * \tparam TLhs The left-hand-side functional type.
		 * \param lhs The left-hand-side functional object.
		 * \param rhs The right-hand-side functional object.
		 * \return The nested composition of both functional objects as new functional object.
		 */
		template <class TLhs>
			requires (!derived_from_unified_base<TLhs, detail::pipe_base_tag>)
		[[nodiscard]]
		friend constexpr auto operator |
		(
			TLhs&& lhs,
			pipe_operator&& rhs
		)
		noexcept(std::is_nothrow_invocable_v<composer_t, TLhs, TDerived&&>)
		{
			return composer_t{}(std::forward<TLhs>(lhs), static_cast<TDerived&&>(rhs));
		}

		/**
		 * \copydoc operator|(TLhs&&, pipe_operator&&)
		 */
		template <class TLhs>
			requires (!derived_from_unified_base<TLhs, detail::pipe_base_tag>)
		[[nodiscard]]
		friend constexpr auto operator |
		(
			TLhs&& lhs,
			const pipe_operator& rhs
		)
		noexcept(std::is_nothrow_invocable_v<composer_t, TLhs, const TDerived&>)
		{
			return composer_t{}(std::forward<TLhs>(lhs), static_cast<const TDerived&>(rhs));
		}
	};

	/**
	 * \brief Helper type which enables unary front currying on functionals via operator <<.
	 * \tparam TDerived The most derived class type.
	 * \tparam TClosureBase The base closure type (with one template argument left to be specified).
	 */
	template <class TDerived, template <class> class TClosureBase>
		requires std::is_class_v<TDerived> && std::same_as<TDerived, std::remove_cvref_t<TDerived>>
	struct bind_front_operator
	{
	private:
		template <class TFunc, class TValue>
		using bind_front_fn = std::remove_cvref_t<decltype(std::bind_front(std::declval<TFunc>(), std::declval<TValue>()))>;

	public:
		/**
		 * \brief Curries the front parameter of this functional object.
		 * \tparam TValue The type of the curried value.
		 * \param value The value to be curried.
		 * \return The curried functional, as a new functional object.
		 */
		template <class TValue>
		[[nodiscard]]
		constexpr auto operator <<
		(
			TValue&& value
		) && noexcept(
			std::is_nothrow_constructible_v<TClosureBase<bind_front_fn<TDerived&&, TValue>>, bind_front_fn<TDerived&&, TValue>>
		)
		{
			return TClosureBase<bind_front_fn<TDerived&&, TValue>>{
				std::bind_front(static_cast<TDerived&&>(*this), std::forward<TValue>(value))
			};
		}

		/**
		 * \copydoc operator!()
		 */
		template <class TValue>
		[[nodiscard]]
		constexpr auto operator <<
		(
			TValue&& value
		) const & noexcept(
			std::is_nothrow_constructible_v<TClosureBase<bind_front_fn<const TDerived&, TValue>>,
											bind_front_fn<const TDerived&, TValue>
			>
		)
		{
			return TClosureBase<bind_front_fn<const TDerived&, TValue>>{
				std::bind_front(static_cast<const TDerived&>(*this), std::forward<TValue>(value))
			};
		}
	};

	/**
	 * \brief Helper type which enables unary back currying on functionals via operator >>.
	 * \tparam TDerived The most derived class type.
	 * \tparam TClosureBase The base closure type (with one template argument left to be specified).
	 */
	template <class TDerived, template <class> class TClosureBase>
		requires std::is_class_v<TDerived> && std::same_as<TDerived, std::remove_cvref_t<TDerived>>
	struct bind_back_operator
	{
	private:
		template <class TFunc, class TValue>
		using bind_back_fn = std::remove_cvref_t<decltype(bind_back(std::declval<TFunc>(), std::declval<TValue>()))>;

	public:
		/**
		 * \brief Curries the back parameter of this functional object.
		 * \tparam TValue The type of the curried value.
		 * \param value The value to be curried.
		 * \return The curried functional, as a new functional object.
		 */
		template <class TValue>
		[[nodiscard]]
		constexpr auto operator >>
		(
			TValue&& value
		) && noexcept(
			std::is_nothrow_constructible_v<TClosureBase<bind_back_fn<TDerived&&, TValue>>, bind_back_fn<TDerived&&, TValue>>
		)
		{
			return TClosureBase<bind_back_fn<TDerived&&, TValue>>{
				bind_back(static_cast<TDerived&&>(*this), std::forward<TValue>(value))
			};
		}

		/**
		 * \copydoc operator!()
		 */
		template <class TValue>
		[[nodiscard]]
		constexpr auto operator >>
		(
			TValue&& value
		) const & noexcept(
			std::is_nothrow_constructible_v<TClosureBase<bind_back_fn<const TDerived&, TValue>>,
											bind_back_fn<const TDerived&, TValue>
			>
		)
		{
			return TClosureBase<bind_back_fn<const TDerived&, TValue>>{
				bind_back(static_cast<const TDerived&>(*this), std::forward<TValue>(value))
			};
		}
	};

	/**
	 * \brief Helper type which accepts a functional type and enables pipe chaining.
	 * \tparam TFunc The functional type.
	 */
	template <class TFunc>
		requires std::same_as<TFunc, std::remove_cvref_t<TFunc>>
	class transform_fn
		: public closure_base_fn<TFunc>,
		public pipe_operator<transform_fn<TFunc>, transform_fn>,
		public bind_front_operator<transform_fn<TFunc>, transform_fn>,
		public bind_back_operator<transform_fn<TFunc>, transform_fn>
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
	transform_fn(TFunc) -> transform_fn<std::remove_cvref_t<TFunc>>;

	/**
	 * \defgroup GROUP_FUNCTIONAL_TRANSFORM transform
	 * @{
	 */

	/**
	 * \brief Functional object which static_cast the given argument to the target type on invocation.
	 * \tparam TTarget The target type.
	 */
	template <class TTarget>
	inline constexpr transform_fn as{
		[]<class T>(T&& v) -> TTarget { return static_cast<TTarget>(std::forward<T>(v)); }
	};

	/** @} */

	/** @} */
}

#endif
