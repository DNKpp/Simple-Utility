//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_FUNCTIONAL_MIXIN_INVOKE_POLICIES_HPP
#define SIMPLE_UTILITY_FUNCTIONAL_MIXIN_INVOKE_POLICIES_HPP

#pragma once

namespace sl::functional
{
	template <concepts::unqualified Derived, function InnerFun>
	class BasicInvokePolicy
	{
	private:
		[[nodiscard]]
		constexpr const Derived& derived() const noexcept
		{
			return static_cast<const Derived&>(*this);
		}

		[[nodiscard]]
		constexpr Derived& derived() noexcept
		{
			return static_cast<Derived&>(*this);
		}

		using Fun = InnerFun;

	protected:
		constexpr BasicInvokePolicy() noexcept
		{
			static_assert(std::is_base_of_v<BasicInvokePolicy, Derived>, "Derived doesn't inherit from BasicInvokePolicy.");
		}

		[[nodiscard]]
		constexpr BasicInvokePolicy(const BasicInvokePolicy&) noexcept = default;
		constexpr BasicInvokePolicy& operator =(const BasicInvokePolicy&) noexcept = default;

		[[nodiscard]]
		constexpr BasicInvokePolicy(BasicInvokePolicy&&) noexcept = default;
		constexpr BasicInvokePolicy& operator =(BasicInvokePolicy&&) noexcept = default;

		constexpr ~BasicInvokePolicy() noexcept = default;

	public:

		template <class... Args>
			requires std::invocable<const Fun&, Args...>
		constexpr decltype(auto) operator ()(Args&&... args) const & noexcept(std::is_nothrow_invocable_v<const Fun&, Args...>)
		{
			return std::invoke(
				forward_unwrapped<const Derived&>(derived()),
				std::forward<Args>(args)...);
		}

		template <class... Args>
			requires std::invocable<Fun&, Args...>
		constexpr decltype(auto) operator ()(Args&&... args) & noexcept(std::is_nothrow_invocable_v<Fun&, Args...>)
		{
			return std::invoke(
				forward_unwrapped<Derived&>(derived()),
				std::forward<Args>(args)...);
		}

		template <class... Args>
			requires std::invocable<const Fun&&, Args...>
		constexpr decltype(auto) operator ()(Args&&... args) const && noexcept(std::is_nothrow_invocable_v<const Fun&&, Args...>)
		{
			return std::invoke(
				forward_unwrapped<const Derived&&>(derived()),
				std::forward<Args>(args)...);
		}

		template <class... Args>
			requires std::invocable<Fun&&, Args...>
		constexpr decltype(auto) operator ()(Args&&... args) && noexcept(std::is_nothrow_invocable_v<Fun&&, Args...>)
		{
			return std::invoke(
				forward_unwrapped<Derived&&>(derived()),
				std::forward<Args>(args)...);
		}
	};
}

#endif
