//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_BIND_BACK_HPP
#define SL_UTILITY_FUNCTIONAL_BIND_BACK_HPP

#pragma once

#include "Simple-Utility/Config.hpp"

#include <concepts>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace sl::functional::detail
{
	template <class Fn, class BoundArgsTuple, class... CallArgs>
	constexpr decltype(auto) call_bind_back(Fn&& func, BoundArgsTuple&& boundArgsTuple, CallArgs&&... callArgs)
	{
		return std::apply(
			[&]<class... TArgs>(TArgs&&... boundArgs) -> decltype(auto)
			{
				return std::invoke(
					std::forward<Fn>(func),
					std::forward<CallArgs>(callArgs)...,
					std::forward<TArgs>(boundArgs)...);
			},
			std::forward<BoundArgsTuple>(boundArgsTuple));
	}

	template <class Fn, class... BoundArgs>
	class BindBack
	{
	public:
		template <class FnCTorArg, class... CTorArgs>
			requires (sizeof...(CTorArgs) != 0 && !std::derived_from<std::remove_cvref_t<FnCTorArg>, BindBack>)
		explicit constexpr BindBack(FnCTorArg&& fn, CTorArgs&&... ctorArgs)
			: m_Fn{std::forward<FnCTorArg>(fn)},
			m_BoundArgs{std::forward<CTorArgs>(ctorArgs)...}
		{
		}

		template <class... CallArgs>
			requires std::invocable<const Fn&, CallArgs..., const BoundArgs&...>
		constexpr decltype(auto) operator()(
			CallArgs&&... callArgs
		) const & noexcept(std::is_nothrow_invocable_v<const Fn&, CallArgs..., const BoundArgs&...>)
		{
			return call_bind_back(m_Fn, m_BoundArgs, std::forward<CallArgs>(callArgs)...);
		}

		template <class... CallArgs>
			requires std::invocable<Fn&, CallArgs..., BoundArgs&...>
		constexpr decltype(auto) operator()(
			CallArgs&&... callArgs
		) & noexcept(std::is_nothrow_invocable_v<Fn&, CallArgs..., BoundArgs&...>)
		{
			return call_bind_back(m_Fn, m_BoundArgs, std::forward<CallArgs>(callArgs)...);
		}

		template <class... CallArgs>
			requires std::invocable<const Fn&&, CallArgs..., const BoundArgs&&...>
		constexpr decltype(auto) operator()(
			CallArgs&&... callArgs
		) const && noexcept(std::is_nothrow_invocable_v<const Fn&&, CallArgs..., const BoundArgs&&...>)
		{
			return call_bind_back(std::move(m_Fn), std::move(m_BoundArgs), std::forward<CallArgs>(callArgs)...);
		}

		template <class... CallArgs>
			requires std::invocable<Fn&&, CallArgs..., BoundArgs&&...>
		constexpr decltype(auto) operator()(
			CallArgs&&... callArgs
		) && noexcept(std::is_nothrow_invocable_v<Fn&&, CallArgs..., BoundArgs&&...>)
		{
			return call_bind_back(std::move(m_Fn), std::move(m_BoundArgs), std::forward<CallArgs>(callArgs)...);
		}

	private:
		SL_UTILITY_NO_UNIQUE_ADDRESS
		Fn m_Fn{};
		std::tuple<BoundArgs...> m_BoundArgs{};
	};

	template <class TFunc, class... TBoundArgs>
	BindBack(TFunc, TBoundArgs...) -> BindBack<TFunc, std::unwrap_ref_decay_t<TBoundArgs>...>;
}

namespace sl::functional
{
	/**
	 * \brief Helper function, which generates a forwarding call wrapper for the given function and curries
	 * the params beginning at the back.
	 */
	inline constexpr auto bind_back = []<class Fn, class... BoundArgs>(
		Fn&& func,
		BoundArgs&&... boundArgs
	)
		noexcept(std::is_nothrow_constructible_v<std::remove_cvref_t<Fn>, Fn>
				&& (... && std::is_nothrow_constructible_v<std::unwrap_ref_decay_t<BoundArgs>, BoundArgs>))
	{
		return detail::BindBack{std::forward<Fn>(func), std::forward<BoundArgs>(boundArgs)...};
	};
}

#endif
