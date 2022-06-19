//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_BASE_HPP
#define SL_UTILITY_FUNCTIONAL_BASE_HPP

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
		) const & noexcept(std::is_nothrow_invocable_v<const TBinaryOp&, const TFunc1&, const TFunc2&, TArgs...>)
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
		) & noexcept(std::is_nothrow_invocable_v<TBinaryOp&, TFunc1&, TFunc2&, TArgs...>)
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
		) && noexcept(std::is_nothrow_invocable_v<TBinaryOp&&, TFunc1&&, TFunc2&&, TArgs...>)
		{
			return std::invoke(
				std::move(m_BinaryOperation),
				std::move(m_Func1),
				std::move(m_Func2),
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
}

namespace sl::functional
{
	template <class TFunc>
	class closure_base_fn
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
		explicit (sizeof...(TArgs) == 1) constexpr closure_base_fn
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
		) const &
		noexcept(std::is_nothrow_invocable_v<const function_type&, TArgs...>)
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
		) &
		noexcept(std::is_nothrow_invocable_v<function_type&, TArgs...>)
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
		) &&
		noexcept(std::is_nothrow_invocable_v<function_type&&, TArgs...>)
		{
			return std::invoke(std::move(m_Func), std::forward<TArgs>(args)...);
		}

	private:
		[[no_unique_address]]
		function_type m_Func{};
	};
}

namespace sl::functional::detail
{
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

	template <class TComposer, class TFunc1, class TFunc2>
	inline constexpr bool is_nothrow_composable_v{ noexcept(TComposer::compose(std::declval<TFunc1>(), std::declval<TFunc2>())) };
}

#endif
