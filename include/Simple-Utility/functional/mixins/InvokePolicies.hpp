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
		BasicInvokePolicy(const BasicInvokePolicy&) = default;
		BasicInvokePolicy& operator =(const BasicInvokePolicy&) = default;

		[[nodiscard]]
		BasicInvokePolicy(BasicInvokePolicy&&) = default;
		BasicInvokePolicy& operator =(BasicInvokePolicy&&) = default;

		~BasicInvokePolicy() = default;

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

	template <concepts::unqualified Derived, function InnerFun>
	class NodiscardInvokePolicy
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
		constexpr NodiscardInvokePolicy() noexcept
		{
			static_assert(std::is_base_of_v<NodiscardInvokePolicy, Derived>, "Derived doesn't inherit from BasicInvokePolicy.");
		}

		[[nodiscard]]
		NodiscardInvokePolicy(const NodiscardInvokePolicy&) = default;
		NodiscardInvokePolicy& operator =(const NodiscardInvokePolicy&) = default;

		[[nodiscard]]
		NodiscardInvokePolicy(NodiscardInvokePolicy&&) = default;
		NodiscardInvokePolicy& operator =(NodiscardInvokePolicy&&) = default;

		~NodiscardInvokePolicy() = default;

	public:

		template <class... Args>
			requires std::invocable<const Fun&, Args...>
		[[nodiscard]]
		constexpr decltype(auto) operator ()(Args&&... args) const & noexcept(std::is_nothrow_invocable_v<const Fun&, Args...>)
		{
			return std::invoke(
				forward_unwrapped<const Derived&>(derived()),
				std::forward<Args>(args)...);
		}

		template <class... Args>
			requires std::invocable<Fun&, Args...>
		[[nodiscard]]
		constexpr decltype(auto) operator ()(Args&&... args) & noexcept(std::is_nothrow_invocable_v<Fun&, Args...>)
		{
			return std::invoke(
				forward_unwrapped<Derived&>(derived()),
				std::forward<Args>(args)...);
		}

		template <class... Args>
			requires std::invocable<const Fun&&, Args...>
		[[nodiscard]]
		constexpr decltype(auto) operator ()(Args&&... args) const && noexcept(std::is_nothrow_invocable_v<const Fun&&, Args...>)
		{
			return std::invoke(
				forward_unwrapped<const Derived&&>(derived()),
				std::forward<Args>(args)...);
		}

		template <class... Args>
			requires std::invocable<Fun&&, Args...>
		[[nodiscard]]
		constexpr decltype(auto) operator ()(Args&&... args) && noexcept(std::is_nothrow_invocable_v<Fun&&, Args...>)
		{
			return std::invoke(
				forward_unwrapped<Derived&&>(derived()),
				std::forward<Args>(args)...);
		}
	};

	template <concepts::unqualified Derived, function InnerFun>
	class PredicateInvokePolicy
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
		constexpr PredicateInvokePolicy() noexcept
		{
			static_assert(std::is_base_of_v<PredicateInvokePolicy, Derived>, "Derived doesn't inherit from PredicateInvokePolicy.");
		}

		[[nodiscard]]
		PredicateInvokePolicy(const PredicateInvokePolicy&) = default;
		PredicateInvokePolicy& operator =(const PredicateInvokePolicy&) = default;

		[[nodiscard]]
		PredicateInvokePolicy(PredicateInvokePolicy&&) = default;
		PredicateInvokePolicy& operator =(PredicateInvokePolicy&&) = default;

		~PredicateInvokePolicy() = default;

	public:

		template <class... Args>
			requires std::predicate<const Fun&, Args...>
		[[nodiscard]]
		constexpr decltype(auto) operator ()(Args&&... args) const & noexcept(std::is_nothrow_invocable_v<const Fun&, Args...>)
		{
			return std::invoke(
				forward_unwrapped<const Derived&>(derived()),
				std::forward<Args>(args)...);
		}

		template <class... Args>
			requires std::predicate<Fun&, Args...>
		[[nodiscard]]
		constexpr decltype(auto) operator ()(Args&&... args) & noexcept(std::is_nothrow_invocable_v<Fun&, Args...>)
		{
			return std::invoke(
				forward_unwrapped<Derived&>(derived()),
				std::forward<Args>(args)...);
		}

		template <class... Args>
			requires std::predicate<const Fun&&, Args...>
		[[nodiscard]]
		constexpr decltype(auto) operator ()(Args&&... args) const && noexcept(std::is_nothrow_invocable_v<const Fun&&, Args...>)
		{
			return std::invoke(
				forward_unwrapped<const Derived&&>(derived()),
				std::forward<Args>(args)...);
		}

		template <class... Args>
			requires std::predicate<Fun&&, Args...>
		[[nodiscard]]
		constexpr decltype(auto) operator ()(Args&&... args) && noexcept(std::is_nothrow_invocable_v<Fun&&, Args...>)
		{
			return std::invoke(
				forward_unwrapped<Derived&&>(derived()),
				std::forward<Args>(args)...);
		}
	};
}

#endif
