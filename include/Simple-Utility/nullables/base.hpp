#ifndef SL_UTILITY_NULLABLES_BASE_HPP
#define SL_UTILITY_NULLABLES_BASE_HPP

#pragma once

#include "Simple-Utility/concepts/stl_extensions.hpp"
#include "Simple-Utility/concepts/operators.hpp"
#include "Simple-Utility/functional/base.hpp"
#include "Simple-Utility/functional/operators/bind.hpp"
#include "Simple-Utility/unified_base.hpp"

#include <cstddef>
#include <type_traits>

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
	 * the ``std::ranges`` library and [this proposal](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0798r8.html).
	 * If you would like to learn more about an algorithm, just visit the specific sub-page:
	 *	- \ref sl::nullables::or_else "or_else"
	 *	- \ref sl::nullables::and_then "and_then"
	 *	- \ref sl::nullables::value_or "value_or"
	 *	- \ref sl::nullables::fwd_value "fwd_value"
	 *
	 *	### Quick algorithm example
	 *	This example demonstrates that \ref sl::nullables::and_then "and_then" and \ref sl::nullables::or_else "or_else" algorithms can
	 *	be chained in arbitrarily length. \ref sl::nullables::value_or "value_or" and \ref sl::nullables::fwd_value "fwd_value" on the
	 *	other side act as a terminator (as they usually do not return a \ref sl::nullables::nullable "nullable" objects).
	 * \snippet algorithm.cpp algorithm chain
	 *
	 * This can be quite useful, but the library can even better than this. All nullable algorithms are designed to work properly with the functional
	 * part of this library, thus the above example can be adjusted like this:
	 * \snippet algorithm.cpp algorithm chain functional
	 *
	 * There is a last step one may walk: Nullable algorithms also support functional composition out of the box, which means that
	 * they are composable with any functional as a new functional object and can be invoked later on:
	 * \snippet algorithm.cpp algorithm chain functional composition
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
	 * If that's done, you should specialize the \ref sl::nullables::nullable_traits "traits":
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
	 *	- \ref sl::nullables::fwd_value_func_t "fwd_value_func_t"
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
	 * \defgroup GROUP_NULLABLES_CUSTOMIZATION_POINTS customization points
	 *
	 * \brief Contains several customization points, which users may hook for their custom types.
	 * \details Nullables namespace are designed to work out of the box with types satisfying the corresponding concepts, but sometimes custom types
	 * can not be tweaked freely. This is when the pre-defined customization points begin to shine, as users may simply put overloads in the types namespace
	 * and let the ADL do the work finding it.
	 */

	/**
	 * \defgroup GROUP_NULLABLES_TRAITS traits
	 *
	 * \brief These traits are used by the \ref GROUP_NULLABLES_ALGORITHMS "algorithms" and my be specialized by users for their custom types.
	 * @{
	 */

	/**
	 * \brief The main trait, which may be specialized from.
	 * \tparam T The type to query
	 */
	template <class T>
	struct traits;

	/**
	 * \brief Convenience alias retrieving the value type of a \ref sl::nullables::nullable "nullable" type
	 * \tparam T The type to query
	 */
	template <class T>
	using value_t = typename traits<std::remove_cvref_t<T>>::value_type;

	/**
	 * \brief Convenience constant retrieving the null object of a \ref sl::nullables::nullable "nullable" type
	 * \tparam T  The type to query
	 */
	template <class T>
	constexpr static auto null_v{ traits<std::remove_cvref_t<T>>::null };

	/**
	 * \brief Specialization for raw pointers
	 * \tparam T Pointer type
	 */
	template <class T>
		requires std::is_pointer_v<T>
	struct traits<T>
	{
		using value_type = std::remove_pointer_t<T>;
		inline static constexpr std::nullptr_t null{ nullptr };
	};

	/** @} */

	namespace detail
	{
		template <class TNullable>
			requires concepts::dereferencable_r<TNullable, value_t<TNullable>>
		[[nodiscard]]
		constexpr decltype(auto) unwrap
		(
			TNullable&& na
		)
		noexcept
		{
			return *std::forward<TNullable>(na);
		}

		struct unwrap_fn
		{
			template <class TNullable>
				requires requires { { unwrap(std::declval<TNullable>()) } -> std::convertible_to<value_t<TNullable>>; }
			[[nodiscard]]
			constexpr auto operator()(TNullable&& na) const noexcept
			{
				return unwrap(std::forward<TNullable>(na));
			}
		};
	}

	/**
	 * \brief Retrieves the value of the given \ref sl::nullables::input_nullable "input_nullable".
	 * \ingroup GROUP_NULLABLES_CUSTOMIZATION_POINTS
	 * \details This function is only called if the given \ref sl::nullables::input_nullable "input_nullable" compares unequal to its ``null``-object,
	 * thus it will never access an invalid \ref sl::nullables::input_nullable "input_nullable" and therefore any additional checks in custom
	 * overloads will be unnecessary overhead, which can be avoided.
	 * \note Custom overloads should never throw.
	 */
	inline constexpr detail::unwrap_fn unwrap{};

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
								typename value_t<T>;
								null_v<T>;
							}
							&& concepts::weakly_equality_comparable_with<T, decltype(null_v<T>)>;

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
						&& concepts::initializes<decltype(null_v<T>), std::remove_cvref_t<T>>
						&& std::is_assignable_v<std::remove_cvref_t<T>&, decltype(null_v<T>)>;
}

