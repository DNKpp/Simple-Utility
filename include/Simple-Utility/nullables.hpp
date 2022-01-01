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
						&& concepts::weakly_equality_comparable_with<T, decltype(nullable_null_v<T>)>
						&& concepts::initializes<std::remove_cvref_t<decltype(nullable_null_v<T>)>, std::remove_cvref_t<T>>
						&& std::is_assignable_v<std::remove_cvref_t<T>&, decltype(nullable_null_v<T>)>;

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

	template <nullable TNullable>
		requires concepts::dereferencable<TNullable>
	[[nodiscard]]
	constexpr decltype(auto) value_unchecked(TNullable&& closure) noexcept
	{
		return *std::forward<TNullable>(closure);
	}

	namespace detail
	{
		template <nullable TNullable>
		using dereference_type_t = decltype(value_unchecked(std::declval<TNullable>()));
	}

	template <class TNullable, class T>
	struct value_or_func_t
	{
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
			return value_or_func_t<TNullable, T>{}
			(
				std::forward<TNullable>(closure),
				std::forward<T>(valueOr.m_Alternative)
			);
		}

	private:
		T m_Alternative;
	};

	template <class T>
	value_or(T&&) -> value_or<T&&>;

	template <class TNullable, class TFunc>
	struct or_else_func_t
	{
	};

	template <nullable TNullable, std::invocable TFunc>
		requires std::constructible_from<std::remove_cvref_t<TNullable>, TNullable>
	struct or_else_func_t<TNullable, TFunc>
	{
		constexpr std::remove_cvref_t<TNullable> operator()(TNullable&& closure, TFunc func)
		{
			static_assert
			(
				std::constructible_from<std::remove_cvref_t<TNullable>, std::invoke_result_t<TFunc>>
				|| std::same_as<void, std::invoke_result_t<TFunc>>,
				"Func return type must be either void or initialize the std::remove_cvref_t<TNullable> type."
			);

			if (closure != nullable_null_v<TNullable>)
			{
				return std::forward<TNullable>(closure);
			}

			if constexpr (std::same_as<void, std::invoke_result_t<TFunc>>)
			{
				std::invoke(func);
				return nullable_null_v<TNullable>;
			}
			else
			{
				return std::invoke(func);
			}
		}
	};

	template <std::invocable TFunc>
	class or_else
	{
	public:
		explicit constexpr or_else(TFunc func) noexcept
			: m_Func{ std::move(func) }
		{
		}

		template <nullable TNullable>
		[[nodiscard]]
		friend constexpr std::remove_cvref_t<TNullable> operator |(TNullable&& closure, or_else&& orElse)
		{
			return or_else_func_t<TNullable, std::reference_wrapper<TFunc>>{}
			(
				std::forward<TNullable>(closure),
				std::ref(orElse.m_Func)
			);
		}

	private:
		TFunc m_Func;
	};

	template <class TNullable, class TFunc>
	struct and_then_func_t
	{
	};

	template <nullable TNullable, std::invocable<detail::dereference_type_t<TNullable>> TFunc>
	struct and_then_func_t<TNullable, TFunc>
	{
		using return_t = std::invoke_result_t<TFunc, detail::dereference_type_t<TNullable>>;

		static_assert(nullable<return_t>, "TFunc must return a nullable type.");

		constexpr return_t operator()(TNullable&& closure, TFunc func)
		{
			if (closure != nullable_null_v<TNullable>)
			{
				return std::invoke(func, value_unchecked(std::forward<TNullable>(closure)));
			}
			return nullable_null_v<return_t>;
		}
	};

	template <class TFunc>
	class and_then
	{
	public:
		explicit constexpr and_then(TFunc func) noexcept
			: m_Func{ std::move(func) }
		{
		}

		template <nullable TNullable>
		friend constexpr
		std::invoke_result_t<TFunc, detail::dereference_type_t<TNullable>> operator |
		(
			TNullable&& closure,
			and_then&& andThen
		)
		{
			return and_then_func_t<TNullable, std::reference_wrapper<TFunc>>{}
			(
				std::forward<TNullable>(closure),
				std::ref(andThen.m_Func)
			);
		}

	private:
		TFunc m_Func;
	};
}

#endif
