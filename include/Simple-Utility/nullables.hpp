//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_NULLABLES_HPP
#define SL_UTILITY_NULLABLES_HPP

#include <concepts>

#include "Simple-Utility/unique_handle.hpp"
#include "Simple-Utility/concepts/operators.hpp"

namespace sl::nullables
{
	/**
	 * \defgroup GROUP_NULLABLES nullables
	 *
	 * \brief This library offers some simple algorithms for convenient working with \ref sl::nullables::nullable "nullable" types.
	 * \details
	 * ## General
	 * The algorithms may be chained in arbitrary combination and deepness, as long as it makes sense to the compiler. Such a
	 * chain may involve multiple different \ref sl::nullables::nullable "nullable" types. Of this library explicitly supported types are:
	 * - \ref sl::unique_handle
	 * - [std::optional](https://en.cppreference.com/w/cpp/utility/optional)
	 * - [std::unique_ptr](https://en.cppreference.com/w/cpp/memory/unique_ptr)
	 * - [std::shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr)
	 * - and ``raw pointers``
	 *
	 * The general idea is making the handling with types e.g. ``std::optional`` less verbose and more enjoyable. The syntax is inspired by
	 * the ``std::ranges`` library.
	 * If you would like to learn more about an algorithm, just visit the specific sub-page:
	 *	- \ref sl::nullables::or_else "or_else"
	 *	- \ref sl::nullables::and_then "and_then"
	 *	- \ref sl::nullables::value_or "value_or"
	 *
	 * ## Working with movable types
	 * If you have an object which you would like to move into the algorithm chain, either because its non-copyable or its simply more performant,
	 * its totally fine to do so. But, as the library assumes taking over the ownership of objects received as ``rvalue ref`` use the remaining
	 * object with care, as it might have or might have not been moved. Simply the same rules for moved object as usual apply.
	 * \snippet nullables.cpp nullables algorithm chain movable
	 *
	 * ## Using custom or third party types
	 * Well, that depends which interface your type offers. If it's already dereferencable via ``operator *`` and it has an explicit ``null``-object,
	 * which it can equality compared to, than you are in a good position.
	 * Otherwise you may specialize the \ref sl::nullables::value_unchecked "value_unchecked" function or simply adjust your interface (if possible).
	 * Either way, a explicit constant ``null``-object is mandatory and can not be avoided.
	 *
	 * If that's done, you should specialize the \ref sl::nullables::nullable_traits "nullable_traits":
	 * \snippet nullables.cpp nullables custom type traits
	 *
	 * After that everything is already setup and your type can be at least used as a \ref sl::nullables::input_nullable "input_nullable" (which makes
	 * it usable as input in \ref sl::nullables::and_then "and_then" and \ref sl::nullables::value_or "value_or" algorithms).
	 * Making it actually a \ref sl::nullables::nullable "nullable" type, it must be assignable and constructible by its ``null``-object, which
	 * might be achieved by overloading the conversion operator of the ``null``-type (which can't be declared explicit).
	 *
	 * Additionally if there is an actual need for that step, you may also specialize the algorithm implementations. Have a look at these members:
	 *	- \ref sl::nullables::value_or_func_t "value_or_func_t"
	 *	- \ref sl::nullables::value_or_func_t "and_then_func_t"
	 *	- \ref sl::nullables::value_or_func_t "or_else_func_t"
	 *
	 *	#### Example: How to make a type ready as nullable
	 *	Given a type which you can not adjust (here ``your_type``, which is quite similar to ``std::optional``), which you would like to use with
	 *	these algorithms.
	 *	\snippet nullables.cpp nullables your_type definition
	 *
	 *	Your first step should then defining a dedicated ``null``-type:
	 *	\snippet nullables.cpp nullables your_type null-type
	 *
	 *	Second you need to specialize the \ref sl::nullables::value_unchecked "value_unchecked" function, because ``your_type`` can not dereferenced
	 *	via ``operator *``.
	 *	\snippet nullables.cpp nullables your_type value_unchecked
	 *
	 *	Last you need to specialize the type traits for that type:
	 *	\snippet nullables.cpp nullables your_type traits
	 *
	 *	That's it. Now ``your_type`` is ready to be used as \ref sl::nullables::nullable "nullable" type.
	 *
	 * @{
	 */

