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
					&& requires(TAdapted t) { { *std::forward<TAdapted>(t) } -> concepts::not_same_as<void>; }
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
			constexpr decltype(auto) operator()
			(
				TAdapted&& adapted
			) const
			noexcept(noexcept(unwrap_adapted(std::forward<TAdapted>(adapted))))
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

	struct in_place_null_t
	{};

	inline constexpr in_place_null_t in_place_null{};

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

	namespace detail
	{
		template <class T>
		concept convertible_to_adapter = requires(T&& arg)
		{
			adapter{ nullables::to_nullables_adapter(std::forward<T>(arg)) };
		};
	}

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

		template <concepts::initializes<null_type> TNullArg>
			requires std::constructible_from<adapted_type, const null_type&>
		[[nodiscard]]
		explicit
		constexpr adapter
		(
			[[maybe_unused]] in_place_null_t,
			TNullArg&& nullArg
		)
		noexcept(std::is_nothrow_constructible_v<null_type, TNullArg>
				&& std::is_nothrow_constructible_v<adapted_type, const null_type&>)
			: m_Null{ std::forward<TNullArg>(nullArg) },
			m_Adapted{ m_Null }
		{
			assert(*this == adapter_null && "Default constructed adapted_type must comapare equally to the provided null object.");
		}

		[[nodiscard]]
		explicit
		constexpr adapter
		(
			[[maybe_unused]] adapter_null_t
		)
		noexcept(std::is_nothrow_default_constructible_v<null_type>
				&& std::is_nothrow_constructible_v<adapted_type, null_type&>)
			requires std::default_initializable<null_type>
					&& std::constructible_from<adapted_type, null_type&>
			: adapter{ in_place_null, null_type{} }
		{}

		template <concepts::initializes<null_type> TNullArg>
			requires concepts::not_same_as<adapter_null_t, std::remove_cvref_t<TNullArg>>
					&& concepts::not_same_as<in_place_null_t, std::remove_cvref_t<TNullArg>>
					&& (!detail::convertible_to_adapter<TNullArg>)
					&& std::constructible_from<adapted_type, const null_type&>
		[[nodiscard]]
		explicit
		constexpr adapter
		(
			TNullArg&& nullArg
		)
		noexcept(std::is_nothrow_constructible_v<adapter, std::in_place_t, TNullArg>)
			: adapter{ in_place_null, nullArg }
		{}

		template <concepts::initializes<null_type> TNullArg, concepts::initializes<adapted_type> TAdaptedArg>
			requires concepts::not_same_as<adapter_null_t, std::remove_cvref_t<TNullArg>>
					&& concepts::not_same_as<in_place_null_t, std::remove_cvref_t<TNullArg>>
					&& concepts::not_same_as<adapter_null_t, std::remove_cvref_t<TAdaptedArg>>
					&& concepts::not_same_as<in_place_null_t, std::remove_cvref_t<TAdaptedArg>>
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

		template <detail::convertible_to_adapter TArg>
			requires concepts::not_same_as<adapter_null_t, std::remove_cvref_t<TArg>>
					&& concepts::not_same_as<in_place_null_t, std::remove_cvref_t<TArg>>
		[[nodiscard]]
		explicit
		constexpr adapter
		(
			TArg&& arg
		)
		noexcept(noexcept(to_nullables_adapter(std::forward<TArg>(arg)))
				&& std::is_nothrow_constructible_v<adapter>)
			: adapter{ to_nullables_adapter(std::forward<TArg>(arg)) }
		{ }

		constexpr adapter& operator =
		(
			[[maybe_unused]] adapter_null_t
		)
		noexcept(std::is_nothrow_assignable_v<adapted_type&, null_type&>)
			requires std::assignable_from<adapted_type&, null_type&>
		{
			m_Adapted = m_Null;

			return *this;
		}

		template <concepts::assignable_to<adapted_type&> TAdaptedArg>
			requires concepts::not_same_as<adapter_null_t, std::remove_cvref_t<TAdaptedArg>>
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
			const adapter& other
		) const
		noexcept(concepts::nothrow_weakly_equality_comparable_with<TAdapted, TNull>)
		= default;

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
