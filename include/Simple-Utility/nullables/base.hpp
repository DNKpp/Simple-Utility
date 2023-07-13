//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_NULLABLES_BASE_HPP
#define SL_UTILITY_NULLABLES_BASE_HPP

#pragma once

#include "Simple-Utility/Config.hpp"
#include "Simple-Utility/concepts/operators.hpp"
#include "Simple-Utility/concepts/stl_extensions.hpp"

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
	 * - \ref sl::unique_handle ("Simple-Utility/unique_handle.hpp" header must be included)
	 * - [std::optional](https://en.cppreference.com/w/cpp/utility/optional) ("Simple-Utility/nullables/std_optional.hpp" header must be included)
	 * - [std::unique_ptr](https://en.cppreference.com/w/cpp/memory/unique_ptr) ("Simple-Utility/nullables/std_pointers.hpp" header must be included)
	 * - [std::shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr) ("Simple-Utility/nullables/std_pointers.hpp" header must be included)
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
	 * At last I would like to quickly present a naive comparison between the usual stl style and the style this library offers.
	 * In this example I utilize a adapter, which already supports iterator adapting out of the box.
	 * \snippet adapter.cpp adapter comparison
	 *
	 * ## Using custom types
	 * Well, that depends which interface your type offers. If it's already dereferencable via ``operator *`` and it has an explicit ``null``-object,
	 * which it can equality compared to, than you are in a good position.
	 *
	 * ### There exists a dedicated type or constexpr null object
	 * Just specialize \ref sl::nullables::traits "nullable traits". If your type then doesn't offer a ``operator *`` access or it
	 * doesn't follow the semantics, you can hook the ``unwrap`` customization point and simply create an overload in the types namespace.
	 *
	 * ### No dedicated type or constexpr null object exists
	 * If your type doesn't have a dedicated null object or it is simply not constexpr constructible, you can wrap your type and a null object into
	 * \ref sl::nullables::adapter "adapter" object. Depending how your type behaves with the null object, this adapter will be either a ``input_nullable``
	 * or even a ``nullable``.
	 *
	 * If your type doesn't offer a ``operator *`` access or it doesn't follow the semantics, you can hook the ``unwrap_adapted`` customization point and
	 * simply create an overload in the types namespace.
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
	constexpr static auto null_v{traits<std::remove_cvref_t<T>>::null};

	/**
	 * \brief Specialization for raw pointers
	 * \tparam T Pointer type
	 */
	template <class T>
		requires std::is_pointer_v<T>
	struct traits<T>
	{
		using value_type = std::remove_pointer_t<T>;
		static constexpr std::nullptr_t null{nullptr};
	};

	/** @} */

	namespace detail
	{
		template <class TNullable>
			requires concepts::dereferencable_r<TNullable, value_t<TNullable>>
		[[nodiscard]]
		constexpr decltype(auto) unwrap(
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
			constexpr decltype(auto) operator()(TNullable&& na) const noexcept
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
	{
	};
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
	 * \brief The core algorithm helper, which executes the held operation when used as the right-hand-side of ``operator |`` expressions,
	 * when the left-hand-side is a valid ``nullable`` type.
	 * \tparam Operation The function type.
	 */
	template <concepts::unqualified Operation>
	class Algorithm
	{
	public:
		[[nodiscard]]
		explicit Algorithm(Operation operation) noexcept(std::is_nothrow_move_constructible_v<Operation>)
			: m_Operation{std::move(operation)}
		{
		}

		template <class Nullable>
			requires input_nullable<Nullable>
					&& std::regular_invocable<Operation, Nullable>
		[[nodiscard]]
		friend constexpr auto operator |(
			Nullable&& input,
			const Algorithm& algorithm
		) noexcept(std::is_nothrow_invocable_v<Operation, Nullable>)
		{
			return std::invoke(algorithm.m_Operation, std::forward<Nullable>(input));
		}

	private:
		SL_UTILITY_NO_UNIQUE_ADDRESS Operation m_Operation{};
	};

	/** @} */

	/** @} */
}

namespace sl::nullables::detail
{
	template <class T>
	using dereference_result_t = decltype(nullables::unwrap(std::declval<T>()));
}

#endif
