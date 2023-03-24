//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_BIND_BACK_HPP
#define SL_UTILITY_BIND_BACK_HPP

#pragma once

#include "Simple-Utility/Config.hpp"

#include <concepts>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace sl::detail
{
	template <class TFunc, class TBoundArgsTuple, class... TCallArgs>
	decltype(auto) bind_back_caller(TFunc&& func, TBoundArgsTuple&& boundArgsTuple, TCallArgs&&... callArgs)
	{
		return std::apply(
			[&]<class... TArgs>(TArgs&&... boundArgs) -> decltype(auto)
			{
				return std::invoke(
					std::forward<TFunc>(func),
					std::forward<TCallArgs>(callArgs)...,
					std::forward<TArgs>(boundArgs)...
				);
			},
			std::forward<TBoundArgsTuple>(boundArgsTuple)
		);
	}

	template <class TFunc, class... TBoundArgs>
	class bind_back_fn
	{
	public:
		template <class UFunc, class... UBoundArgs>
			requires (sizeof...(UBoundArgs) != 0 && !std::derived_from<std::decay_t<UFunc>, bind_back_fn>)
		explicit bind_back_fn(UFunc&& func, UBoundArgs&&... ba)
			: m_Func(std::forward<UFunc>(func)),
			m_BoundArgs(std::forward<UBoundArgs>(ba)...)
		{}

		template <class... TCallArgs>
		std::invoke_result_t<TFunc&, TCallArgs..., TBoundArgs&...> operator()
		(
			TCallArgs&&... callArgs
		) & noexcept(std::is_nothrow_invocable_v<TFunc&, TCallArgs..., TBoundArgs&...>)
		{
			return bind_back_caller(m_Func, m_BoundArgs, std::forward<TCallArgs>(callArgs)...);
		}

		template <class... TCallArgs>
		std::invoke_result_t<const TFunc&, TCallArgs..., const TBoundArgs&...> operator()
		(
			TCallArgs&&... callArgs
		) const & noexcept(std::is_nothrow_invocable_v<const TFunc&, TCallArgs..., const TBoundArgs&...>)
		{
			return bind_back_caller(m_Func, m_BoundArgs, std::forward<TCallArgs>(callArgs)...);
		}

		template <class... TCallArgs>
		std::invoke_result_t<TFunc, TCallArgs..., TBoundArgs...> operator()
		(
			TCallArgs&&... callArgs
		) && noexcept(std::is_nothrow_invocable_v<TFunc, TCallArgs..., TBoundArgs...>)
		{
			return bind_back_caller(std::move(m_Func), std::move(m_BoundArgs), std::forward<TCallArgs>(callArgs)...);
		}

	private:
		SL_UTILITY_NO_UNIQUE_ADDRESS
		TFunc m_Func;
		std::tuple<TBoundArgs...> m_BoundArgs;
	};

	template <class TFunc, class... TBoundArgs>
	bind_back_fn(TFunc, TBoundArgs ...) -> bind_back_fn<std::remove_cvref_t<TFunc>, std::unwrap_ref_decay_t<TBoundArgs>...>;
}

namespace sl
{
	/**
	 * \brief Helper function, which generates a forwarding call wrapper for the given function and curries
	 * the params beginning at the back.
	 */
	inline constexpr auto bind_back = []<class TFunc, class... TBoundArgs>(TFunc&& func, TBoundArgs&&... boundArgs)
	{
		return detail::bind_back_fn{ std::forward<TFunc>(func), std::forward<TBoundArgs>(boundArgs)... };
	};
}

#endif
