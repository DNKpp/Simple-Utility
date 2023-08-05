//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_FUNCTIONAL_BASIC_CLOSURE_HPP
#define SIMPLE_UTILITY_FUNCTIONAL_BASIC_CLOSURE_HPP

#pragma once

#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>

#include "Simple-Utility/concepts/stl_extensions.hpp"

namespace sl::functional
{
	/**
	 * \defgroup GROUP_FUNCTIONAL functional
	 * \brief This namespace contains helper types for complex functional chaining and concrete functional objects.
	 * \details As working more and more with ``ranges`` and functional style approach, this library is designed to fully
	 * support the programmers in such cases.
	 *
	 * This namespaces aims to simplify caller code, when supplying algorithm with user provided function objects. Therefore several
	 * types are offered, which enable different composition operations. Which one exactly, depends on the concrete closure template.
	 * 
	 * \note The wrapped function serves only as an enabler for the composition operators. Once a composition has been started, any
	 * type of functional may be provided.
	 *
	 * \details
	 * # Transform closure
	 * The transform part of this library is designed to help the programmer when working with ``ranges`` or any other algorithm,
	 * which accepts a functional as part of its interface. There is often the need to apply multiple sub-steps on a value,
	 * before the work is actually done.
	 * Because of this many existing functions are not usable by their own, thus they must be wrapped into a lambda, which
	 * results in quite a bit of noise. With objects of type ``Transform`` programmers have the option to pipe function results into
	 * other functions, and thus nest multiple functions into each other.
	 * \snippet functional/Transform.cpp transform piped
	 * ``sf::envelop`` is a simple factory function, which hands over the given functional to the ``Transform`` template, so users don't have
	 * to supply the actual template params (CTAD unfortunately isn't a thing yet for typedefs...).
	 * 
	 * ``composition`` is equivalent to
	 * \code{.cpp}
	 * [](const int i) { return std::to_string(i + 42); };
	 * \endcode
	 * 
	 * Sure, that seems to be a lot more effort for this trivial task, but this approach lets you compose existing features into more complex
	 * ones. Have a look at the next example, where we pre-define the ``to_string`` in forehand:
	 * \snippet functional/Transform.cpp transform to_string definition
	 * 
	 * \snippet functional/Transform.cpp transform more complex pipeline
	 * We create a range of integer values and transform these into strings (NOTE: ``std::format`` was no option on gcc at the time of writing).
	 * We first negate the values, multiply the result by two, transform them to a string and then formulate a sentence with them.
	 * As you can see, once the pipe is active, one can chain any type of functional; even mutable lambdas are supported!
	 * 
	 * # Predicate closure
	 * ``Predicate`` objects are fully compatible with ``Transform`` objects, thus they may be mixed as desired. Additionally they
	 * offer composing via operator &&, || and other common operations. Other than piping, all such composed predicates will receive the
	 * identically input and are required to return boolean convertible results. Eventually ``Predicate`` are easily invertible via operator !.
	 * 
	 * Let there be a pre-defined predicate ``is_even``:
	 * \snippet functional/Predicate.cpp predicate is_even definition
	 * You can combine predicates in various ways and therefore create much more complex predicates. For example, this snippet filters values which
	 * are even and less than 44 OR odd and greater than 45. The usual operator precedences apply here as-well.
	 * \snippet functional/Predicate.cpp predicate more complex
	 * 
	 * \{
	 */

	/**
	 * \brief Determines whether the given type satisfies the constraints of a function type.
	 * \tparam T Type to check.
	 * \details There are not many syntactic constraints, the concept can check for. The important one is rather semantic:
	 * The given type must be invokable with params which will be determined later.
	 */
	template <class T>
	concept function = concepts::unqualified<T>
						&& std::is_object_v<T>;

	/**
	 * \brief Determines whether the given type satisfies the constraints of an invoke-policy.
	 * \tparam T Type to check.
	 * \details There are not many syntactic constraints, the concept can check for. The policy enables the invoke operators
	 * for the closure type and should therefore accept all valid argument combinations.
	 */
	template <template <class> class T>
	concept invoke_policy = true;

	/**
	 * \brief Determines whether the given type satisfies the constraints of a operator-policy.
	 * \tparam T Type to check.
	 * \details There are not many syntactic constraints, the concept can check for. The policy enables the various composing operators
	 * for the closure type. It should define a binary operator(-set) and return a composition (wrapped in another closure type) of both.
	 */
	template <template <class> class T>
	concept operator_policy = true;