	/**
	 * \defgroup GROUP_NULLABLES_TRAITS traits
	 *
	 * \brief These traits are used by the \ref GROUP_NULLABLES_ALGORITHMS "algorithms" and my be extended by users.
	 * @{
	 */

	/**
	 * \brief The main trait, which may be specialized from.
	 * \tparam T The type to query
	 */
	template <class T>
	struct nullable_traits
	{
	};

	/**
	 * \brief Convenience alias retrieving the value type of a \ref sl::nullables::nullable "nullable" type
	 * \tparam T The type to query
	 */
	template <class T>
	using nullable_value_t = typename nullable_traits<std::remove_cvref_t<T>>::value_type;

	/**
	 * \brief Convenience constant retrieving the null object of a \ref sl::nullables::nullable "nullable" type
	 * \tparam T  The type to query
	 */
	template <class T>
	constexpr static auto nullable_null_v{ nullable_traits<std::remove_cvref_t<T>>::null };

	/**
	 * \brief Checks whether a type is an \ref sl::nullables::input_nullable "input_nullable".
	 * \details A type is considered as \ref sl::nullables::input_nullable "input_nullable" if:
	 *		- ``nullable_traits`` is found which exposes the ``value_type`` and ``null``-object,
	 *		- it satisfies the \ref sl::concepts::weakly_equality_comparable_with "weakly_equality_comparable_with" concept with its ``null``-object,
	 * \tparam T Type to check
	 */
	template <class T>
	concept input_nullable = requires
							{
								typename nullable_value_t<T>;
								nullable_null_v<T>;
							}
							&& concepts::weakly_equality_comparable_with<T, decltype(nullable_null_v<T>)>;

	/**
	 * \brief Checks whether a type is \ref sl::nullables::nullable "nullable".
	 * \details A type is considered as \ref sl::nullables::nullable "nullable" if:
	 *		- it satisfies the \ref sl::nullables::input_nullable "input_nullable" concept
	 *		- it is initializable by its ``null``-object and
	 *		- it is assignable by its ``null``-object.
	 * \tparam T Type to check
	 */
	template <class T>
	concept nullable = input_nullable<T>
						&& concepts::initializes<std::remove_cvref_t<decltype(nullable_null_v<T>)>, std::remove_cvref_t<T>>
						&& std::is_assignable_v<std::remove_cvref_t<T>&, decltype(nullable_null_v<T>)>;

	/**
	 * \brief Specialization for sl::unique_handle
	 * \tparam TArgs Arguments of sl::unique_handle
	 */
	template <class... TArgs>
	struct nullable_traits<unique_handle<TArgs...>>
	{
		using value_type = typename unique_handle<TArgs...>::value_type;
		constexpr static nullhandle_t null{};
	};

	/**
	 * \brief Specialization for std::optional
	 * \tparam TArgs Arguments of std::optional
	 */
	template <class... TArgs>
	struct nullable_traits<std::optional<TArgs...>>
	{
		using value_type = typename std::optional<TArgs...>::value_type;
		constexpr static std::nullopt_t null{ std::nullopt };
	};

	/**
	 * \brief Specialization for std::unique_ptr
	 * \tparam TArgs Arguments of std::unique_ptr
	 */
	template <class... TArgs>
	struct nullable_traits<std::unique_ptr<TArgs...>>
	{
		using value_type = typename std::unique_ptr<TArgs...>::element_type;
		constexpr static std::nullptr_t null{ nullptr };
	};

	/**
	 * \brief Specialization for std::shared_ptr
	 * \tparam TArgs Arguments of std::shared_ptr
	 */
	template <class... TArgs>
	struct nullable_traits<std::shared_ptr<TArgs...>>
	{
		using value_type = typename std::shared_ptr<TArgs...>::element_type;
		constexpr static std::nullptr_t null{ nullptr };
	};

