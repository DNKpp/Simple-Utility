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
	struct unwrap_functional_r
	{
		using type = T;
	};

	template <derived_from_unified_base<closure_base_tag> T>
	struct unwrap_functional_r<T>
	{
		using type = typename unwrap_functional_r<closure_function_t<T>>::type;
	};

	template <class T>
	using unwrap_functional_r_t = typename unwrap_functional_r<T>::type;

	template <class TFunc>
	[[nodiscard]]
	constexpr unwrap_functional_r_t<TFunc> unwrap_functional(TFunc&& func) noexcept
	{
		return std::forward<TFunc>(func);
	}

	template <derived_from_unified_base<closure_base_tag> TClosure>
	[[nodiscard]]
	constexpr unwrap_functional_r_t<TClosure> unwrap_functional(TClosure&& closure) noexcept(noexcept(*std::declval<TClosure>()))
	{
		return unwrap_functional(*std::forward<TClosure>(closure));
	}
}

namespace sl::functional
{
	template <class TOperator, class... TFunctions>
		requires std::same_as<TOperator, std::remove_cvref_t<TOperator>>
				&& (1 < sizeof...(TFunctions))
				&& (std::same_as<TFunctions, std::remove_cvref_t<TFunctions>> && ...)
				&& (!derived_from_unified_base<TFunctions, detail::closure_base_tag> && ...)
	class composition_fn
		: private unified_base<detail::composition_base_tag>
	{
	public:
		using function_storage_t = std::tuple<TFunctions...>;
		using operation_t = TOperator;

		template <concepts::initializes<operation_t> TOperationArg, class... TFunctionArgs>
			requires std::constructible_from<function_storage_t, detail::unwrap_functional_r_t<TFunctionArgs>...>
		[[nodiscard]]
		constexpr composition_fn
		(
			TOperationArg&& operationArg,
			TFunctionArgs&&... functionArgs
		)
		noexcept(std::is_nothrow_constructible_v<operation_t, TOperationArg>
				&& (std::is_nothrow_constructible_v<TFunctions, detail::unwrap_functional_r_t<TFunctionArgs>> && ...)
		)
			: m_Operation{ std::forward<TOperationArg>(operationArg) },
			m_Functions{ detail::unwrap_functional(std::forward<TFunctionArgs>(functionArgs))... }

		{}

		template <class... TCallArgs>
		[[nodiscard]]
		constexpr decltype(auto) operator ()
		(
			TCallArgs&&... callArgs
		) const & noexcept(std::is_nothrow_invocable_v<const operation_t&, const function_storage_t&, TCallArgs...>)
		{
			return std::invoke(m_Operation, m_Functions, std::forward<TCallArgs>(callArgs)...);
		}

		template <class... TCallArgs>
		[[nodiscard]]
		constexpr decltype(auto) operator ()
		(
			TCallArgs&&... callArgs
		) & noexcept(std::is_nothrow_invocable_v<operation_t&, function_storage_t&, TCallArgs...>)
		{
			return std::invoke(m_Operation, m_Functions, std::forward<TCallArgs>(callArgs)...);
		}

		template <class... TCallArgs>
		[[nodiscard]]
		constexpr decltype(auto) operator ()
		(
			TCallArgs&&... callArgs
		) && noexcept(std::is_nothrow_invocable_v<operation_t&&, function_storage_t&&, TCallArgs...>)
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

	/**
	 * \brief Deduction guide.
	 * \tparam TOperator Type of the given operation type.
	 * \tparam TFunctions Type of the given functions type.
	 */
	template <class TOperator, class... TFunctions>
	composition_fn(TOperator, TFunctions ...) -> composition_fn<TOperator, detail::unwrap_functional_r_t<TFunctions>...>;

	template <class T>
		requires std::same_as<T, std::remove_cvref_t<T>>
	class value_fn
	{
	public:
		using value_type = std::conditional_t<std::is_array_v<T>, T*, T>;
		using reference_type = const std::unwrap_reference_t<value_type>&;

		template <class... TArgs>
		[[nodiscard]]
		explicit (1 == sizeof...(TArgs))
		constexpr value_fn(TArgs&&... args) noexcept(std::is_nothrow_constructible_v<value_type, TArgs...>)
			: m_Value{ std::forward<TArgs>(args)... }
		{}

		[[nodiscard]]
		constexpr reference_type operator ()() const & noexcept(std::is_nothrow_convertible_v<reference_type, value_type>)
		{
			return m_Value;
		}

		[[nodiscard]]
		constexpr value_type operator ()() && noexcept(std::is_nothrow_move_constructible_v<value_type>)
		{
			return std::move(m_Value);
		}

	private:
		value_type m_Value{};
	};

	/**
	 * \brief Deduction guide.
	 * \tparam TValue Type of the given value.
	 */
	template <class TValue>
	value_fn(TValue) -> value_fn<TValue>;
}

namespace sl::functional::detail
{
	template <class TFunction>
	struct is_nothrow_constructible_helper
		: std::bool_constant<
			std::is_nothrow_copy_constructible_v<unwrap_functional_r_t<TFunction>>
			&& std::is_nothrow_move_constructible_v<unwrap_functional_r_t<TFunction>>
		>
	{};

