//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_NULLABLES_ADAPTER_HPP
#define SL_UTILITY_NULLABLES_ADAPTER_HPP

#pragma once

#include "Simple-Utility/nullables/base.hpp"
#include "Simple-Utility/concepts/stl_extensions.hpp"

#include <cassert>
#include <ranges>

namespace sl::nullables
{
	namespace detail
	{
		template <class TAdapted>
		   requires concepts::dereferencable<TAdapted>
				   && requires(TAdapted t){ { *std::forward<TAdapted>(t) } -> concepts::not_same_as<void>; }
		[[nodiscard]]
		constexpr decltype(auto) unwrap_adapted(TAdapted&& adapted)
		{
			return *std::forward<TAdapted>(adapted);
		}

		struct unwrap_adapted_fn
		{
			template <class TAdapted>
				requires requires { unwrap_adapted(std::declval<TAdapted>()); }
			[[nodiscard]]
			constexpr auto operator()(TAdapted&& adapted) const noexcept(noexcept(unwrap_adapted(std::forward<TAdapted>(adapted))))
			{
				return unwrap_adapted(std::forward<TAdapted>(adapted));
			}
		};
	}

	inline constexpr detail::unwrap_adapted_fn unwrap_adapted{};
}

namespace sl::nullables
{
	struct adapter_null_t
	{};

	inline constexpr adapter_null_t adapter_null{};

	template <class T>
	using adapted_value_t = std::remove_cvref_t<decltype(unwrap_adapted(std::declval<T>()))>;

	template <class TAdapted, class TNull>
	concept adaptable_with = std::movable<TNull>
							&& std::movable<TAdapted>
							&& concepts::weakly_equality_comparable_with<TAdapted, TNull>
							&& requires(TAdapted&& t)
							{
								typename adapted_value_t<TAdapted>;
								{ unwrap_adapted(std::forward<TAdapted>(t)) } -> std::convertible_to<adapted_value_t<TAdapted>>;
							};

	template <class TNull, adaptable_with<TNull> TAdapted>
		requires std::same_as<TNull, std::remove_cvref_t<TNull>>
				&& std::same_as<TAdapted, std::remove_cvref_t<TAdapted>>
	class adapter;
}

namespace sl::nullables::detail
{
	template <std::ranges::borrowed_range TRange>
	[[nodiscard]]
	constexpr adapter<std::ranges::sentinel_t<TRange>, std::ranges::iterator_t<TRange>> to_nullables_adapter(TRange&& range)
	{
		return { std::ranges::end(range), std::ranges::begin(range) };
	}

	struct to_nullables_adapter_fn
	{
		template <class TArg>
			requires requires { adapter{ to_nullables_adapter(std::declval<TArg>()) }; }
		[[nodiscard]]
		constexpr auto operator()(TArg&& arg) const noexcept(noexcept(to_nullables_adapter(std::forward<TArg>(arg))))
		{
			return to_nullables_adapter(std::forward<TArg>(arg));
		}
	};
}

namespace sl::nullables
{
	inline constexpr detail::to_nullables_adapter_fn to_nullables_adapter{};

	template <class TNull, adaptable_with<TNull> TAdapted>
		requires std::same_as<TNull, std::remove_cvref_t<TNull>>
				&& std::same_as<TAdapted, std::remove_cvref_t<TAdapted>>
	class adapter
	{
	public:
		using adapted_type = TAdapted;
		using value_type = adapted_value_t<TAdapted>;
		using null_type = TNull;

		constexpr ~adapter() noexcept = default;

		[[nodiscard]]
		constexpr adapter
		(
			const adapter& other
		) noexcept(std::is_nothrow_copy_constructible_v<TNull>
					&& std::is_nothrow_copy_constructible_v<TAdapted>)
		= default;

		constexpr adapter& operator =
		(
			const adapter& other
		) noexcept(std::is_nothrow_copy_assignable_v<TNull>
					&& std::is_nothrow_copy_assignable_v<TAdapted>)
		= default;

		[[nodiscard]]
		constexpr adapter
		(
			adapter&& other
		) noexcept(std::is_nothrow_move_constructible_v<TNull>
					&& std::is_nothrow_move_constructible_v<TAdapted>)
		= default;