	/**
	 * \brief Specialization for raw pointers
	 * \tparam T Pointer type
	 */
	template <class T>
		requires std::is_pointer_v<T>
	struct nullable_traits<T>
	{
		using value_type = std::remove_pointer_t<T>;
		constexpr static std::nullptr_t null{ nullptr };
	};

	/** @} */

	/**
	 * \brief Returns the internal value of the \ref sl::nullables::input_nullable "input_nullable" object. Must not throw.
	 * \details This function will be utilized by the \ref sl::nullables::input_nullable "input_nullable" algorithms. It is only called
	 * when the \ref sl::nullables::input_nullable "input_nullable" != its null-object and should therefore not perform any additional
	 * security checks.
	 * \tparam TNullable The \ref sl::nullables::input_nullable "input_nullable" type
	 * \param closure The \ref sl::nullables::input_nullable "input_nullable" object
	 * \return Returns the value as is
	 */
	template <input_nullable TNullable>
		requires concepts::dereferencable<TNullable>
	[[nodiscard]]
	constexpr decltype(auto) value_unchecked(TNullable&& closure) noexcept
	{
		return *std::forward<TNullable>(closure);
	}

	namespace detail
	{
		template <input_nullable TNullable>
		using dereference_type_t = decltype(value_unchecked(std::declval<TNullable>()));

		template <class TNullable, class TAlt>
		concept has_value_or_member = input_nullable<TNullable>
									&& requires(TNullable n, TAlt a)
									{
										{ n.value_or(a) } -> std::convertible_to<nullable_value_t<TNullable>>;
									};
	}

	/**
	 * \defgroup GROUP_NULLABLES_ALGORITHMS algorithms
	 *
	 * \brief These algorithms are designed to work with any type which satisfies the \ref sl::nullables::nullable "nullable" concept.
	 * \details For further read, visit any algorithms sub-page.
	 * @{
	 */

	/**
	 * \defgroup GROUP_NULLABLES_ALGORITHMS_IMPL implementation
	 *
	 * \brief These are the implementations of the \ref GROUP_NULLABLES_ALGORITHMS "nullable algorithms". Users may
	 * specialize them for custom types, but should not explicitly used in code.
	 * @{
	 */

	/**
	 * \brief Base type for the ``value_or`` algorithms. May be specialized.
	 * \tparam TNullable The decayed type of the \ref sl::nullables::input_nullable "input_nullable"
	 * \tparam TAlt The decayed type of the alternative. 
	 */
	template <class TNullable, class TAlt>
	struct value_or_func_t;

	/**
	 * \brief Specialization which will be used instead if the ``value_or`` functions is present as a member of ``TNullable`` type.
	 * \tparam TNullable The decayed type of the \ref sl::nullables::input_nullable "input_nullable"
	 * \tparam TAlt The decayed type of the alternative. 
	 */
	template <input_nullable TNullable, class TAlt>
		requires detail::has_value_or_member<TNullable, TAlt>
	struct value_or_func_t<TNullable, TAlt>
	{
		/**
		 * \brief Invoke operator
		 * \tparam UNullable The actual \ref sl::nullables::input_nullable "input_nullable" type
		 * \tparam UAlt The actual alternative type
		 * \param closure The nullable object
		 * \param alternative The alternative object
		 * \return Returns the invocation result of the ``value_or`` member function.
		 */
		template <input_nullable UNullable, class UAlt>
		[[nodiscard]]
		constexpr decltype(auto) operator()(UNullable&& closure, UAlt&& alternative)
		{
			return std::forward<UNullable>(closure).value_or(std::forward<UAlt>(alternative));
		}
	};

	/**
	 * \brief General algorithm implementation. May be specialized by users if necessary.
	 * \tparam TNullable The decayed type of the \ref sl::nullables::input_nullable "input_nullable"
	 * \tparam TAlt The decayed type of the alternative. Must initialize ``nullable_value_t<TNullable>``
	 */
	template <input_nullable TNullable, class TAlt>
		requires (!detail::has_value_or_member<TNullable, TAlt>)
	struct value_or_func_t<TNullable, TAlt>
	{
		static_assert
		(
			concepts::initializes<TAlt, nullable_value_t<TNullable>>,
			"The alternative must be usable to initialize a nullable_value_t<TNullable> value."
		);