	template <class TOperation, class... TFunctions>
	struct is_nothrow_constructible_helper<composition_fn<TOperation, TFunctions...>>
		: std::bool_constant<std::conjunction_v<is_nothrow_constructible_helper<TFunctions>...>>
	{};

	template <class TOperation, class... TFunctions>
	inline constexpr bool is_nothrow_composable_v{
		std::is_nothrow_constructible_v<TOperation>
		&& (is_nothrow_constructible_helper<std::remove_cvref_t<TFunctions>>::value && ...)
	};

	template <class T, class... TCallArgs>
	struct is_type_list_nothrow_invokable;

	template <class... TFunctions, class... TCallArgs>
	struct is_type_list_nothrow_invokable<std::tuple<TFunctions...>, TCallArgs...>
		: std::bool_constant<std::conjunction_v<std::is_nothrow_invocable<TFunctions>...>>
	{};

	template <class T, class... TCallArgs>
	inline constexpr bool is_type_list_nothrow_invokable_v{
		is_type_list_nothrow_invokable<std::remove_cvref_t<T>, TCallArgs...>::value
	};
}

namespace sl::functional::operators::detail
{
	using functional::detail::unwrap_functional;

	enum class composition_strategy_t
	{
		nested_only,
		prefer_join
	};

	template <class T>
	concept joinable_operation = std::remove_cvref_t<T>::composition_strategy == composition_strategy_t::prefer_join;

	template <class T, class TJoinableOperation>
	concept joinable_with = joinable_operation<TJoinableOperation>
							&& derived_from_unified_base<T, functional::detail::composition_base_tag>
							&& std::same_as<typename std::remove_cvref_t<T>::operation_t, TJoinableOperation>;

	template <class TOperation, class... TTupleElements>
	[[nodiscard]]
	constexpr auto make_composition_from_tuple
	(
		std::tuple<TTupleElements...>&& functionsTuple
	) noexcept(functional::detail::is_nothrow_composable_v<TOperation, TTupleElements...> )
	{
		return std::apply(
			[]<class... TFunctions>(TFunctions&&... functions)
			{
				return composition_fn{ TOperation{}, std::forward<TFunctions>(functions)... };
			},
			std::move(functionsTuple)
		);
	}

	template <class TOperation, class TFunc1, class TFunc2>
		requires (!joinable_with<TFunc1, TOperation>
				&& !joinable_with<TFunc2, TOperation>)
	[[nodiscard]]
	constexpr auto make_composition(TFunc1&& func1, TFunc2&& func2)
	{
		return composition_fn{ TOperation{}, std::forward<TFunc1>(func1), std::forward<TFunc2>(func2) };
	}

	template <class TOperation, joinable_with<TOperation> TFunc1, joinable_with<TOperation> TFunc2>
	[[nodiscard]]
	constexpr auto make_composition(TFunc1&& func1, TFunc2&& func2)
	{
		return make_composition_from_tuple<TOperation>(
			std::tuple_cat(
				*std::forward<TFunc1>(func1),
				*std::forward<TFunc2>(func2)
			)
		);
	}

	template <class TOperation, joinable_with<TOperation> TFunc1, class TFunc2>
		requires (!joinable_with<TFunc2, TOperation>)
	[[nodiscard]]
	constexpr auto make_composition(TFunc1&& func1, TFunc2&& func2)
	{
		return make_composition_from_tuple<TOperation>(
			std::tuple_cat(
				*std::forward<TFunc1>(func1),
				std::tuple<TFunc2>(std::forward<TFunc2>(func2))
			)
		);
	}

	template <class TOperation, class TFunc1, joinable_with<TOperation> TFunc2>
		requires (!joinable_with<TFunc1, TOperation>)
	[[nodiscard]]
	constexpr auto make_composition(TFunc1&& func1, TFunc2&& func2)
	{
		return make_composition_from_tuple<TOperation>(
			std::tuple_cat(
				std::tuple<TFunc1>(std::forward<TFunc1>(func1)),
				*std::forward<TFunc2>(func2)
			)
		);
	}

	template <template <class> class TClosureBase, class TOperation>
	struct compose_helper_t
	{
		template <class TFunc1, class TFunc2>
		[[nodiscard]]
		constexpr auto operator ()
		(
			TFunc1&& func1,
			TFunc2&& func2
		) const noexcept(functional::detail::is_nothrow_composable_v<TOperation, TFunc1, TFunc2>)
		{
			using composition_t = decltype(make_composition<TOperation>(
				unwrap_functional(std::forward<TFunc1>(func1)),
				unwrap_functional(std::forward<TFunc2>(func2))
			));
			return TClosureBase<composition_t>{
				make_composition<TOperation>(
					unwrap_functional(std::forward<TFunc1>(func1)),
					unwrap_functional(std::forward<TFunc2>(func2))
				)
			};
		}
	};
}

#endif
