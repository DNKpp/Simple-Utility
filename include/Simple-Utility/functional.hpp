//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_HPP
#define SL_UTILITY_FUNCTIONAL_HPP

#pragma once

#include <concepts>
#include <functional>
#include <type_traits>

namespace sl::functional::detail
{
	template <class TDerived>
	struct pipe;

	template <class TFunc1, class TFunc2>
	class composition_fn
		: public pipe<composition_fn<TFunc1, TFunc2>>
	{
	public:
		using function_type1 = TFunc1;
		using function_type2 = TFunc2;

		template <class TArg1, class TArg2>
		constexpr composition_fn
		(
			TArg1&& arg1,
			TArg2&& arg2
		)
		noexcept(std::is_nothrow_constructible_v<TFunc1, TArg1>
				&& std::is_nothrow_constructible_v<TFunc2, TArg2>)
			: m_Func1{ std::forward<TArg1>(arg1) },
			m_Func2{ std::forward<TArg2>(arg2) }

		{}

		template <class... TArgs>
		constexpr decltype(auto) operator ()
		(
			TArgs&&... v
		) const
		noexcept(noexcept(std::invoke(
			std::declval<const TFunc2&>(),
			std::invoke(std::declval<const TFunc1&>(), std::forward<TArgs>(v)...)
		)))
		{
			return std::invoke(
				m_Func2,
				std::invoke(m_Func1, std::forward<TArgs>(v)...)
			);
		}

		template <class... TArgs>
		constexpr decltype(auto) operator ()
		(
			TArgs&&... v
		)
		noexcept(noexcept(std::invoke(
			std::declval<TFunc2&>(),
			std::invoke(std::declval<TFunc1&>(), std::forward<TArgs>(v)...)
		)))
		{
			return std::invoke(
				m_Func2,
				std::invoke(m_Func1, std::forward<TArgs>(v)...)
			);
		}

	private:
		[[no_unique_address]]
		TFunc1 m_Func1{};
		[[no_unique_address]]
		TFunc2 m_Func2{};
	};

	template <class TFunc1, class TFunc2>
	composition_fn(TFunc1, TFunc2) -> composition_fn<std::remove_cvref_t<TFunc1>, std::remove_cvref_t<TFunc2>>;

	template <class TDerived>
	struct pipe
	{
		template <class TOther>
		constexpr auto operator |
		(
			TOther&& other
		) && noexcept(noexcept(composition_fn{ static_cast<TDerived&&>(*this), std::forward<TOther>(other) }))
		{
			return composition_fn{
				static_cast<TDerived&&>(*this),
				std::forward<TOther>(other)
			};
		}

		template <class TOther>
		constexpr auto operator |
		(
			TOther&& other
		) const & noexcept(noexcept(composition_fn{ static_cast<const TDerived&>(*this), std::forward<TOther>(other) }))
		{
			return composition_fn{
				static_cast<const TDerived&>(*this),
				std::forward<TOther>(other)
			};
		}

		template <class TLhs>
		friend constexpr auto operator |
		(
			TLhs&& lhs,
			pipe&& rhs
		)
		noexcept(noexcept(composition_fn{ std::forward<TLhs>(lhs), static_cast<TDerived&&>(rhs) }))
			requires (!requires { lhs.operator|(std::move(rhs)); })

		{
			return composition_fn{
				std::forward<TLhs>(lhs),
				static_cast<TDerived&&>(rhs)
			};
		}

		template <class TLhs>
		friend constexpr auto operator |
		(
			TLhs&& lhs,
			const pipe& rhs
		)
		noexcept(noexcept(composition_fn{ std::forward<TLhs>(lhs), static_cast<const TDerived&>(rhs) }))
			requires (!requires { lhs.operator|(rhs); })
		{
			return composition_fn{
				std::forward<TLhs>(lhs),
				static_cast<const TDerived&>(rhs)
			};
		}
	};
}

namespace sl::functional
{
	template <class TFunc>
	class transform_fn
		: public detail::pipe<transform_fn<TFunc>>
	{
	public:
		using function_type = TFunc;

		template <class... TArgs>
			requires std::constructible_from<TFunc, TArgs...>
		explicit constexpr transform_fn
		(
			TArgs&&... args
		)
		noexcept(std::is_nothrow_constructible_v<TFunc, TArgs...>)
			: m_Func{ std::forward<TArgs>(args)... }
		{}

		template <class... TArgs>
		constexpr decltype(auto) operator ()
		(TArgs&&... args) const noexcept(noexcept(std::invoke(std::declval<const TFunc&>(), args...)))
		{
			return std::invoke(m_Func, std::forward<TArgs>(args)...);
		}

		template <class... TArgs>
		constexpr decltype(auto) operator ()(TArgs&&... args) noexcept(noexcept(std::invoke(std::declval<TFunc&>(), args...)))
		{
			return std::invoke(m_Func, std::forward<TArgs>(args)...);
		}

	private:
		[[no_unique_address]]
		TFunc m_Func{};
	};

	template <class TFunc>
	transform_fn(TFunc) -> transform_fn<std::remove_cvref_t<TFunc>>;

	template <class TTarget>
	inline constexpr transform_fn as{
		[]<class T>(T&& v) -> TTarget { return static_cast<TTarget>(std::forward<T>(v)); }
	};

	template <class T>
	inline constexpr transform_fn get{
		[]<class TTuple>(TTuple&& v) -> decltype(auto)
		{
			using std::get;
			return get<T>(std::forward<TTuple>(v));
		}
	};

	template <std::size_t VIndex>
	inline constexpr transform_fn get_at{
		[]<class TTuple>(TTuple&& v) -> decltype(auto)
		{
			using std::get;
			return get<VIndex>(std::forward<TTuple>(v));
		}
	};
}

#endif
