//          Copyright Dominic Koepke 2019 - 2023.
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
#include "Simple-Utility/concepts/utility.hpp"

namespace sl::functional::detail
{
	struct composition_base_tag
	{
	};

	struct closure_base_tag
	{
	};

	struct enable_operators_base_tag
	{
	};

	template <class TTo, class... TArgs>
	inline constexpr bool force_explicit_constructor_v{false};

	template <class TTo, class T>
	inline constexpr bool force_explicit_constructor_v<TTo, T>{!std::is_convertible_v<T, TTo>};
}

namespace sl::functional::operators
{
	enum class composition_strategy_t
	{
		nested,
		join
	};

	template <class T>
	struct tag_traits;

	template <class T>
	concept tag = std::is_class_v<T>
				&& std::same_as<T, std::remove_cvref_t<T>>
				&& requires
				{
					typename tag_traits<T>::operation_t;
					{ tag_traits<T>::composition_strategy } -> std::convertible_to<composition_strategy_t>;
				};

	template <tag T>
	using tag_operation_t = typename tag_traits<T>::operation_t;

	template <tag T>
	inline constexpr composition_strategy_t tag_composition_strategy_v{tag_traits<T>::composition_strategy};
}

namespace sl::functional
{
	/**
	 * \defgroup GROUP_FUNCTIONAL functional
	 * \brief This namespace contains helper types for complex functional chaining and concrete functional objects.
	 * \details As working more and more with ``ranges`` and functional style approach, this library is designed to fully
	 * support the programmers in such cases.
	 *
	 * This namespaces aims to simplify caller code, when supplying algorithm with user provided function objects. Therefore several
	 * types are offered, which enable different composition modes.
	 *
	 * # transform
	 * The transform part of this library is designed to help the programmer when working with ``ranges`` or any other algorithm,
	 * which accepts a functional as part of its interface. There is often the need to do multiple steps before the work is actually done.
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
	 * offer composing via operator &&, || and other common operations. Other than piping, all such composed predicates will receive the
	 * identically input and are required to return boolean convertible results. Finally ``predicate_fn`` are easily invertible via operator !.
	 *
	 * # currying
	 * When working in functional style, there also often exists the desire to curry existing function objects, before giving them out of hands.
	 * This library supports this currently in two styles:
	 * - front currying via operator << (points to the front)
	 * - back currying via operator << (points to the back)
	 *
	 * Users are free to use them both on the same functional, but this will soon lead to non-obvious behaviour when mixing too arbitrarily. The
	 * library tries to keep the call stack as flat as possible and therefore combines multiple binds with equally directions into one final
	 * composition. When mixing them up like
	 * \code{.cpp}
	 * myTransform << 42 >> 1337 << "Hello, World!" >> 3.1415;
	 * \endcode
	 * the result will be not as good as it could. The code will still work; a semantically equal but a better optimized (and readable) solution
	 * would be something like this:
	 * \code{.cpp}
	 * myTransform << 42 << "Hello, World!" >> 3.1415 >> 1337;
	 * \endcode
	 * Note how the ordering of the two back-bound-values changed (which is the ordering readers understand probably best).
	 *
	 * Values will usually be bound by value, but users may wrap their values into ``std::reference_wrapper``, which will then be unwrapped during
	 * invocation and thus provided as (const) lvalue-reference to the invoked function(s).
	 * \snippet functional/base.cpp value_fn wrapped
	 *
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
		explicit(detail::force_explicit_constructor_v<function_type, TArgs...>)
		/**\endcond*/
		closure_base_fn(
			TArgs&&... args
		) noexcept(std::is_nothrow_constructible_v<function_type, TArgs...>)
			: m_Func{std::forward<TArgs>(args)...}
		{
		}

		/**
		 * \brief Invokes the internal functional with the given arguments.
		 * \tparam TArgs The argument types.
		 * \param args The arguments.
		 * \return Returns as received by invocation.
		 */
		template <class... TArgs>
			requires std::invocable<const function_type&, TArgs...>
		[[nodiscard]]
		constexpr decltype(auto) operator ()(
			TArgs&&... args
		) const & noexcept(std::is_nothrow_invocable_v<const function_type&, TArgs...>)
		{
			return std::invoke(m_Func, std::forward<TArgs>(args)...);
		}

		/**
		 * \copydoc operator()()
		 */
		template <class... TArgs>
			requires std::invocable<function_type&, TArgs...>
		[[nodiscard]]
		constexpr decltype(auto) operator ()(
			TArgs&&... args
		) & noexcept(std::is_nothrow_invocable_v<function_type&, TArgs...>)
		{
			return std::invoke(m_Func, std::forward<TArgs>(args)...);
		}