		/**
		 * \brief Invoke operator
		 * \tparam UNullable The actual \ref sl::nullables::input_nullable "input_nullable" type
		 * \tparam UAlt The actual alternative type
		 * \param closure The nullable object
		 * \param alternative The alternative object
		 * \return Returns the either the \ref sl::nullables::input_nullable "input_nullable" value
		 * or the forwarded alternative.
		 */
		template <input_nullable UNullable, class UAlt>
		[[nodiscard]]
		constexpr nullable_value_t<TNullable> operator()(UNullable&& closure, UAlt&& alternative)
		{
			if (closure != nullable_null_v<UNullable>)
			{
				return value_unchecked(std::forward<UNullable>(closure));
			}
			return std::forward<UAlt>(alternative);
		}
	};

	/**
	 * \brief Base type for the ``or_else`` algorithms. May be specialized.
	 * \tparam TNullable  Type of the \ref sl::nullables::nullable "nullable"
	 * \tparam TFunc Type of the passed function
	 */
	template <class TNullable, class TFunc>
	struct or_else_func_t;

	/**
	 * \brief General algorithm implementation. May be specialized by users if necessary.
	 * \tparam TNullable Type of the \ref sl::nullables::nullable "nullable"
	 * \tparam TFunc Type of the passed function.
	 */
	template <nullable TNullable, std::invocable TFunc>
		requires std::constructible_from<std::remove_cvref_t<TNullable>, TNullable>
	struct or_else_func_t<TNullable, TFunc>
	{
		[[nodiscard]]
		constexpr std::remove_cvref_t<TNullable> operator()(TNullable&& closure, TFunc func)
		{
			static_assert
			(
				std::constructible_from<std::remove_cvref_t<TNullable>, std::invoke_result_t<TFunc>>
				|| std::same_as<void, std::invoke_result_t<TFunc>>,
				"Func return type must be either void or initialize the std::remove_cvref_t<TNullable> type."
			);

			if (closure != nullable_null_v<TNullable>)
			{
				return std::forward<TNullable>(closure);
			}

			if constexpr (std::same_as<void, std::invoke_result_t<TFunc>>)
			{
				std::invoke(func);
				return nullable_null_v<TNullable>;
			}
			else
			{
				return std::invoke(func);
			}
		}
	};

	/**
	 * \brief Base type for the ``and_then`` algorithms. May be specialized.
	 * \tparam TNullable  Type of the \ref sl::nullables::input_nullable "input_nullable"
	 * \tparam TFunc Type of the passed function
	 */
	template <class TNullable, class TFunc>
	struct and_then_func_t;

	/**
	 * \brief General algorithm implementation. May be specialized by users if necessary.
	 * \tparam TNullable Type of the \ref sl::nullables::input_nullable "input_nullable"
	 * \tparam TFunc Type of the passed function.
	 */
	template <input_nullable TNullable, class TFunc>
	struct and_then_func_t<TNullable, TFunc>
	{
		static_assert
		(
			std::invocable<TFunc, detail::dereference_type_t<TNullable>>,
			"TFunc must accept the value returned by TNullables as parameter."
		);

		using return_t = std::invoke_result_t<TFunc, detail::dereference_type_t<TNullable>>;
		static_assert(nullable<return_t>, "TFunc must return a nullable type.");

		[[nodiscard]]
		constexpr return_t operator()(TNullable&& closure, TFunc func)
		{
			if (closure != nullable_null_v<TNullable>)
			{
				return std::invoke(func, value_unchecked(std::forward<TNullable>(closure)));
			}
			return nullable_null_v<return_t>;
		}
	};

	/** @} */

