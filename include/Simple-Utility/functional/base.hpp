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

#include "Simple-Utility/unified_base.hpp"
#include "Simple-Utility/concepts/stl_extensions.hpp"

namespace sl::functional::detail
{
	template <class TFunc1, class TFunc2, class TBinaryOp>
		requires std::same_as<TFunc1, std::remove_cvref_t<TFunc1>>
				&& std::same_as<TFunc2, std::remove_cvref_t<TFunc2>>
				&& std::same_as<TBinaryOp, std::remove_cvref_t<TBinaryOp>>
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

	struct composition_base_tag
	{};

	struct closure_base_tag
	{};
}

namespace sl::functional
{
	/**
	 * \defgroup GROUP_FUNCTIONAL functional
	 * \brief This namespace contains helper types for complex functional chaining and concrete functional objects.
	 * \details As working more and more with ``ranges`` and functional style approach, this library is designed to fully
	 * support the programmers in such cases.
	 *
	 * There are mainly two parts:
	 * - transform
	 * - predicate
	 *
	 * # transform
	 * The transform part of this library is designed to help the programmer when working with ``ranges`` or any other algorithm,
	 * which accepts a functional as part of its interface. There is often the need to do multiple steps, before the work is actually done.
	 * Because of this many existing functional objects are not usable by their own, thus they must be wrapped into a lambda, which
	 * results in quite a bit of noise. With objects of type ``transform_fn`` programmers have the option to pipe function results into
	 * other functions, and thus nest multiple functions into each other.
	 * \snippet functional/transform.cpp functional piped
	 * ``comp`` is equivalent to
	 * \code{.cpp}
	 * [](const int i) { return std::to_string(i + 42); };
	 * \endcode
	 *
	 * # predicates
	 * ``predicate_fn`` objects are fully compatible with ``transform_fn`` objects, thus they may be mixed as desired. Additionally they
	 * offer composing via operator && and ||. Other than piping, all such composed predicates will receive the identically input and
	 * are required to return boolean convertible results. Finally ``predicate_fn`` are easily invertible via operator !.
	 *
	 * \note The library classes are designed to only accept functional objects or pointers as value.
	 * This usually works fine but may lead to surprisingly effects when working with mutable lambdas or other functional types.
	 * In those cases the functional object itself may be wrapped into a ``std::reference_wrapper`` before putting into the
	 * closure object.
	 * \snippet functional/predicate.cpp predicate wrapped
	 * @{
	 */

	template <class TOperator, class... TFunctions>
		requires std::same_as<TOperator, std::remove_cvref_t<TOperator>>
				&& (1 < sizeof...(TFunctions))
				&& (std::same_as<TFunctions, std::remove_cvref_t<TFunctions>> && ...)
	class composition_fn
		: private unified_base<detail::composition_base_tag>
	{
	public:
		using function_storage_t = std::tuple<TFunctions...>;
		using operation_t = TOperator;

		template <concepts::initializes<operation_t> TOperationArg, class... TFunctionArgs>
			requires std::constructible_from<function_storage_t, TFunctionArgs...>
		[[nodiscard]]
		constexpr composition_fn
		(
			TOperationArg&& operationArg,
			TFunctionArgs&&... functionArgs
		)
		noexcept(std::is_nothrow_constructible_v<operation_t, TOperationArg>
				&& std::is_nothrow_constructible_v<function_storage_t, TFunctionArgs...>
		)
			: m_Operation{ std::forward<TOperationArg>(operationArg) },
			m_Functions{ std::forward<TFunctionArgs>(functionArgs)... }

		{}

		template <class... TCallArgs>
		[[nodiscard]]
		constexpr decltype(auto) operator ()
		(
			TCallArgs&&... callArgs
		) const & noexcept(
			std::is_nothrow_invocable_v<const operation_t&, const function_storage_t&, TCallArgs...>
		)
		{
			return std::invoke(m_Operation, m_Functions, std::forward<TCallArgs>(callArgs)...);
		}

		template <class... TCallArgs>
		[[nodiscard]]
		constexpr decltype(auto) operator ()
		(
			TCallArgs&&... callArgs
		) & noexcept(
			std::is_nothrow_invocable_v<operation_t&, function_storage_t&, TCallArgs...>
		)
		{
			return std::invoke(m_Operation, m_Functions, std::forward<TCallArgs>(callArgs)...);
		}

		template <class... TCallArgs>
		[[nodiscard]]
		constexpr decltype(auto) operator ()
		(
			TCallArgs&&... callArgs
		) && noexcept(
			std::is_nothrow_invocable_v<operation_t&&, function_storage_t&&, TCallArgs...>
		)
		{
			return std::invoke(std::move(m_Operation), std::move(m_Functions), std::forward<TCallArgs>(callArgs)...);
		}

		[[nodiscard]]
		constexpr function_storage_t operator *() const & noexcept(std::is_nothrow_copy_constructible_v<function_storage_t>)
		{
			return m_Functions;
		}

		[[nodiscard]]
		constexpr function_storage_t operator *() && noexcept(std::is_nothrow_move_constructible_v<function_storage_t>)
		{
			return std::move(m_Functions);
		}

	private:
		[[no_unique_address]]
		operation_t m_Operation{};
		function_storage_t m_Functions{};
	};

