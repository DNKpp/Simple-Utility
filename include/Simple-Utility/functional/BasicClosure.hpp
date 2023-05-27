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
	template <class T>
	concept function = concepts::unqualified<T>
						&& std::is_object_v<T>;

	template <template <class> class T>
	concept operator_policy = true;

	template <function Fun, template <class, class> class InvokablePolicy, template <class> class... OperatorPolicies>
		requires (... && operator_policy<OperatorPolicies>)
	class BasicClosure
		: public InvokablePolicy<BasicClosure<Fun, InvokablePolicy, OperatorPolicies...>, Fun>,
		public OperatorPolicies<BasicClosure<Fun, InvokablePolicy, OperatorPolicies...>>...
	{
	public:
		template <class... Args>
			requires std::constructible_from<Fun, Args...>
		explicit constexpr BasicClosure(Args&&... args) noexcept(std::is_nothrow_constructible_v<Fun, Args...>)
			: m_Fun{std::forward<Args>(args)...}
		{
		}

		BasicClosure(const BasicClosure&)
			requires std::copy_constructible<Fun> = default;

		BasicClosure& operator =(const BasicClosure&)
			requires std::is_copy_assignable_v<Fun> = default;

		BasicClosure(BasicClosure&&)
			requires std::move_constructible<Fun> = default;

		BasicClosure& operator =(BasicClosure&&)
			requires std::is_move_assignable_v<Fun> = default;

		~BasicClosure() = default;

		explicit constexpr operator const Fun&() const & noexcept
		{
			return m_Fun;
		}

		explicit constexpr operator Fun&() & noexcept
		{
			return m_Fun;
		}

		explicit constexpr operator const Fun&&() const && noexcept
		{
			return std::move(m_Fun);
		}

		explicit constexpr operator Fun&&() && noexcept
		{
			return std::move(m_Fun);
		}

	private:
		Fun m_Fun{};
	};

	template <class T>
	struct unwrap_functional
	{
		using type = T;
	};

	template <class Fun, template <class, class> class InvokablePolicy, template <class> class... OperatorPolicies>
	struct unwrap_functional<const BasicClosure<Fun, InvokablePolicy, OperatorPolicies...>&>
	{
		using type = const Fun&;
	};

	template <class Fun, template <class, class> class InvokablePolicy, template <class> class... OperatorPolicies>
	struct unwrap_functional<BasicClosure<Fun, InvokablePolicy, OperatorPolicies...>&>
	{
		using type = Fun&;
	};

	template <class Fun, template <class, class> class InvokablePolicy, template <class> class... OperatorPolicies>
	struct unwrap_functional<const BasicClosure<Fun, InvokablePolicy, OperatorPolicies...>&&>
	{
		using type = const Fun&&;
	};

	template <class Fun, template <class, class> class InvokablePolicy, template <class> class... OperatorPolicies>
	struct unwrap_functional<BasicClosure<Fun, InvokablePolicy, OperatorPolicies...>&&>
	{
		using type = Fun&&;
	};

	template <class T>
	using unwrap_functional_t = typename unwrap_functional<T>::type;

	template <class Fn>
	[[nodiscard]]
	constexpr unwrap_functional_t<Fn>&& forward_unwrapped(std::remove_reference_t<Fn>& fn) noexcept
	{
		return static_cast<unwrap_functional_t<Fn>&&>(std::forward<Fn>(fn));
	}

	template <class T>
	struct closure_template;

	template <class Fn, template <class, class> class InvokablePolicy, template <class> class... OperatorPolicies>
	struct closure_template<BasicClosure<Fn, InvokablePolicy, OperatorPolicies...>>
	{
		template <class NewFn>
		using type = BasicClosure<NewFn, InvokablePolicy, OperatorPolicies...>;
	};

	template <template <class> class Closure, class Fn>
	constexpr Closure<std::remove_cvref_t<Fn>> enclose_functional(
		Fn&& fn
	) noexcept(std::is_nothrow_constructible_v<std::remove_cvref_t<Fn>, Fn>)
	{
		return Closure<std::remove_cvref_t<Fn>>{std::forward<Fn>(fn)};
	}
}

#endif
