//          Copyright Dominic Koepke 2019 - 2023.
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
					&& requires{ { *std::declval<TAdapted>() } -> concepts::not_same_as<void>; }
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
	/**
	 * \defgroup GROUP_NULLABLES_ADAPTER adapter
	 * \ingroup GROUP_NULLABLES
	 *  @{
	 */

	/**
	 * \brief Dedicated null type for \ref sl::nullables::adapter "adapters".
	 * \relates sl::nullables::adapter
	 */
	struct adapter_null_t
	{};

	/**
	 * \brief Dedicated null object for \ref sl::nullables::adapter "adapters".
	 * \relates sl::nullables::adapter
	 */
	inline constexpr adapter_null_t adapter_null{};

	/**
	 * \brief Tag type for \ref sl::nullables::adapter "adapters", which can be used to disambiguate the construction
	 * with just a null-object.
	 * \relates sl::nullables::adapter
	 */
	struct in_place_null_t
	{};

	/**
	 * \brief Tag object for \ref sl::nullables::adapter "adapters", which can be used to disambiguate the construction
	 * with just a null-object.
	 * \relates sl::nullables::adapter
	 */
	inline constexpr in_place_null_t in_place_null{};

	template <class T>
	using adapted_value_t = std::remove_cvref_t<decltype(unwrap_adapted(std::declval<T>()))>;

	/**
	 * \brief Determines whether the given adapted and null type satisfy the requirements to be used within a \ref sl::nullables::adapter "adapter".
	 * \tparam TAdapted The adapted type.
	 * \tparam TNull The null type.
	 */
	template <class TAdapted, class TNull>
	concept adaptable_with = std::movable<TNull>
							&& std::movable<TAdapted>
							&& concepts::weakly_equality_comparable_with<TAdapted, TNull>
							&& requires
							{
								typename adapted_value_t<TAdapted>;
								{ unwrap_adapted(std::declval<TAdapted>()) } -> std::convertible_to<adapted_value_t<TAdapted>>;
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
	/**
	 * \brief Converts the given argument to a \ref sl::nullables::adapter "adapter" object.
	 * \ingroup GROUP_NULLABLES_CUSTOMIZATION_POINTS
	 * \relates sl::nullables::adapter
	 * \details This is a customization point, which users may hook to add convenient conversions from there
	 * types to adapters.
	 */
	inline constexpr detail::to_nullables_adapter_fn to_nullables_adapter{};

	namespace detail
	{
		template <class T>
		concept convertible_to_adapter = requires
		{
			adapter{ nullables::to_nullables_adapter(std::declval<T>()) };
		};
	}

	/**
	 * \brief A adapter class, mimic the behaviour of nullable types.
	 * \tparam TNull The null type.
	 * \tparam TAdapted The adapted type.
	 * \details This class aims to provide a layer of abstraction for types, which logically have a null-state but no dedicated null-object to compare with.
	 * For example an arbitrary iterator can be compared to its end iterator, but that end iterator usually must be retrieved from the container (unless
	 * end iterator is a dedicated sentinel type). adapter aims to fill the gap and therefore accepts a state object and a null object. The adapter itself
	 * can be then compared to its dedicated ``adapter_null`` object and thus satisfying at least the ``input_nullable`` concept. If the state object
	 * is then initializable and constructible via the given null-object, the ``nullable`` concept is also satisfied.
	 */
	template <class TNull, adaptable_with<TNull> TAdapted>
		requires std::same_as<TNull, std::remove_cvref_t<TNull>>
				&& std::same_as<TAdapted, std::remove_cvref_t<TAdapted>>
	class adapter
	{
	public:
		using adapted_type = TAdapted;
		using value_type = adapted_value_t<TAdapted>;
		using null_type = TNull;

		/**
		 * \brief Default destructor.
		 */
		constexpr ~adapter() noexcept = default;

		/**
		 * \brief Default copy constructor.
		 * \param other The other adapter.
		 */
		[[nodiscard]]
		constexpr adapter
		(
			const adapter& other
		) noexcept(std::is_nothrow_copy_constructible_v<TNull>
					&& std::is_nothrow_copy_constructible_v<TAdapted>)
		= default;

		/**
		 * \brief Default copy assignment operator.
		 * \param other The other adapter.
		 */
		constexpr adapter& operator =
		(
			const adapter& other
		) noexcept(std::is_nothrow_copy_assignable_v<TNull>
					&& std::is_nothrow_copy_assignable_v<TAdapted>)
		= default;

		/**
		 * \brief Default move constructor.
		 * \param other The other adapter.
		 */
		[[nodiscard]]
		constexpr adapter
		(
			adapter&& other
		) noexcept(std::is_nothrow_move_constructible_v<TNull>
					&& std::is_nothrow_move_constructible_v<TAdapted>)
		= default;

		/**
		 * \brief Default move assignment operator.
		 * \param other The other adapter.
		 */
		constexpr adapter& operator =
		(
			adapter&& other
		) noexcept(std::is_nothrow_move_assignable_v<TNull>
					&& std::is_nothrow_move_assignable_v<TAdapted>)
		= default;

		/**
		 * \brief Constructs the null object with the given argument and then constructs the adapted object with the null object.
		 * \tparam TNullArg The constructor argument type for the null object.
		 * \param nullArg  The constructor argument for the null object.
		 * \attention The behaviour is undefined if after the construction adapted does not compare equal to the null object.
		 */
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

		/**
		 * \brief Default constructs the null object and then constructs the adapted object with the null object.
		 * \attention The behaviour is undefined if after the construction adapted does not compare equal to the null object.
		 */
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

		/**
		 * \copydoc adapter(in_place_null_t, TNullArg&&)
		 */
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

		/**
		 * \brief Constructs the null object and the adapted with the given arguments.
		 * \tparam TNullArg The constructor argument type for the null object.
		 * \tparam TAdaptedArg The constructor argument type for the adapted object.
		 * \param nullArg  The constructor argument for the null object.
		 * \param adaptedArg  The constructor argument for the adapted object.
		 */
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

		/**
		 * \brief Constructs the adapter with the result of a ``to_nullables_adapter`` invocation.
		 * \tparam TArg The argument type.
		 * \param arg The argument.
		 * \note This function makes use of the customization point ``to_nullables_adapter``, which may be hooked by users.
		 */
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

		/**
		 * \brief Assigns null to the adapted object.
		 * \attention The behaviour is undefined if adapted does not compare equal to the null object after the assignment.
		 * \return Reference to this.
		 */
		constexpr adapter& operator =
		(
			[[maybe_unused]] adapter_null_t
		)
		noexcept(std::is_nothrow_assignable_v<adapted_type&, null_type&>)
			requires std::assignable_from<adapted_type&, null_type&>
		{
			m_Adapted = m_Null;
			assert(*this == adapter_null && "adapted_type must comapare equally to the null object.");

			return *this;
		}

		/**
		 * \brief Assigns the adapted object.
		 * \return Reference to this.
		 */
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

		/**
		 * \brief Retrieves the value from the adapted object.
		 * \note This function makes use of the ``unwrap_adapter`` customization point, which may be hooked by users.
		 * \attention If the adapted object compares equal to the null object, the behaviour is undefined.
		 * \return Returns the result as received.
		 */
		[[nodiscard]]
		constexpr decltype(auto) operator *() const &
		{
			assert(*this != adapter_null && "Dereferenced adapted nullable is equally to its null.");

			return unwrap_adapted(m_Adapted);
		}

		/**
		 * \copydoc operator*
		 */
		[[nodiscard]]
		constexpr decltype(auto) operator *() &
		{
			assert(*this != adapter_null && "Dereferenced adapted nullable is equally to its null.");

			return unwrap_adapted(m_Adapted);
		}

		/**
		 * \copydoc operator*
		 */
		[[nodiscard]]
		constexpr decltype(auto) operator *() &&
		{
			assert(*this != adapter_null && "Dereferenced adapted nullable is equally to its null.");

			return unwrap_adapted(std::move(m_Adapted));
		}

		/**
		 * \brief Comparison operator with its ``adapter_null```object.
		 * \return Returns true if the adapted object compares equal to the null object.
		 */
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

	/**
	 * \brief Deduction guide.
	 * \relates sl::nullables::adapter
	 * \tparam TNull The null type.
	 * \tparam TAdapted The adapted type.
	 */
	template <class TNull, class TAdapted>
	adapter(TNull, TAdapted) -> adapter<TNull, TAdapted>;

	/**
	 * \brief Deduction guide, which makes use of ``to_nullables_adapter`` customization point.
	 * \relates sl::nullables::adapter
	 * \tparam T The argument type.
	 */
	template <class T>
		requires requires { to_nullables_adapter(std::declval<T>()); }
	adapter
	(
		T&& t
	) -> adapter<
		typename std::remove_cvref_t<decltype(to_nullables_adapter(std::declval<T>()))>::null_type,
		typename std::remove_cvref_t<decltype(to_nullables_adapter(std::declval<T>()))>::adapted_type
	>;

	/**
	 * \brief Specialization for raw pointers
	 * \ingroup GROUP_NULLABLES_TRAITS
	 * \relates sl::nullables::adapter
	 * \tparam TNull Null type
	 * \tparam TAdapted Adapted type
	 */
	template <class TNull, class TAdapted>
	struct traits<adapter<TNull, TAdapted>>
	{
		using value_type = adapted_value_t<TAdapted>;
		constexpr static adapter_null_t null{ adapter_null };
	};

	/** @} */
}

#endif