	template <class TOperator, class... TFunctions>
	composition_fn(TOperator, TFunctions ...) -> composition_fn<std::remove_cvref_t<TOperator>, std::remove_cvref_t<TFunctions>...>;

	/**
	 * \brief Helper type which simply stores functional objects or function pointers.
	 * \tparam TFunc The functional type.
	 */
	template <class TFunc>
		requires std::same_as<TFunc, std::remove_cvref_t<TFunc>>
	class closure_base_fn
		: private unified_base<detail::closure_base_tag>
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
		constexpr
		/**\cond conditional-explicit*/
		explicit(sizeof...(TArgs) == 1)
		/**\endcond*/
		closure_base_fn
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
		) const & noexcept(std::is_nothrow_invocable_v<const function_type&, TArgs...>)
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
		) & noexcept(std::is_nothrow_invocable_v<function_type&, TArgs...>)
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
		) && noexcept(std::is_nothrow_invocable_v<function_type&&, TArgs...>)
		{
			return std::invoke(std::move(m_Func), std::forward<TArgs>(args)...);
		}

		[[nodiscard]]
		constexpr function_type operator *() const & noexcept(std::is_nothrow_copy_constructible_v<function_type>)
		{
			return m_Func;
		}

		[[nodiscard]]
		constexpr function_type operator *() && noexcept(std::is_nothrow_move_constructible_v<function_type>)
		{
			return std::move(m_Func);
		}

	private:
		[[no_unique_address]]
		function_type m_Func{};
	};

	template <class T>
	using closure_function_t = typename std::remove_cvref_t<T>::function_type;

	/** @} */
}

namespace sl::functional::detail
{
	template <class T>
	struct unwrap_functional_result
	{
		using type = T;
	};

	template <derived_from_unified_base<closure_base_tag> T>
	struct unwrap_functional_result<T>
	{
		using type = typename unwrap_functional_result<closure_function_t<T>>::type;
	};

	template <class T>
	using unwrap_functional_result_t = typename unwrap_functional_result<T>::type;

	template <class TFunc>
	[[nodiscard]]
	constexpr unwrap_functional_result_t<TFunc> unwrap_functional(TFunc&& func) noexcept
	{
		return std::forward<TFunc>(func);
	}

	template <derived_from_unified_base<closure_base_tag> TClosure>
	[[nodiscard]]
	constexpr unwrap_functional_result_t<TClosure> unwrap_functional(TClosure&& closure) noexcept(noexcept(*std::declval<TClosure>()))
	{
		return unwrap_functional(*std::forward<TClosure>(closure));
	}

