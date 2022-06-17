//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_HPP
#define SL_UTILITY_FUNCTIONAL_HPP

#pragma once

#include <concepts>
#include <functional>
#include <type_traits>

namespace sl::functional::detail
{
	template <class TFunc1, class TFunc2, class TBinaryOp>
	class binary_composition_fn
	{
	public:
		using binary_operation_type = TBinaryOp;
		using function1_type = TFunc1;
		using function2_type = TFunc2;

		template <class TFunc1Arg, class TFunc2Arg, class TBinaryOpArg = TBinaryOp>
		[[nodiscard]]
		constexpr binary_composition_fn
		(
			TFunc1Arg&& func1Arg,
			TFunc2Arg&& func2Arg,
			TBinaryOpArg&& binaryOpArg = {}
		)
		noexcept(std::is_nothrow_constructible_v<TFunc1, TFunc1Arg>
				&& std::is_nothrow_constructible_v<TFunc2, TFunc2Arg>
				&& std::is_nothrow_constructible_v<TBinaryOp, TBinaryOpArg>)
			: m_Func1{ std::forward<TFunc1Arg>(func1Arg) },
			m_Func2{ std::forward<TFunc2Arg>(func2Arg) },
			m_BinaryOperation{ std::forward<TBinaryOpArg>(binaryOpArg) }
		{}

		template <class... TArgs>
		[[nodiscard]]
		constexpr decltype(auto) operator ()
		(
			TArgs&&... v
		) const
		noexcept(noexcept(std::invoke(
			std::declval<const TBinaryOp&>(),
			std::declval<const TFunc1&>(),
			std::declval<const TFunc2&>(),
			std::forward<TArgs>(v)...
		)))
		{
			return std::invoke(
				m_BinaryOperation,
				m_Func1,
				m_Func2,
				std::forward<TArgs>(v)...
			);
		}

		template <class... TArgs>
		[[nodiscard]]
		constexpr decltype(auto) operator ()
		(
			TArgs&&... v
		)
		noexcept(noexcept(std::invoke(
			std::declval<TBinaryOp&>(),
			std::declval<TFunc1&>(),
			std::declval<TFunc2&>(),
			std::forward<TArgs>(v)...
		)))
		{
			return std::invoke(
				m_BinaryOperation,
				m_Func1,
				m_Func2,
				std::forward<TArgs>(v)...
			);
		}

	private:
		[[no_unique_address]]
		TFunc1 m_Func1{};
		[[no_unique_address]]
		TFunc2 m_Func2{};
		[[no_unique_address]]
		TBinaryOp m_BinaryOperation{};
	};

	template <class TFunc1, class TFunc2, class TBinaryOp>
	binary_composition_fn
	(
		TFunc1,
		TFunc2,
		TBinaryOp
	) -> binary_composition_fn<std::remove_cvref_t<TFunc1>, std::remove_cvref_t<TFunc2>, std::remove_cvref_t<TBinaryOp>>;

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

	struct binary_conjunction_fn
	{
		template <class TFunc1, class TFunc2, class... TArgs>
		[[nodiscard]]
		constexpr decltype(auto) operator ()
		(
			TFunc1&& func1,
			TFunc2&& func2,
			const TArgs&... v
		) const
		noexcept(noexcept(std::invoke(std::forward<TFunc1>(func1), v...))
				&& noexcept(std::invoke(std::forward<TFunc2>(func2), v...))
		)
			requires std::predicate<TFunc1, TArgs...>
					&& std::predicate<TFunc2, TArgs...>
		{
			return std::invoke(std::forward<TFunc1>(func1), v...)
					&& std::invoke(std::forward<TFunc2>(func2), v...);
		}
	};

	template <class TFunc>
	class closure_fn
	{
	public:
		using function_type = TFunc;

		/**
		 * \brief Forwards the constructor arguments to the internal functional object.
		 * \tparam TArgs The constructor argument types.
		 * \param args The constructor arguments.
		 */
		template <class... TArgs>
			requires std::constructible_from<function_type, TArgs...>
		[[nodiscard]]
		explicit (sizeof...(TArgs) == 1) constexpr closure_fn
		(
			TArgs&&... args
		)
		noexcept(std::is_nothrow_constructible_v<function_type, TArgs...>)
			: m_Func{ std::forward<TArgs>(args)... }
		{}

		/**
		 * \brief Invokes the internal functional with the given arguments.
		 * \tparam TArgs The argument types.
		 * \param args The arguments.
		 * \return Returns as received by invocation.
		 */
		template <class... TArgs>
		[[nodiscard]]
		constexpr decltype(auto) operator ()
		(
			TArgs&&... args
		) const noexcept(noexcept(std::invoke(std::declval<const function_type&>(), args...)))
		{
			return std::invoke(m_Func, std::forward<TArgs>(args)...);
		}

		/**
		 * \copydoc operator()()
		 */
		template <class... TArgs>
		[[nodiscard]]
		constexpr decltype(auto) operator ()
		(
			TArgs&&... args
		) noexcept(noexcept(std::invoke(std::declval<function_type&>(), args...)))
		{
			return std::invoke(m_Func, std::forward<TArgs>(args)...);
		}

	private:
		[[no_unique_address]]
		function_type m_Func{};
	};

	template <template <class> class TClosureBase, class TBinaryOp>
	struct composer
	{
		template <class TFunc1, class TFunc2>
		using closure_base_fn = TClosureBase<binary_composition_fn<
				std::remove_cvref_t<TFunc1>, std::remove_cvref_t<TFunc2>, TBinaryOp>
		>;

		template <class TFunc1, class TFunc2>
		[[nodiscard]]
		static constexpr closure_base_fn<TFunc1, TFunc2> compose
		(
			TFunc1 func1,
			TFunc2 func2
		)
		noexcept(std::is_nothrow_constructible_v<closure_base_fn<TFunc1, TFunc2>, TFunc1, TFunc2>)
		{
			return { std::forward<TFunc1>(func1), std::forward<TFunc2>(func2) };
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

	template <class TDerived, template <class> class TClosureBase>
	struct conjunction_op
	{
		template <class TFunc1, class TFunc2>
		using closure_base_fn = TClosureBase<binary_composition_fn<
				std::remove_cvref_t<TFunc1>, std::remove_cvref_t<TFunc2>, binary_conjunction_fn>
		>;

		template <class TOther>
		[[nodiscard]]
		constexpr closure_base_fn<TDerived, TOther> operator &&
		(
			TOther&& other
		) && noexcept(std::is_nothrow_constructible_v<closure_base_fn<TDerived, TOther>, TDerived&&, TOther>)
		{
			return { static_cast<TDerived&&>(*this), std::forward<TOther>(other) };
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