	/**
	 * \brief The core class, wrapping one functional object and enabling a variety of composing operators for it.
	 * \tparam Fn The wrapped functional type.
	 * \tparam InvokePolicy CRTP type, providing different operator () implementations.
	 * \tparam OperatorPolicies CRTP types, providing different composing operators.
	 */
	template <function Fn, template <class> class InvokePolicy, template <class> class... OperatorPolicies>
		requires invoke_policy<InvokePolicy>
				&& (... && operator_policy<OperatorPolicies>)
	class BasicClosure
		: public InvokePolicy<BasicClosure<Fn, InvokePolicy, OperatorPolicies...>>,
		public OperatorPolicies<BasicClosure<Fn, InvokePolicy, OperatorPolicies...>>...
	{
	public:
		/**
		 * \brief Forwarding constructor, forwarding all of its arguments to its stored functional.
		 * \tparam Args Constructor argument types.
		 * \param args Constructor arguments.
		 */
		template <class... Args>
			requires std::constructible_from<Fn, Args...>
		explicit constexpr BasicClosure(Args&&... args) noexcept(std::is_nothrow_constructible_v<Fn, Args...>)
			: m_Fn{std::forward<Args>(args)...}
		{
		}

		/**
		 * \brief Defaulted copy constructor.
		 * \note Only present if the stored functional is also copy constructible.
		 */
		BasicClosure(const BasicClosure&)
			requires std::copy_constructible<Fn> = default;

		/**
		 * \brief Defaulted copy assign operator.
		 * \note Only present if the stored functional is also copy assignable.
		 */
		BasicClosure& operator =(const BasicClosure&)
			requires std::is_copy_assignable_v<Fn> = default;

		/**
		 * \brief Defaulted move constructor.
		 * \note Only present if the stored functional is also move constructible.
		 */
		BasicClosure(BasicClosure&&)
			requires std::move_constructible<Fn> = default;

		/**
		 * \brief Defaulted move assign operator.
		 * \note Only present if the stored functional is also move assignable.
		 */
		BasicClosure& operator =(BasicClosure&&)
			requires std::is_move_assignable_v<Fn> = default;

		/**
		 * \brief Defaulted destructor.
		 */
		~BasicClosure() = default;

		/**
		 * \brief Conversion operator, providing access to the stored functional as const lvalue-reference.
		 */
		[[nodiscard]]
		explicit constexpr operator const Fn&() const & noexcept
		{
			return m_Fn;
		}

		/**
		 * \brief Conversion operator, providing access to the stored functional as lvalue-reference.
		 */
		[[nodiscard]]
		explicit constexpr operator Fn&() & noexcept
		{
			return m_Fn;
		}

		/**
		 * \brief Conversion operator, providing access to the stored functional as const rvalue-reference.
		 */
		[[nodiscard]]
		explicit constexpr operator const Fn&&() const && noexcept
		{
			return std::move(m_Fn);
		}

		/**
		 * \brief Conversion operator, providing access to the stored functional as rvalue-reference.
		 */
		[[nodiscard]]
		explicit constexpr operator Fn&&() && noexcept
		{
			return std::move(m_Fn);
		}

	private:
		Fn m_Fn{};
	};

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_FUNCTIONAL_UNWRAP_FUNCTIONAL unwrap_functional
	 * \ingroup GROUP_FUNCTIONAL
	 * \brief Trait, unwrapping the functional type if a closure type is given, otherwise the given type.
	 * 
	 * \{
	 */

	/**
	 * \brief Primary template for non BasicClosure types.
	 * \tparam T The functional type.
	 */
	template <class T>
	struct unwrap_functional
	{
		using type = T;
	};

	/**
	 * \brief Specialization for BasicClosure class. Purposely undefined.
	 * \tparam Fn The wrapped functional type.
	 * \tparam InvokePolicy CRTP type, providing different operator () implementations.
	 * \tparam OperatorPolicies CRTP types, providing different composing operators.
	 */
	template <class Fn, template <class> class InvokePolicy, template <class> class... OperatorPolicies>
	struct unwrap_functional<BasicClosure<Fn, InvokePolicy, OperatorPolicies...>>;

	/**
	 * \brief Specialization for const BasicClosure class. Purposely undefined.
	 * \tparam Fn The wrapped functional type.
	 * \tparam InvokePolicy CRTP type, providing different operator () implementations.
	 * \tparam OperatorPolicies CRTP types, providing different composing operators.
	 */
	template <class Fn, template <class> class InvokePolicy, template <class> class... OperatorPolicies>
	struct unwrap_functional<const BasicClosure<Fn, InvokePolicy, OperatorPolicies...>>;

	/**
	 * \brief Specialization for const lvalue-references of BasicClosure class.
	 * \tparam Fn The wrapped functional type.
	 * \tparam InvokePolicy CRTP type, providing different operator () implementations.
	 * \tparam OperatorPolicies CRTP types, providing different composing operators.
	 */
	template <class Fn, template <class> class InvokePolicy, template <class> class... OperatorPolicies>
	struct unwrap_functional<const BasicClosure<Fn, InvokePolicy, OperatorPolicies...>&>
	{
		using type = const Fn&;
	};

	/**
	 * \brief Specialization for lvalue-references of BasicClosure class.
	 * \tparam Fn The wrapped functional type.
	 * \tparam InvokePolicy CRTP type, providing different operator () implementations.
	 * \tparam OperatorPolicies CRTP types, providing different composing operators.
	 */
	template <class Fn, template <class> class InvokePolicy, template <class> class... OperatorPolicies>
	struct unwrap_functional<BasicClosure<Fn, InvokePolicy, OperatorPolicies...>&>
	{
		using type = Fn&;
	};

	/**
	 * \brief Specialization for const rvalue-references of BasicClosure class.
	 * \tparam Fn The wrapped functional type.
	 * \tparam InvokePolicy CRTP type, providing different operator () implementations.
	 * \tparam OperatorPolicies CRTP types, providing different composing operators.
	 */
	template <class Fn, template <class> class InvokePolicy, template <class> class... OperatorPolicies>
	struct unwrap_functional<const BasicClosure<Fn, InvokePolicy, OperatorPolicies...>&&>
	{
		using type = const Fn&&;
	};

	/**
	 * \brief Specialization for rvalue-references of BasicClosure class.
	 * \tparam Fn The wrapped functional type.
	 * \tparam InvokePolicy CRTP type, providing different operator () implementations.
	 * \tparam OperatorPolicies CRTP types, providing different composing operators.
	 */
	template <class Fn, template <class> class InvokePolicy, template <class> class... OperatorPolicies>
	struct unwrap_functional<BasicClosure<Fn, InvokePolicy, OperatorPolicies...>&&>
	{
		using type = Fn&&;
	};

	/**
	 * \brief Convenient alias for the type member alias to unwrap_functional trait.
	 * \tparam T The functional type.
	 */
	template <class T>
	using unwrap_functional_t = typename unwrap_functional<T>::type;

	/**
	 * \}
	 */

	/**
	 * \brief Unwraps the functional if stored in a closure. Otherwise forwards as-is.
	 * \ingroup GROUP_FUNCTIONAL
	 * \tparam Fn The functional type.
	 * \param fn The functional to be unwrapped.
	 * \return The unwrapped functional.
	 */
	template <class Fn>
	[[nodiscard]]
	constexpr unwrap_functional_t<Fn&&>&& forward_unwrapped(std::remove_reference_t<Fn>& fn) noexcept
	{
		return static_cast<unwrap_functional_t<Fn&&>&&>(std::forward<Fn>(fn));
	}

	/**
	 * \defgroup GROUP_FUNCTIONAL_CLOSURE_TEMPLATE closure_template
	 * \ingroup GROUP_FUNCTIONAL
	 * \brief Trait type, exposing the closure template.
	 * 
	 * \{
	 */

	/**
	 * \brief Primary template, purposely undefined.
	 * \tparam T Given functional type. 
	 */
	template <class T>
	struct closure_template;

	/**
	 * \brief Specialization, providing the closure template as member alias.
	 * \tparam Fn The wrapped functional type.
	 * \tparam InvokePolicy CRTP type, providing different operator () implementations.
	 * \tparam OperatorPolicies CRTP types, providing different composing operators.
	 */
	template <class Fn, template <class> class InvokePolicy, template <class> class... OperatorPolicies>
	struct closure_template<BasicClosure<Fn, InvokePolicy, OperatorPolicies...>>
	{
		template <class NewFn>
		using type = BasicClosure<NewFn, InvokePolicy, OperatorPolicies...>;
	};

	/**
	 * \}
	 */

	/**
	 * \brief Wraps the given functional into the given closure type.
	 * \ingroup GROUP_FUNCTIONAL
	 * \tparam Fn The functional type.
	 * \param fn The functional to be wrapped.
	 * \return The wrapped functional.
	 */
	template <template <class> class Closure, class Fn>
	constexpr Closure<std::remove_cvref_t<Fn>> envelop(
		Fn&& fn
	) noexcept(std::is_nothrow_constructible_v<std::remove_cvref_t<Fn>, Fn>)
	{
		return Closure<std::remove_cvref_t<Fn>>{std::forward<Fn>(fn)};
	}
}

#endif
