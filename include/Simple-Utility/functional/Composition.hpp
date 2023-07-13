//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_FUNCTIONAL_COMPOSITION_HPP
#define SIMPLE_UTILITY_FUNCTIONAL_COMPOSITION_HPP

#include <concepts>
#include <functional>
#include <tuple>
#include <type_traits>

#include "Simple-Utility/Config.hpp"
#include "Simple-Utility/Tuple.hpp"
#include "Simple-Utility/TypeList.hpp"
#include "Simple-Utility/concepts/stl_extensions.hpp"
#include "Simple-Utility/functional/BasicClosure.hpp"
#include "Simple-Utility/functional/Overloaded.hpp"

namespace sl::functional
{
	template <class T>
	concept composition_strategy = concepts::unqualified<T>;

	template <composition_strategy CompStrategy, function FirstFn, function SecondFn, function... OtherFns>
	class Composition
	{
	public:
		using CompositionStrategy = CompStrategy;

		template <class FirstCTorArg, class SecondCTorArg, class... OtherCTorArgs>
			requires std::constructible_from<FirstFn, FirstCTorArg>
					&& std::constructible_from<SecondFn, SecondCTorArg>
					&& (... && std::constructible_from<OtherFns, OtherCTorArgs>)
		[[nodiscard]]
		constexpr Composition(
			FirstCTorArg&& firstArg,
			SecondCTorArg&& secondArg,
			OtherCTorArgs&&... otherArgs
		) noexcept(std::is_nothrow_constructible_v<FirstFn, FirstCTorArg>
					&& std::is_nothrow_constructible_v<SecondFn, SecondCTorArg>
					&& (... && std::is_nothrow_constructible_v<OtherFns, OtherCTorArgs>))
			: m_Functions{
				std::forward<FirstCTorArg>(firstArg),
				std::forward<SecondCTorArg>(secondArg),
				std::forward<OtherCTorArgs>(otherArgs)...
			}
		{
		}

		template <class... Args>
			requires std::invocable<CompStrategy, std::tuple<const FirstFn&, const SecondFn&, const OtherFns&...>, Args...>
		constexpr decltype(auto) operator ()(
			Args&&... args
		) const & noexcept(std::is_nothrow_invocable_v<
			CompStrategy,
			std::tuple<const FirstFn&, const SecondFn&, const OtherFns&...>,
			Args...>)
		{
			return std::invoke(CompStrategy{}, fns(), std::forward<Args>(args)...);
		}

		template <class... Args>
			requires std::invocable<CompStrategy, std::tuple<FirstFn&, SecondFn&, OtherFns&...>, Args...>
		constexpr decltype(auto) operator ()(
			Args&&... args
		) & noexcept(std::is_nothrow_invocable_v<
			CompStrategy,
			std::tuple<FirstFn&, SecondFn&, OtherFns&...>,
			Args...>)
		{
			return std::invoke(CompStrategy{}, fns(), std::forward<Args>(args)...);
		}

		template <class... Args>
			requires std::invocable<CompStrategy, std::tuple<const FirstFn&&, const SecondFn&&, const OtherFns&&...>, Args...>
		constexpr decltype(auto) operator ()(
			Args&&... args
		) const && noexcept(std::is_nothrow_invocable_v<
			CompStrategy,
			std::tuple<const FirstFn&&, const SecondFn&&, const OtherFns&&...>,
			Args...>)
		{
			return std::invoke(CompStrategy{}, std::move(*this).fns(), std::forward<Args>(args)...);
		}

		template <class... Args>
			requires std::invocable<CompStrategy, std::tuple<FirstFn&&, SecondFn&&, OtherFns&&...>, Args...>
		constexpr decltype(auto) operator ()(
			Args&&... args
		) && noexcept(std::is_nothrow_invocable_v<
			CompStrategy,
			std::tuple<FirstFn&&, SecondFn&&, OtherFns&&...>,
			Args...>)
		{
			return std::invoke(CompStrategy{}, std::move(*this).fns(), std::forward<Args>(args)...);
		}