	/**
	 * \brief Retrieves the value of a \ref sl::nullables::input_nullable "input_nullable" if it's not equal to its ''null''-object.
	 * Returns the alternative otherwise.
	 * \tparam T Type of alternative. Must initialize ``nullable_value_t<TNullable>`` objects.
	 *
	 * \details This algorithm returns the alternative if the \ref sl::nullables::input_nullable "input_nullable" compares equal to its
	 * ``null``-object. Otherwise the value of the \ref sl::nullables::input_nullable "input_nullable" is returned.
	 * \note In the following examples the outcome is always presented within the ``REQUIRE()`` statement.
	 *
	 * This example shows what happens when a valid \ref sl::nullables::input_nullable "input_nullable" is used in a ``value_or`` expression.
	 * \snippet nullables.cpp value_or valid copyable
	 *
	 *
	 * This example shows what happens when an invalid \ref sl::nullables::input_nullable "input_nullable" is used in a ``value_or`` expression.
	 * \snippet nullables.cpp value_or invalid copyable
	 */
	template <class T>
	class value_or
	{
	public:
		/**
		 * \brief Constructor awaiting an alternative value
		 * \param alternative The passed alternative value
		 */
		explicit constexpr value_or(T alternative) noexcept
			: m_Alternative{ std::forward<T>(alternative) }
		{
		}

		/**
		 * \brief Deleted copy-constructor
		 */
		value_or(const value_or&) = delete;
		/**
		 * \brief Deleted copy-assign
		 */
		value_or& operator =(const value_or&) = delete;

		/**
		 * \brief Operator which let the algorithm operate on the \ref sl::nullables::input_nullable "input_nullable" on the left side.
		 * \tparam TNullable The \ref sl::nullables::input_nullable "input_nullable" type
		 * \param closure The \ref sl::nullables::input_nullable "input_nullable" object
		 * \param valueOr The algorithm object
		 * \return Returns either the value of the \ref sl::nullables::input_nullable "input_nullable" or the alternative.
		 */
		template <input_nullable TNullable>
		[[nodiscard]]
		friend constexpr nullable_value_t<TNullable> operator |(TNullable&& closure, value_or&& valueOr)
		{
			return value_or_func_t<std::remove_cvref_t<TNullable>, std::remove_cvref_t<T>>{}
			(
				std::forward<TNullable>(closure),
				std::forward<T>(valueOr.m_Alternative)
			);
		}

	private:
		T m_Alternative;
	};

	/**
	 * \brief Deduction guide for \ref value_or
	 * \tparam T The type of the alternative
	 */
	template <class T>
	value_or(T&&) -> value_or<T>;

	/**
	 * \brief Passes the value of the \ref sl::nullables::input_nullable "input_nullable" to the function if it's not equal to its ``null``-object.
	 * Returns the ``null``-object otherwise.
	 * \tparam TFunc The type of the passed function. The function should be invokable with types returned by \ref sl::nullables::value_unchecked
	 * "value_unchecked" and return a type which satisfies the \ref sl::nullables::nullable "nullable" concept (this type does not have to be
	 * the same as the type already used in the expression).
	 *
	 * \details This algorithm uses the actual value of a \ref sl::nullables::input_nullable "input_nullable" and passes it to the given functional.
	 * The functional should then return \b any \ref sl::nullables::nullable "nullable" \b compatible \b type as desired.
	 * Otherwise, if the \ref sl::nullables::input_nullable "input_nullable" compares equal to its ``null``-object, the ``null``-object is returned instead.
	 *
	 * \note In the following examples the outcome is always presented within the ``REQUIRE()`` statement.
	 *
	 * This example shows what happens when a valid \ref sl::nullables::input_nullable "input_nullable" is used in a ``and_then`` expression.
	 * \snippet nullables.cpp and_then valid copyable
	 *
	 *
	 * This example shows what happens when an invalid \ref sl::nullables::input_nullable "input_nullable" is used in a ``and_then`` expression.
	 * Note that the resulting \ref sl::nullables::nullable "nullable" type differs from the starting one.
	 * \snippet nullables.cpp and_then invalid copyable
	 */
	template <class TFunc>
	class and_then
	{
	public:
		/**
		 * \brief Constructor awaiting a function object
		 * \param func The function object
		 */
		explicit constexpr and_then(TFunc func) noexcept
			: m_Func{ std::move(func) }
		{
		}

