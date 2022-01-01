//          Copyright Dominic Koepke 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_CONDITIONAL_ALGORITHMS_HPP
#define SL_UTILITY_CONDITIONAL_ALGORITHMS_HPP

#include <concepts>

#include "Simple-Utility/unique_handle.hpp"
#include "Simple-Utility/concepts/operators.hpp"

namespace sl::nullables
{
	template <class T>
	struct nullable_traits
	{
	};

	template <class T>
	using nullable_value_t = typename nullable_traits<std::decay_t<T>>::value_type;

	template <class T>
	constexpr static auto nullable_null_v{ nullable_traits<std::decay_t<T>>::null };

	template <class T>
	concept nullable = requires
						{
							typename nullable_value_t<T>;
							nullable_null_v<T>;
						}
						// custom std::equality_comparable_with, because its too strict
						&& requires(T t)
						{
							{ t == nullable_null_v<T> } -> std::convertible_to<bool>;
							{ t != nullable_null_v<T> } -> std::convertible_to<bool>;
							{ nullable_null_v<T> == t } -> std::convertible_to<bool>;
							{ nullable_null_v<T> != t } -> std::convertible_to<bool>;
						};

	template <class... TArgs>
	struct nullable_traits<unique_handle<TArgs...>>
	{
		using value_type = typename unique_handle<TArgs...>::value_type;
		constexpr static nullhandle_t null{};
	};

	template <class... TArgs>
	struct nullable_traits<std::optional<TArgs...>>
	{
		using value_type = typename std::optional<TArgs...>::value_type;
		constexpr static auto null{ std::nullopt };
	};

	template <class... TArgs>
	struct nullable_traits<std::unique_ptr<TArgs...>>
	{
		using value_type = typename std::unique_ptr<TArgs...>::element_type;
		constexpr static std::nullptr_t null{ nullptr };
	};

	template <class... TArgs>
	struct nullable_traits<std::shared_ptr<TArgs...>>
	{
		using value_type = typename std::shared_ptr<TArgs...>::element_type;
		constexpr static std::nullptr_t null{ nullptr };
	};

	template <class T>
		requires std::is_pointer_v<T>
	struct nullable_traits<T>
	{
		using value_type = std::remove_pointer_t<T>;
		constexpr static std::nullptr_t null{ nullptr };
	};

	[[nodiscard]]
	constexpr decltype(auto) value_unchecked(concepts::dereferencable auto&& closure) noexcept
	{
		return *closure;
	}

	template <class TClosure, class T>
	[[nodiscard]]
	constexpr auto value_or(TClosure&& closure, T&& alternative)
		requires requires { closure.value_or(std::forward<T>(alternative)); }
	{
		// let existing implementations handle the errors; don't want to over-constraint this
		return closure.value_or(std::forward<T>(alternative));
	}

	template <nullable TNullable, std::convertible_to<nullable_value_t<TNullable>> T>
	[[nodiscard]]
	constexpr auto value_or(TNullable&& closure, T&& alternative)
		requires (!requires { closure.value_or(std::forward<T>(alternative)); })
	{
		if (closure != nullable_null_v<TNullable>)
		{
			return value_unchecked(std::forward<TNullable>(closure));
		}
		return std::forward<T>(alternative);
	}

	template <std::invocable TFunc>
	struct or_else
	{
		template <nullable TNullable>
		friend constexpr std::decay_t<TNullable> operator |(TNullable&& closure, or_else&& orElse)
			requires std::constructible_from<std::decay_t<TNullable>, TNullable>
					&& std::constructible_from<std::decay_t<TNullable>, std::invoke_result_t<TFunc>>
		{
			if (closure != nullable_null_v<TNullable>)
			{
				return std::forward<TNullable>(closure);
			}
			return std::invoke(orElse.func);
		}

		TFunc func;
	};

	template <class TFunc>
	struct and_then
	{
		template <nullable TNullable>
		friend constexpr std::invoke_result_t<TFunc, nullable_value_t<TNullable>> operator |(TNullable&& closure, and_then&& andThen)
			requires nullable<std::invoke_result_t<TFunc, nullable_value_t<TNullable>>>
		{
			if (closure != nullable_null_v<TNullable>)
			{
				return std::invoke(andThen.func, value_unchecked(std::forward<TNullable>(closure)));
			}
			return nullable_null_v<
				std::invoke_result_t<TFunc, nullable_value_t<TNullable>>
			>;
		}

		TFunc func;
	};
}

#endif
