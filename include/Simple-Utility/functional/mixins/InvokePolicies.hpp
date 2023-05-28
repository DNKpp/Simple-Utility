//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_FUNCTIONAL_MIXIN_INVOKE_POLICIES_HPP
#define SIMPLE_UTILITY_FUNCTIONAL_MIXIN_INVOKE_POLICIES_HPP

#pragma once

#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>

#include "Simple-Utility/CRTPBase.hpp"
#include "Simple-Utility/concepts/stl_extensions.hpp"
#include "Simple-Utility/functional/BasicClosure.hpp"
#include "Simple-Utility/tuple/General.hpp"

namespace sl::functional::detail
{
	template <concepts::unqualified Derived, class InvokeStrategy>
	class BasicInvokePolicy
		: public CRTPBase<Derived>
	{
	private:
		using CRTPBase<Derived>::derived;

	public:
		template <class... Args>
			requires std::invocable<InvokeStrategy, const Derived&, Args...>
		constexpr decltype(auto) operator ()(
			Args&&... args
		) const & noexcept(std::is_nothrow_invocable_v<InvokeStrategy, const Derived&, Args...>)
		{
			return std::invoke(InvokeStrategy{}, derived(), std::forward<Args>(args)...);
		}

		template <class... Args>
			requires std::invocable<InvokeStrategy, Derived&, Args...>
		constexpr decltype(auto) operator ()(Args&&... args) & noexcept(std::is_nothrow_invocable_v<InvokeStrategy, Derived&, Args...>)
		{
			return std::invoke(InvokeStrategy{}, derived(), std::forward<Args>(args)...);
		}

		template <class... Args>
			requires std::invocable<InvokeStrategy, const Derived&&, Args...>
		constexpr decltype(auto) operator ()(
			Args&&... args
		) const && noexcept(std::is_nothrow_invocable_v<InvokeStrategy, const Derived&&, Args...>)
		{
			return std::invoke(InvokeStrategy{}, std::move(derived()), std::forward<Args>(args)...);
		}

		template <class... Args>
			requires std::invocable<InvokeStrategy, Derived&&, Args...>
		constexpr decltype(auto) operator ()(
			Args&&... args
		) && noexcept(std::is_nothrow_invocable_v<InvokeStrategy, Derived&&, Args...>)
		{
			return std::invoke(InvokeStrategy{}, std::move(derived()), std::forward<Args>(args)...);
		}
	};

	template <concepts::unqualified Derived, class InvokeStrategy>
	class NodiscardInvokePolicy
		: public CRTPBase<Derived>
	{
	private:
		using CRTPBase<Derived>::derived;

	public:
		template <class... Args>
			requires std::invocable<InvokeStrategy, const Derived&, Args...>
		[[nodiscard]]
		constexpr decltype(auto) operator ()(
			Args&&... args
		) const & noexcept(std::is_nothrow_invocable_v<InvokeStrategy, const Derived&, Args...>)
		{
			return std::invoke(InvokeStrategy{}, derived(), std::forward<Args>(args)...);
		}

		template <class... Args>
			requires std::invocable<InvokeStrategy, Derived&, Args...>
		[[nodiscard]]
		constexpr decltype(auto) operator ()(Args&&... args) & noexcept(std::is_nothrow_invocable_v<InvokeStrategy, Derived&, Args...>)
		{
			return std::invoke(InvokeStrategy{}, derived(), std::forward<Args>(args)...);
		}

		template <class... Args>
			requires std::invocable<InvokeStrategy, const Derived&&, Args...>
		[[nodiscard]]
		constexpr decltype(auto) operator ()(
			Args&&... args
		) const && noexcept(std::is_nothrow_invocable_v<InvokeStrategy, const Derived&&, Args...>)
		{
			return std::invoke(InvokeStrategy{}, std::move(derived()), std::forward<Args>(args)...);
		}

		template <class... Args>
			requires std::invocable<InvokeStrategy, Derived&&, Args...>
		[[nodiscard]]
		constexpr decltype(auto) operator ()(
			Args&&... args
		) && noexcept(std::is_nothrow_invocable_v<InvokeStrategy, Derived&&, Args...>)
		{
			return std::invoke(InvokeStrategy{}, std::move(derived()), std::forward<Args>(args)...);
		}
	};

	struct InvocableStrategy
	{
		template <class Fn, class... Args>
			requires std::invocable<unwrap_functional_t<Fn&&>, Args...>
		[[nodiscard]]
		constexpr std::invoke_result_t<unwrap_functional_t<Fn&&>, Args...> operator ()(
			Fn&& fn,
			Args&&... args
		) const noexcept(std::is_nothrow_invocable_v<unwrap_functional_t<Fn&&>, Args...>)
		{
			return std::invoke(
				forward_unwrapped<Fn>(fn),
				std::forward<Args>(args)...);
		}
	};

	struct PredicateStrategy
	{
		template <class Fn, class... Args>
			requires std::predicate<unwrap_functional_t<Fn&&>, Args...>
		[[nodiscard]]
		constexpr std::invoke_result_t<unwrap_functional_t<Fn&&>, Args...> operator ()(
			Fn&& fn,
			Args&&... args
		) const noexcept(std::is_nothrow_invocable_v<unwrap_functional_t<Fn&&>, Args...>)
		{
			return std::invoke(
				forward_unwrapped<Fn>(fn),
				std::forward<Args>(args)...);
		}
	};

	struct ApplicableStrategy
	{
		template <class Fn, class Tuple>
			requires concepts::applicable<unwrap_functional_t<Fn&&>, Tuple>
		[[nodiscard]]
		constexpr decltype(auto) operator ()(
			Fn&& fn,
			Tuple&& t
		) const noexcept(concepts::nothrow_applicable<unwrap_functional_t<Fn&&>, Tuple>)
		{
			return std::apply(
				forward_unwrapped<Fn>(fn),
				std::forward<Tuple>(t));
		}
	};
}

namespace sl::functional
{
	template <concepts::unqualified Derived>
	using BasicInvokePolicy = detail::BasicInvokePolicy<Derived, detail::InvocableStrategy>;

	template <concepts::unqualified Derived>
	using NodiscardInvokePolicy = detail::NodiscardInvokePolicy<Derived, detail::InvocableStrategy>;

	template <concepts::unqualified Derived>
	using PredicateInvokePolicy = detail::NodiscardInvokePolicy<Derived, detail::PredicateStrategy>;

	template <concepts::unqualified Derived>
	using BasicApplyPolicy = detail::NodiscardInvokePolicy<Derived, detail::ApplicableStrategy>;

	template <concepts::unqualified Derived>
	using NodiscardApplyPolicy = detail::NodiscardInvokePolicy<Derived, detail::ApplicableStrategy>;
}

#endif