		constexpr adapter& operator =
		(
			adapter&& other
		) noexcept(std::is_nothrow_move_assignable_v<TNull>
					&& std::is_nothrow_move_assignable_v<TAdapted>)
		= default;

		//template <concepts::initializes<null_type> TNullArg>
		//	requires std::constructible_from<adapted_type, const null_type&>
		//			//&& !requires(TNullArg&& arg)
		//			//{
		//			//	{ to_nullables_adapter(std::forward<TNullArg>(arg)) } -> std::convertible_to<adapter>;
		//			//}
		//[[nodiscard]]
		//explicit
		//constexpr adapter
		//(
		//	TNullArg&& nullArg
		//)
		//noexcept(std::is_nothrow_constructible_v<null_type, TNullArg>
		//		&& std::is_nothrow_constructible_v<adapted_type, const null_type&>)
		//	: m_Null{ std::forward<TNullArg>(nullArg) },
		//	m_Adapted{ m_Null }
		//{
		//	assert(*this == adapter_null && "Default constructed adapted_type must comapare equally to the provided null object.");
		//}

		template <class TArg>
			requires requires(TArg&& arg)
			{
				{ to_nullables_adapter(std::forward<TArg>(arg)) } -> std::convertible_to<adapter>;
			}
		[[nodiscard]]
		explicit
		constexpr adapter(TArg&& arg)
			: adapter{ to_nullables_adapter(std::forward<TArg>(arg)) }
		{ }

		template <concepts::initializes<null_type> TNullArg, concepts::initializes<adapted_type> TAdaptedArg>
		[[nodiscard]]
		constexpr adapter
		(
			TNullArg&& nullArg,
			TAdaptedArg&& adaptedArg
		)
		noexcept(std::is_nothrow_constructible_v<null_type, TNullArg>
				&& std::is_nothrow_constructible_v<adapted_type, TAdaptedArg>)
			: m_Null{ std::forward<TNullArg>(nullArg) },
			m_Adapted{ std::forward<TAdaptedArg>(adaptedArg) }
		{}

		template <concepts::assignable_to<adapted_type&> TAdaptedArg>
		constexpr adapter& operator =
		(
			TAdaptedArg&& adaptedArg
		)
		noexcept(std::is_nothrow_assignable_v<adapted_type&, TAdaptedArg>)
		{
			m_Adapted = std::forward<TAdaptedArg>(adaptedArg);

			return *this;
		}

		[[nodiscard]]
		constexpr decltype(auto) operator *() const &
		{
			assert(*this != adapter_null && "Dereferenced adapted nullable is equally to its null.");

			return unwrap_adapted(m_Adapted);
		}

		[[nodiscard]]
		constexpr decltype(auto) operator *() &
		{
			assert(*this != adapter_null && "Dereferenced adapted nullable is equally to its null.");

			return unwrap_adapted(m_Adapted);
		}

		[[nodiscard]]
		constexpr decltype(auto) operator *() &&
		{
			assert(*this != adapter_null && "Dereferenced adapted nullable is equally to its null.");

			return unwrap_adapted(std::move(m_Adapted));
		}

		[[nodiscard]]
		constexpr bool operator ==
		(
			adapter_null_t
		) const
		noexcept(concepts::nothrow_weakly_equality_comparable_with<TAdapted, TNull>)
		{
			return m_Adapted == m_Null;
		}

	private:
		[[no_unique_address]]
		null_type m_Null{};
		adapted_type m_Adapted{};
	};

	template <class TNull, class TAdapted>
	adapter(TNull, TAdapted) -> adapter<TNull, TAdapted>;

	template <class T>
		requires requires { to_nullables_adapter(std::declval<T>()); }
	adapter
	(
		T&& t
	) -> adapter<
		typename std::remove_cvref_t<decltype(to_nullables_adapter(std::declval<T>()))>::null_type,
		typename std::remove_cvref_t<decltype(to_nullables_adapter(std::declval<T>()))>::adapted_type
	>;

	template <class TNull, class TAdapted>
	struct traits<adapter<TNull, TAdapted>>
	{
		using value_type = adapted_value_t<TAdapted>;
		constexpr static adapter_null_t null{ adapter_null };
	};
}

#endif