		/**
		 * \brief Operator which let the algorithm operate on the \ref sl::nullables::input_nullable "input_nullable" on the left side.
		 * \tparam TNullable The \ref sl::nullables::input_nullable "input_nullable" type
		 * \param closure The \ref sl::nullables::input_nullable "input_nullable" object
		 * \param andThen The algorithm object
		 * \return Returns either a ``null``-object (if \ref sl::nullables::input_nullable "input_nullable" compares equal to its ``null``-object)
		 * or the invocation result of the passed function.
		 */
		template <input_nullable TNullable>
		[[nodiscard]]
		friend constexpr
		std::invoke_result_t<TFunc, detail::dereference_type_t<TNullable>> operator |
		(
			TNullable&& closure,
			and_then&& andThen
		)
		{
			return and_then_func_t<TNullable, std::reference_wrapper<TFunc>>{}
			(
				std::forward<TNullable>(closure),
				std::ref(andThen.m_Func)
			);
		}

	private:
		TFunc m_Func;
	};

	/**
	 * \brief Returns the \ref sl::nullables::nullable "nullable" if it's not equal to its ''null''-object. Executes the passed function otherwise.
	 * \tparam TFunc The type of the passed function. The return type must be either ```void`` or such that it can be
	 * used to initialize a new object of type ``std::remove_cvref_t<TNullable>``.
	 *
	 * \details This algorithm simply forwards the \ref sl::nullables::nullable "nullable" if it compares unequal to its ``null``-object.
	 * Otherwise the passed function will be called and the returned value initializes a new \ref sl::nullables::nullable "nullable"-object of the same type.
	 * If the functions return type is ``void``, the ``null``-object of that \ref sl::nullables::nullable "nullable"-type will be returned instead.
	 *
	 * \note In the following examples the outcome is always presented within the ``REQUIRE()`` statement.
	 * 
	 * This example shows what happens when a valid \ref sl::nullables::nullable "nullable" is used in a ``or_else`` expression.
	 * \snippet nullables.cpp or_else valid copyable
	 * \---
	 * 
	 * This example shows what happens when an invalid \ref sl::nullables::nullable "nullable" is used in a ``or_else`` expression and the functional has a
	 * return type other than ``void``.
	 * \snippet nullables.cpp or_else invalid value non-void return copyable
	 * \---
	 * 
	 * This example shows what happens when an invalid \ref sl::nullables::nullable "nullable" is used in a ``or_else`` expression and the functional does not
	 * return anything.
	 * \snippet nullables.cpp or_else invalid value void return copyable
	 */
	template <std::invocable TFunc>
	class or_else
	{
	public:
		/**
		 * \brief Constructor awaiting a function object
		 * \param func The function object
		 */
		explicit constexpr or_else(TFunc func) noexcept
			: m_Func{ std::move(func) }
		{
		}

		/**
		 * \brief Operator which let the algorithm operate on the \ref sl::nullables::nullable "nullable" on the left side.
		 * \tparam TNullable The \ref sl::nullables::nullable "nullable" type
		 * \param closure The \ref sl::nullables::nullable "nullable" object
		 * \param orElse The algorithm object
		 * \return Returns either the \ref sl::nullables::nullable "nullable" or the invocation result of the function. If the function has
		 * a ``void`` return type then a the ``null``-object of the given \ref sl::nullables::nullable "nullable" will be returned.
		 */
		template <nullable TNullable>
		[[nodiscard]]
		friend constexpr std::remove_cvref_t<TNullable> operator |(TNullable&& closure, or_else&& orElse)
		{
			return or_else_func_t<TNullable, std::reference_wrapper<TFunc>>{}
			(
				std::forward<TNullable>(closure),
				std::ref(orElse.m_Func)
			);
		}

	private:
		TFunc m_Func;
	};

	/** @} */

	/** @} */
}

#endif