		constexpr std::tuple<const FirstFn&, const SecondFn&, const OtherFns&...> fns() const & noexcept
		{
			return tuple::transform_elements(m_Functions, [](auto& t) -> const auto& { return t; });
		}

		constexpr std::tuple<FirstFn&, SecondFn&, OtherFns&...> fns() & noexcept
		{
			return tuple::transform_elements(m_Functions, [](auto& t) -> auto& { return t; });
		}

		constexpr std::tuple<const FirstFn&&, const SecondFn&&, const OtherFns&&...> fns() const && noexcept
		{
			return tuple::transform_elements(m_Functions, [](auto& t) -> const auto&& { return std::move(t); });
		}

		constexpr std::tuple<FirstFn&&, SecondFn&&, OtherFns&&...> fns() && noexcept
		{
			return tuple::transform_elements(m_Functions, [](auto& t) -> auto&& { return std::move(t); });
		}

	private:
		SL_UTILITY_NO_UNIQUE_ADDRESS
		std::tuple<FirstFn, SecondFn, OtherFns...> m_Functions;
	};

	template <class T>
	struct is_composition
		: public std::false_type
	{
	};

	template <composition_strategy CompositionStrategy, function... Fns>
	struct is_composition<Composition<CompositionStrategy, Fns...>>
		: public std::true_type
	{
	};

	template <class T>
	inline constexpr bool is_composition_v = is_composition<T>::value;
}

namespace sl::functional
{
	template <composition_strategy CompositionStrategy, class FirstFn, class SecondFn, class... OtherFns>
		requires (std::same_as<unwrap_functional_t<FirstFn>, FirstFn>
				&& std::same_as<unwrap_functional_t<SecondFn>, SecondFn>
				&& (... && std::same_as<unwrap_functional_t<OtherFns>, OtherFns>))
	[[nodiscard]]
	constexpr auto make_composition(
		FirstFn&& firstFn,
		SecondFn&& secondFn,
		OtherFns&&... otherFns
	) noexcept(std::is_nothrow_constructible_v<std::remove_cvref_t<FirstFn>, FirstFn>
				&& std::is_nothrow_constructible_v<std::remove_cvref_t<SecondFn>, SecondFn>
				&& (... && std::is_nothrow_constructible_v<std::remove_cvref_t<OtherFns>, OtherFns>))
	{
		constexpr auto as_tuple = Overloaded{
			[]<class T>(T&& fn)
				requires is_composition_v<std::remove_cvref_t<T>>
						&& std::same_as<CompositionStrategy, typename std::remove_cvref_t<T>::CompositionStrategy>
			{
				return std::forward<T>(fn).fns();
			},
			[]<class T>(T&& fn) { return std::forward_as_tuple(std::forward<T>(fn)); }
		};

		return std::apply(
			[&]<class... Fns>(Fns&&... fns)
			{
				return Composition<CompositionStrategy, std::remove_cvref_t<Fns>...>{std::forward<Fns>(fns)...};
			},
			std::tuple_cat(
				as_tuple(std::forward<FirstFn>(firstFn)),
				as_tuple(std::forward<SecondFn>(secondFn)),
				as_tuple(std::forward<OtherFns>(otherFns))...));
	}

	template <composition_strategy CompositionStrategy, class FirstFn, class SecondFn, class... OtherFns>
	[[nodiscard]]
	constexpr auto make_composition(
		FirstFn&& firstFn,
		SecondFn&& secondFn,
		OtherFns&&... otherFns
	) noexcept(noexcept(make_composition<CompositionStrategy>(
		forward_unwrapped<FirstFn>(firstFn),
		forward_unwrapped<SecondFn>(secondFn),
		forward_unwrapped<OtherFns>(otherFns)...)))
	{
		return make_composition<CompositionStrategy>(
			forward_unwrapped<FirstFn>(firstFn),
			forward_unwrapped<SecondFn>(secondFn),
			forward_unwrapped<OtherFns>(otherFns)...);
	}
}

#endif