namespace sl::nullables::detail
{
	struct algorithm_tag
	{ };
}

namespace sl::nullables
{
	/**
	 * \defgroup GROUP_NULLABLES_ALGORITHMS algorithms
	 *
	 * \brief Contains nullable algorithm related symbols.
	 * \details There already exists several pre-defined algorithms, but users may easily write their own algorithm within a few lines.
	 * \snippet nullables/base.cpp algorithm custom
	 * @{
	 */

	/**
	 * \brief The core algorithm helper, which executes the held algorithm when used as the right-hand-side of ``operator |`` expressions,
	 * when the left-hand-side is a valid ``nullable`` type.
	 * \tparam TFunc The function type.
	 */
	template <class TFunc>
		requires std::same_as<TFunc, std::remove_cvref_t<TFunc>>
	class algorithm_fn final
		: private unified_base<detail::algorithm_tag>,
		public functional::closure_base_fn<TFunc>,
		public functional::enable_operation<
			algorithm_fn,
			functional::operators::bind_back,
			functional::operators::bind_front
		>
	{
		using closure_t = functional::closure_base_fn<TFunc>;
	public:
		using closure_t::closure_t;
	};

	/**
	 * \brief Deduction guide for \ref sl::nullables::algorithm_fn "algorithm_fn" types.
	 * \tparam TFunc The function type.
	 */
	template <class TFunc>
	algorithm_fn(TFunc) -> algorithm_fn<TFunc>;

	/**
	 * \brief The operator is used to chain nullables with algorithms.
	 * \tparam TNullable The nullable type.
	 * \tparam TAlgorithm The algorithm type. 
	 * \param nullable The nullable object.
	 * \param algorithm The algorithm object.
	 * \return Returns the forwarded result of the algorithm invocation.
	 */
	template <input_nullable TNullable, derived_from_unified_base<detail::algorithm_tag> TAlgorithm>
		requires std::invocable<TAlgorithm, TNullable>
	[[nodiscard]]
	constexpr decltype(auto) operator |
	(
		TNullable&& nullable,
		TAlgorithm&& algorithm
	)
	noexcept(std::is_nothrow_invocable_v<TAlgorithm, TNullable>)
	{
		return std::invoke(std::forward<TAlgorithm>(algorithm), std::forward<TNullable>(nullable));
	}

	/** @} */

	/** @} */
}

namespace sl::nullables::detail
{
	template <class T>
	using dereference_result_t = decltype(nullables::unwrap(std::declval<T>()));
}

#endif