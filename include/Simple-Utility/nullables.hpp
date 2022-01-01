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
	using nullable_value_t = typename nullable_traits<std::remove_cvref_t<T>>::value_type;

	template <class T>
	constexpr static auto nullable_null_v{ nullable_traits<std::remove_cvref_t<T>>::null };

	template <class T>
	concept nullable = requires
						{
							typename nullable_value_t<T>;
							nullable_null_v<T>;
						}
						&& concepts::weakly_equality_comparable_with<T, decltype(nullable_null_v<T>)>;

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

	template <class TNullable, class T>
	struct value_or_func_t
	{
		// purposely empty
	};

	template <nullable TNullable, class T>
		requires requires(TNullable n, T a)
		{
			{ n.value_or(a) } -> std::convertible_to<nullable_value_t<TNullable>>;
		}
	struct value_or_func_t<TNullable, T>
	{
		constexpr nullable_value_t<TNullable> operator()(TNullable&& closure, T&& alternative)
		{
			return std::forward<TNullable>(closure).value_or(std::forward<T>(alternative));
		}
	};

	template <nullable TNullable, concepts::initializes<nullable_value_t<TNullable>> T>
		requires (!requires(TNullable n, T a)
		{
			{ n.value_or(a) } -> std::convertible_to<nullable_value_t<TNullable>>;
		})
	struct value_or_func_t<TNullable, T>
	{
		constexpr nullable_value_t<TNullable> operator()(TNullable&& closure, T&& alternative)
		{
			if (closure != nullable_null_v<TNullable>)
			{
				return value_unchecked(std::forward<TNullable>(closure));
			}
			return std::forward<T>(alternative);
		}
	};

	template <class T>
	class value_or
	{
	public:
		explicit constexpr value_or(T&& alternative) noexcept
			: m_Alternative{ std::forward<T>(alternative) }
		{
		}

		template <nullable TNullable>
			requires std::constructible_from<nullable_value_t<TNullable>, T>
		friend constexpr nullable_value_t<TNullable> operator |(TNullable&& closure, value_or&& valueOr)
		{
			return value_or_func_t<TNullable, T>{}(std::forward<TNullable>(closure), std::forward<T>(valueOr.m_Alternative));
		}

	private:
		T m_Alternative;
	};

	template <class T>
	value_or(T&&) -> value_or<T&&>;

	template <class TNullable, class TFunc>
	struct or_else_func_t
	{
		constexpr or_else_func_t()
		{
			static_assert(false, "None of the specialized versions of or_else_func_t could handle the passed arguments");
		}
	};

	template <nullable TNullable, std::invocable TFunc>
		requires std::constructible_from<std::remove_cvref_t<TNullable>, TNullable>
				&& std::constructible_from<std::remove_cvref_t<TNullable>, std::invoke_result_t<TFunc>>
	struct or_else_func_t<TNullable, TFunc>
	{
		constexpr std::remove_cvref_t<TNullable> operator()(TNullable&& closure, TFunc func)
		{
			if (closure != nullable_null_v<TNullable>)
			{
				return std::forward<TNullable>(closure);
			}
			return std::invoke(func);
		}
	};

	template <std::invocable TFunc>
	class or_else
	{
	public:
		explicit constexpr or_else(TFunc func) noexcept
			: m_Func{ std::ref(func) }
		{
		}

		template <nullable TNullable>
		[[nodiscard]]
		friend constexpr std::remove_cvref_t<TNullable> operator |(TNullable&& closure, or_else&& orElse)
		{
			return or_else_func_t<TNullable, std::reference_wrapper<TFunc>>{}(std::forward<TNullable>(closure), orElse.m_Func);
		}

	private:
		std::reference_wrapper<TFunc> m_Func;
	};

	template <class TFunc>
	class and_then
	{
	public:
		explicit constexpr and_then(TFunc func) noexcept
			: m_Func{ std::ref(func) }
		{
		}

		template <nullable TNullable>
		friend constexpr std::invoke_result_t<TFunc, nullable_value_t<TNullable>> operator |(TNullable&& closure, and_then&& andThen)
			requires nullable<std::invoke_result_t<TFunc, nullable_value_t<TNullable>>>
		{
			if (closure != nullable_null_v<TNullable>)
			{
				return std::invoke(andThen.m_Func, value_unchecked(std::forward<TNullable>(closure)));
			}
			return nullable_null_v<
				std::invoke_result_t<TFunc, nullable_value_t<TNullable>>
			>;
		}

	private:
		std::reference_wrapper<TFunc> m_Func;
	};
}

#endif
