//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_TRANSFORM_HPP
#define SL_UTILITY_FUNCTIONAL_TRANSFORM_HPP

#pragma once

#include "Simple-Utility/functional/detail.hpp"

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
		noexcept(noexcept(std::invoke(
			std::forward<TFunc2>(func2),
			std::invoke(std::forward<TFunc1>(func1), std::forward<TArgs>(v)...)
		)))
		{
			return std::invoke(
				std::forward<TFunc2>(func2),
				std::invoke(std::forward<TFunc1>(func1), std::forward<TArgs>(v)...)
			);
		}
	};

	template <class TDerived, template <class> class TClosureBase>
	struct pipe_op
	{
	private:
		using composer_t = composer<TClosureBase, binary_nesting_fn>;

	public:
		template <class TOther>
		[[nodiscard]]
		constexpr auto operator |
		(
			TOther&& other
		) && noexcept(noexcept(composer_t::compose(std::declval<TDerived&&>(), std::declval<TOther>())))
		{
			return composer_t::compose(static_cast<TDerived&&>(*this), std::forward<TOther>(other));
		}

		template <class TOther>
		[[nodiscard]]
		constexpr auto operator |
		(
			TOther&& other
		) const & noexcept(noexcept(composer_t::compose(std::declval<const TDerived&>(), std::declval<TOther>())))
		{
			return composer_t::compose(static_cast<const TDerived&>(*this), std::forward<TOther>(other));
		}

		template <class TLhs>
		[[nodiscard]]
		friend constexpr auto operator |
		(
			TLhs&& lhs,
			pipe_op&& rhs
		)
		noexcept(noexcept(composer_t::compose(std::declval<TLhs>(), std::declval<TDerived&&>())))
			requires (!requires { lhs.operator|(std::move(rhs)); })

		{
			return composer_t::compose(std::forward<TLhs>(lhs), static_cast<TDerived&&>(rhs));
		}

		template <class TLhs>
		[[nodiscard]]
		friend constexpr auto operator |
		(
			TLhs&& lhs,
			const pipe_op& rhs
		)
		noexcept(noexcept(composer_t::compose(std::declval<TLhs>(), std::declval<const TDerived&>())))
			requires (!requires { lhs.operator|(rhs); })
		{
			return composer_t::compose(std::forward<TLhs>(lhs), static_cast<const TDerived&>(rhs));
		}
	};
}

namespace sl::functional
{
	/**
	 * \brief Helper type which accepts a functional type and enables pipe chaining.
	 * \tparam TFunc The functional type.
	 */
	template <class TFunc>
	class transform_fn
		: public detail::closure_fn<std::remove_cvref_t<TFunc>>,
		public detail::pipe_op<transform_fn<TFunc>, transform_fn>
	{
		using closure_fn = detail::closure_fn<std::remove_cvref_t<TFunc>>;
	public:
		using closure_fn::closure_fn;
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
}

#endif