		/**
		 * \copydoc operator()()
		 */
		template <class... TArgs>
			requires std::invocable<function_type&&, TArgs...>
		[[nodiscard]]
		constexpr decltype(auto) operator ()(
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

	/**
	 * \brief Deduction guide.
	 * \tparam TFunc Type of the given value.
	 */
	template <class TFunc>
	closure_base_fn(TFunc) -> closure_base_fn<TFunc>;

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

	template <class TCurOperation, class TFunctionsTuple, class... TCallArgs>
	[[nodiscard]]
	static constexpr decltype(auto) call_operation(
		TCurOperation&& op,
		TFunctionsTuple&& functionsTuple,
		TCallArgs&&... callArgs
	)
	{
		return std::apply(
			[&]<class... UFunctions>(UFunctions&&... functions)
			{
				return std::invoke(
					std::forward<TCurOperation>(op),
					std::forward_as_tuple(std::forward<TCallArgs>(callArgs)...),
					std::forward<UFunctions>(functions)...
				);
			},
			std::forward<TFunctionsTuple>(functionsTuple)
		);
	}
}

namespace sl::functional
{
	/**
	 * \addtogroup GROUP_FUNCTIONAL
	 * @{
	 */

	/**
	 * \brief Functional type, which composes multiple functional objects via the given operation.
	 * \tparam TOperation The composing operation type.
	 * \tparam TFunctions The composed function types.
	 */
	template <class TOperation, class... TFunctions>
		requires std::same_as<TOperation, std::remove_cvref_t<TOperation>>
				&& (0 < sizeof...(TFunctions))
				&& (std::same_as<TFunctions, std::remove_cvref_t<TFunctions>> && ...)
				&& (!derived_from_unified_base<TFunctions, detail::closure_base_tag> && ...)
	class composition_fn
		: private unified_base<detail::composition_base_tag>
	{
	public:
		using function_storage_t = std::tuple<TFunctions...>;
		using operation_t = TOperation;

		template <concepts::initializes<operation_t> TOperationArg, class... TFunctionArgs>
			requires std::constructible_from<function_storage_t, detail::unwrap_functional_r_t<TFunctionArgs>...>
		[[nodiscard]]
		constexpr composition_fn(
			TOperationArg&& operationArg,
			TFunctionArgs&&... functionArgs
		)
			noexcept(std::is_nothrow_constructible_v<operation_t, TOperationArg>
					&& (std::is_nothrow_constructible_v<TFunctions, detail::unwrap_functional_r_t<TFunctionArgs>> && ...)
			)
			: m_Operation{std::forward<TOperationArg>(operationArg)},
			m_Functions{detail::unwrap_functional(std::forward<TFunctionArgs>(functionArgs))...}

		{
		}

		template <class... TCallArgs>
			requires std::invocable<const operation_t&, std::tuple<TCallArgs...>, const TFunctions&...>
		[[nodiscard]]
		constexpr decltype(auto) operator ()(
			TCallArgs&&... callArgs
		) const & noexcept(std::is_nothrow_invocable_v<const operation_t&, std::tuple<TCallArgs...>, const TFunctions&...>)
		{
			return detail::call_operation(m_Operation, m_Functions, std::forward<TCallArgs>(callArgs)...);
		}

		template <class... TCallArgs>
			requires std::invocable<operation_t&, std::tuple<TCallArgs...>, TFunctions&...>
		[[nodiscard]]
		constexpr decltype(auto) operator ()(
			TCallArgs&&... callArgs
		) & noexcept(std::is_nothrow_invocable_v<operation_t&, std::tuple<TCallArgs...>, TFunctions&...>)
		{
			return detail::call_operation(m_Operation, m_Functions, std::forward<TCallArgs>(callArgs)...);
		}

		template <class... TCallArgs>
			requires std::invocable<operation_t&&, std::tuple<TCallArgs...>, TFunctions&&...>
		[[nodiscard]]
		constexpr decltype(auto) operator ()(
			TCallArgs&&... callArgs
		) && noexcept(std::is_nothrow_invocable_v<operation_t&&, std::tuple<TCallArgs...>, TFunctions&&...>)
		{
			return detail::call_operation(std::move(m_Operation), std::move(m_Functions), std::forward<TCallArgs>(callArgs)...);
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
	 * \tparam TOperation Type of the given operation type.
	 * \tparam TFunctions Type of the given functions type.
	 */
	template <class TOperation, class... TFunctions>
	composition_fn(TOperation, TFunctions...) -> composition_fn<TOperation, detail::unwrap_functional_r_t<TFunctions>...>;
}

namespace sl::functional::operators::detail
{
	template <class T>
	concept joinable_operation = tag<T>
								&& tag_composition_strategy_v<T> == composition_strategy_t::join;

	template <class T, class TJoinableOperation>
	concept joinable_with = joinable_operation<TJoinableOperation>
							&& derived_from_unified_base<T, functional::detail::composition_base_tag>
							&& std::same_as<typename std::remove_cvref_t<T>::operation_t, tag_operation_t<TJoinableOperation>>;
}

namespace sl::functional::detail
{
	template <class TFunction>
	struct is_nothrow_constructible_helper
		: std::bool_constant<
			std::is_nothrow_copy_constructible_v<unwrap_functional_r_t<TFunction>>
			&& std::is_nothrow_move_constructible_v<unwrap_functional_r_t<TFunction>>
		>
	{
	};

	template <class TOperation, class... TFunctions>
	struct is_nothrow_constructible_helper<composition_fn<TOperation, TFunctions...>>
		: std::bool_constant<std::conjunction_v<is_nothrow_constructible_helper<TFunctions>...>>
	{
	};
}

namespace sl::functional
{
	template <operators::tag TOperationTag, class... TFunctions>
	inline constexpr bool is_nothrow_composable_v{
		std::is_nothrow_constructible_v<operators::tag_operation_t<TOperationTag>>
		&& (detail::is_nothrow_constructible_helper<std::remove_cvref_t<TFunctions>>::value && ...)
	};

	/**
	 * \brief Functional helper type, which takes a value and returns them on invocation.
	 * \details The general design allows only immutable access to the stored value, but when working with rvalue references,
	 * one may retrieve a mutable rvalue reference. If a function needs a mutable reference, users may explicitly wrap their values
	 * in a ``std::reference_wrapper``, but then they must follow the lifetime rules of these.
	 * \note In general the ``value_type`` will be equally to ``T``, but if ``T`` is an array type, it will decayed to (possibly const qualified) ``T*``.
	 * \tparam T The decayed value type.
	 */
	template <class T>
		requires std::same_as<T, std::decay_t<T>>
	class value_fn
	{
	public:
		using value_type = T;

	private:
		value_type m_Value{};

	public:
		/**
		 * \brief Defaulted destructor.
		 */
		constexpr ~value_fn() = default;

		/**
		 * \brief Forwarding constructor.
		 * \tparam TU The constructor argument type.
		 * \param value The argument, which is forwarded to the internal value constructor.
		 */
		template <class TU = T>
			requires std::constructible_from<T, TU>
					&& (!std::same_as<value_fn, std::remove_cvref_t<TU>>)
		[[nodiscard]]
		constexpr
		explicit(!std::convertible_to<TU, T>) value_fn(
			TU&& value
		) noexcept(std::is_nothrow_constructible_v<T, TU>)
			: m_Value{std::forward<TU>(value)}
		{
		}

		/**
		 * \brief Defaulted copy-constructor.
		 */
		constexpr value_fn(const value_fn&) = default;

		/**
		 * \brief Defaulted copy-assignment operator.
		 */
		constexpr value_fn& operator =(const value_fn&) = default;

		/**
		 * \brief Defaulted move-constructor.
		 */
		constexpr value_fn(value_fn&&) = default;

		/**
		 * \brief Defaulted move-assignment operator.
		 */
		constexpr value_fn& operator =(value_fn&&) = default;

		/**
		 * \brief The invocation operator.
		 * \return Returns a const lvalue reference to value.
		 */
		[[nodiscard]]
		constexpr const value_type& operator ()() const & noexcept
		{
			return m_Value;
		}

		/**
		 * \brief The invocation operator.
		 * \return Returns a const lvalue reference to value.
		 */
		[[nodiscard]]
		constexpr const value_type& operator ()() & noexcept
		{
			return m_Value;
		}

		/**
		 * \brief The invocation operator.
		 * \return Returns a const rvalue reference to value.
		 */
		[[nodiscard]]
		constexpr const value_type&& operator ()() const && noexcept
		{
			return std::move(m_Value);
		}

		/**
		 * \brief The invocation operator.
		 * \return Returns a rvalue reference to value.
		 */
		[[nodiscard]]
		constexpr value_type&& operator ()() && noexcept
		{
			return std::move(m_Value);
		}
	};

	/**
	 * \brief Deduction guide.
	 * \tparam TValue Type of the given value.
	 */
	template <class TValue>
	value_fn(TValue) -> value_fn<TValue>;

	template <template <class> class TClosureTemplate, operators::tag... TOperationTags>
		requires concepts::unique_types<TOperationTags...>
	struct enable_operation
		: private unified_base<detail::enable_operators_base_tag>,
		public TOperationTags...
	{
		template <class TFunc>
		using closure_t = TClosureTemplate<TFunc>;
	};

	/** @} */
}

namespace sl::functional::detail
{
	template <operators::tag TOperationTag, class... TTupleElements>
	[[nodiscard]]
	constexpr auto make_composition_from_tuple(
		std::tuple<TTupleElements...>&& functionsTuple
	) noexcept(is_nothrow_composable_v<TOperationTag, TTupleElements...> )
	{
		return std::apply(
			[]<class... TFunctions>(TFunctions&&... functions)
			{
				return composition_fn{operators::tag_operation_t<TOperationTag>{}, std::forward<TFunctions>(functions)...};
			},
			std::move(functionsTuple)
		);
	}

	template <operators::tag TOperationTag, class TFunc1, class TFunc2>
		requires (!operators::detail::joinable_with<TFunc1, TOperationTag>
				&& !operators::detail::joinable_with<TFunc2, TOperationTag>)
	[[nodiscard]]
	constexpr auto make_composition(
		TFunc1&& func1,
		TFunc2&& func2
	) noexcept(is_nothrow_composable_v<TOperationTag, TFunc1, TFunc2>)
	{
		return composition_fn{operators::tag_operation_t<TOperationTag>{}, std::forward<TFunc1>(func1), std::forward<TFunc2>(func2)};
	}

	template <operators::tag TOperationTag,
		operators::detail::joinable_with<TOperationTag> TFunc1,
		operators::detail::joinable_with<TOperationTag> TFunc2
	>
	[[nodiscard]]
	constexpr auto make_composition(
		TFunc1&& func1,
		TFunc2&& func2
	) noexcept(is_nothrow_composable_v<TOperationTag, TFunc1, TFunc2>)
	{
		return make_composition_from_tuple<TOperationTag>(
			std::tuple_cat(
				*std::forward<TFunc1>(func1),
				*std::forward<TFunc2>(func2)
			)
		);
	}

	template <operators::tag TOperationTag, operators::detail::joinable_with<TOperationTag> TFunc1, class TFunc2>
		requires (!operators::detail::joinable_with<TFunc2, TOperationTag>)
	[[nodiscard]]
	constexpr auto make_composition(
		TFunc1&& func1,
		TFunc2&& func2
	) noexcept(is_nothrow_composable_v<TOperationTag, TFunc1, TFunc2>)
	{
		return make_composition_from_tuple<TOperationTag>(
			std::tuple_cat(
				*std::forward<TFunc1>(func1),
				std::forward_as_tuple(std::forward<TFunc2>(func2))
			)
		);
	}

	template <operators::tag TOperationTag, class TFunc1, operators::detail::joinable_with<TOperationTag> TFunc2>
		requires (!operators::detail::joinable_with<TFunc1, TOperationTag>)
	[[nodiscard]]
	constexpr auto make_composition(
		TFunc1&& func1,
		TFunc2&& func2
	) noexcept(is_nothrow_composable_v<TOperationTag, TFunc1, TFunc2>)
	{
		return make_composition_from_tuple<TOperationTag>(
			std::tuple_cat(
				std::forward_as_tuple(std::forward<TFunc1>(func1)),
				*std::forward<TFunc2>(func2)
			)
		);
	}

	template <class TFunc, template <class> class TClosureTemplate, operators::tag... TOperations>
	// ReSharper disable once CppFunctionIsNotImplemented
	typename enable_operation<TClosureTemplate, TOperations...>::template closure_t<TFunc> lookup_closure(
		const enable_operation<TClosureTemplate, TOperations...>& op
	);

	template <class TFunc, derived_from_unified_base<enable_operators_base_tag> T>
	using closure_t = decltype(lookup_closure<TFunc>(std::declval<T>()));

	template <operators::tag TOperationTag, class TFunc1, class TFunc2>
		requires (std::derived_from<std::remove_cvref_t<TFunc1>, TOperationTag>
				&& derived_from_unified_base<TFunc1, enable_operators_base_tag>)
				|| (std::derived_from<std::remove_cvref_t<TFunc2>, TOperationTag>
					&& derived_from_unified_base<TFunc2, enable_operators_base_tag>)
	constexpr auto make_composition_from_tag(
		TFunc1&& func1,
		TFunc2&& func2
	) noexcept(is_nothrow_composable_v<TOperationTag, TFunc1, TFunc2>)
	{
		using composition_t = decltype(make_composition<TOperationTag>(
			unwrap_functional(std::declval<TFunc1>()),
			unwrap_functional(std::declval<TFunc2>())
		));
		using composition_closure_t = closure_t<
			composition_t,
			std::conditional_t<
				derived_from_unified_base<TFunc1, enable_operators_base_tag>,
				TFunc1,
				TFunc2
			>
		>;

		return composition_closure_t{
			make_composition<TOperationTag>(
				unwrap_functional(std::forward<TFunc1>(func1)),
				unwrap_functional(std::forward<TFunc2>(func2))
			)
		};
	}
}

#endif