	template <template <class> class TClosureBase, class TOperation>
	struct composer
	{
		template <class TFunc1, class TFunc2>
		using closure_t = TClosureBase<
			composition_fn<
				TOperation,
				std::remove_cvref_t<unwrap_functional_result_t<TFunc1>>,
				std::remove_cvref_t<unwrap_functional_result_t<TFunc2>>
			>
		>;

		template <class TFunc1, class TFunc2>
		[[nodiscard]]
		static constexpr closure_t<TFunc1, TFunc2> compose
		(
			TFunc1&& func1,
			TFunc2&& func2
		)
		noexcept(
			std::is_nothrow_constructible_v<
				closure_t<TFunc1, TFunc2>,
				TOperation,
				unwrap_functional_result_t<TFunc1>,
				unwrap_functional_result_t<TFunc2>
			>)
		{
			return { TOperation{}, unwrap_functional(std::forward<TFunc1>(func1)), unwrap_functional(std::forward<TFunc2>(func2)) };
		}
	};

	template <class T>
	using composition_operation_t = typename std::remove_cvref_t<T>::operation_t;

	template <class T>
	concept associative_operation = std::remove_cvref_t<T>::is_associative_operation;;

	template <template <class> class TClosureBase, class TOperation, class TFunc1, class TFunc2>
	using composition_closure_t = TClosureBase<
		composition_fn<
			TOperation,
			std::remove_cvref_t<unwrap_functional_result_t<TFunc1>>,
			std::remove_cvref_t<unwrap_functional_result_t<TFunc2>>
		>
	>;

	template <template <class> class TClosureBase, class TOperation>
	struct binary_composer_t
	{
		template <class TFunc1, class TFunc2>
		using closure_t = composition_closure_t<TClosureBase, TOperation, TFunc1, TFunc2>;

		template <class TFunc1, class TFunc2>
		[[nodiscard]]
		static constexpr closure_t<TFunc1, TFunc2> compose
		(
			TFunc1&& func1,
			TFunc2&& func2
		) noexcept(
			std::is_nothrow_constructible_v<
				closure_t<TFunc1, TFunc2>,
				TOperation,
				unwrap_functional_result_t<TFunc1>,
				unwrap_functional_result_t<TFunc2>
			>)
		{
			return { TOperation{}, unwrap_functional(std::forward<TFunc1>(func1)), unwrap_functional(std::forward<TFunc2>(func2)) };
		}
	};

	template <template <class> class TClosureBase, associative_operation TOperation>
	struct binary_composer_t<TClosureBase, TOperation>
	{
		template <class TFunc1, class TFunc2>
		using closure_t = composition_closure_t<TClosureBase, TOperation, TFunc1, TFunc2>;

		template <class TFunc1, class TFunc2>
		//requires is_associative_operation_v<TOperation>
		//		&& derived_from_unified_base<TFunc1>
		//		&& std::same_as<composition_operation_t<TFunc1>, TOperation>
		//		&& (!derived_from_unified_base<TFunc2> || !std::same_as<composition_operation_t<TFunc1>, TOperation>)
		[[nodiscard]]
		static constexpr auto compose
		(
			TFunc1&& func1,
			TFunc2&& func2
		)
		{
			if constexpr (derived_from_unified_base<TFunc1, composition_base_tag> && std::same_as<composition_operation_t<TFunc1>, TOperation>)

			return { std::forward<TFunc1>(func1), std::forward<TFunc2>(func2) };
		}
	};

	template <class TComposer, class TFunc1, class TFunc2>
	inline constexpr bool is_nothrow_composable_v{
		noexcept(TComposer::compose(
			std::declval<unwrap_functional_result_t<TFunc1>>(),
			std::declval<unwrap_functional_result_t<TFunc2>>()
		))
	};
}

#endif
