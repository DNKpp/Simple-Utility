//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_TRANSFORM_HPP
#define SL_UTILITY_FUNCTIONAL_TRANSFORM_HPP

#pragma once

#include "Simple-Utility/functional/base.hpp"

namespace sl::functional::detail
{
	struct binary_nesting_fn
	{
		template <class TFunc1, class TFunc2, class... TArgs>
		[[nodiscard]]
		constexpr decltype(auto) operator ()
		(
			TFunc1&& func1,
			TFunc2&& func2,
			TArgs&&... v
		) const
		noexcept(std::is_nothrow_invocable_v<TFunc1, TArgs...>
				&& std::is_nothrow_invocable_v<TFunc2, std::invoke_result_t<TFunc1, TArgs...>>)
			requires std::invocable<TFunc1, TArgs...>
					&& std::invocable<TFunc2, std::invoke_result_t<TFunc1, TArgs...>>
		{
			return std::invoke(
				std::forward<TFunc2>(func2),
				std::invoke(std::forward<TFunc1>(func1), std::forward<TArgs>(v)...)
			);
		}
	};
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
	{
	private:
		using composer_t = detail::composer<TClosureBase, detail::binary_nesting_fn>;

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
		) && noexcept(detail::is_nothrow_composable_v<composer_t, TDerived&&, TOther>)
		{
			return composer_t::compose(static_cast<TDerived&&>(*this), std::forward<TOther>(other));
		}

		/**
		 * \copydoc operator|()
		 */
		template <class TOther>
		[[nodiscard]]
		constexpr auto operator |
		(
			TOther&& other
		) const & noexcept(detail::is_nothrow_composable_v<composer_t, const TDerived&, TOther>)
		{
			return composer_t::compose(static_cast<const TDerived&>(*this), std::forward<TOther>(other));
		}

		/**
		 * \brief Composes both functional objects as nested function (result of left-hand-side as argument for right-hand-side).
		 * \tparam TLhs The left-hand-side functional type.
		 * \param lhs The left-hand-side functional object.
		 * \param rhs The right-hand-side functional object.
		 * \return The nested composition of both functional objects as new functional object.
		 */
		template <class TLhs>
		[[nodiscard]]
		friend constexpr auto operator |
		(
			TLhs&& lhs,
			pipe_operator&& rhs
		)
		noexcept(detail::is_nothrow_composable_v<composer_t, TLhs, TDerived&&>)
			requires (!requires { lhs.operator|(std::move(rhs)); })

		{
			return composer_t::compose(std::forward<TLhs>(lhs), static_cast<TDerived&&>(rhs));
		}

		/**
		 * \copydoc operator|(TLhs&&, pipe_operator&&)
		 */
		template <class TLhs>
		[[nodiscard]]
		friend constexpr auto operator |
		(
			TLhs&& lhs,
			const pipe_operator& rhs
		)
		noexcept(detail::is_nothrow_composable_v<composer_t, TLhs, const TDerived&>)
			requires (!requires { lhs.operator|(rhs); })
		{
			return composer_t::compose(std::forward<TLhs>(lhs), static_cast<const TDerived&>(rhs));
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
		public pipe_operator<transform_fn<TFunc>, transform_fn>
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
	 * \brief Functional object which static_cast the given argument to the target type on invocation.
	 * \tparam TTarget The target type.
	 */
	template <class TTarget>
	inline constexpr transform_fn as{
		[]<class T>(T&& v) -> TTarget { return static_cast<TTarget>(std::forward<T>(v)); }
	};

	/**
	 * \brief Functional object which retrieves an object of a specific type from a tuple-like argument.
	 * \tparam T The type to be retrieved.
	 */
	template <class T>
	inline constexpr transform_fn get{
		[]<class TTuple>(TTuple&& v) -> decltype(auto)
		{
			using std::get;
			return get<T>(std::forward<TTuple>(v));
		}
	};

	/**
	 * \brief Functional object which retrieves an object at a specific index from a tuple-like argument.
	 * \tparam VIndex The index of type to be retrieved.
	 */
	template <std::size_t VIndex>
	inline constexpr transform_fn get_at{
		[]<class TTuple>(TTuple&& v) -> decltype(auto)
		{
			using std::get;
			return get<VIndex>(std::forward<TTuple>(v));
		}
	};

	/** @} */
}

#endif
