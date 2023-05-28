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

	template <function Fn, template <class> class InvokePolicy, template <class> class... OperatorPolicies>
		requires (... && operator_policy<OperatorPolicies>)
	class BasicClosure
		: public InvokePolicy<BasicClosure<Fn, InvokePolicy, OperatorPolicies...>>,
		public OperatorPolicies<BasicClosure<Fn, InvokePolicy, OperatorPolicies...>>...
	{
	public:
		template <class... Args>
			requires std::constructible_from<Fn, Args...>
		explicit constexpr BasicClosure(Args&&... args) noexcept(std::is_nothrow_constructible_v<Fn, Args...>)
			: m_Fn{std::forward<Args>(args)...}
		{
		}

		BasicClosure(const BasicClosure&)
			requires std::copy_constructible<Fn> = default;

		BasicClosure& operator =(const BasicClosure&)
			requires std::is_copy_assignable_v<Fn> = default;

		BasicClosure(BasicClosure&&)
			requires std::move_constructible<Fn> = default;

		BasicClosure& operator =(BasicClosure&&)
			requires std::is_move_assignable_v<Fn> = default;

		~BasicClosure() = default;

		[[nodiscard]]
		explicit constexpr operator const Fn&() const & noexcept
		{
			return m_Fn;
		}

		[[nodiscard]]
		explicit constexpr operator Fn&() & noexcept
		{
			return m_Fn;
		}

		[[nodiscard]]
		explicit constexpr operator const Fn&&() const && noexcept
		{
			return std::move(m_Fn);
		}

		[[nodiscard]]
		explicit constexpr operator Fn&&() && noexcept
		{
			return std::move(m_Fn);
		}

	private:
		Fn m_Fn{};
	};

	template <class T>
	struct unwrap_functional
	{
		using type = T;
	};

	template <class Fn, template <class> class InvokePolicy, template <class> class... OperatorPolicies>
	struct unwrap_functional<BasicClosure<Fn, InvokePolicy, OperatorPolicies...>>;

	template <class Fn, template <class> class InvokePolicy, template <class> class... OperatorPolicies>
	struct unwrap_functional<const BasicClosure<Fn, InvokePolicy, OperatorPolicies...>>;

	template <class Fn, template <class> class InvokePolicy, template <class> class... OperatorPolicies>
	struct unwrap_functional<const BasicClosure<Fn, InvokePolicy, OperatorPolicies...>&>
	{
		using type = const Fn&;
	};

	template <class Fn, template <class> class InvokePolicy, template <class> class... OperatorPolicies>
	struct unwrap_functional<BasicClosure<Fn, InvokePolicy, OperatorPolicies...>&>
	{
		using type = Fn&;
	};

	template <class Fn, template <class> class InvokePolicy, template <class> class... OperatorPolicies>
	struct unwrap_functional<const BasicClosure<Fn, InvokePolicy, OperatorPolicies...>&&>
	{
		using type = const Fn&&;
	};

	template <class Fn, template <class> class InvokePolicy, template <class> class... OperatorPolicies>
	struct unwrap_functional<BasicClosure<Fn, InvokePolicy, OperatorPolicies...>&&>
	{
		using type = Fn&&;
	};

	template <class T>
	using unwrap_functional_t = typename unwrap_functional<T>::type;

	template <class Fn>
	[[nodiscard]]
	constexpr unwrap_functional_t<Fn&&>&& forward_unwrapped(std::remove_reference_t<Fn>& fn) noexcept
	{
		return static_cast<unwrap_functional_t<Fn&&>&&>(std::forward<Fn>(fn));
	}

	template <class T>
	struct closure_template;

	template <class Fn, template <class> class InvokePolicy, template <class> class... OperatorPolicies>
	struct closure_template<BasicClosure<Fn, InvokePolicy, OperatorPolicies...>>
	{
		template <class NewFn>
		using type = BasicClosure<NewFn, InvokePolicy, OperatorPolicies...>;
	};

	template <template <class> class Closure, class Fn>
	constexpr Closure<std::remove_cvref_t<Fn>> envelop(
		Fn&& fn
	) noexcept(std::is_nothrow_constructible_v<std::remove_cvref_t<Fn>, Fn>)
	{
		return Closure<std::remove_cvref_t<Fn>>{std::forward<Fn>(fn)};
	}
}

#endif
