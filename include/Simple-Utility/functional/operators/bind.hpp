//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_OPERATORS_BIND_HPP
#define SL_UTILITY_FUNCTIONAL_OPERATORS_BIND_HPP

#pragma once

#include "Simple-Utility/functional/base.hpp"

namespace sl::functional::operators::detail
{
	struct bind_front_caller_fn
	{
		static constexpr composition_strategy_t composition_strategy{ composition_strategy_t::prefer_join };

		template <class TFunctionsTuple, class... TCallArgs>
		[[nodiscard]]
		constexpr decltype(auto) operator ()
		(
			TFunctionsTuple&& functionsTuple,
			TCallArgs&&... callArgs
		) const
		{
			static_assert(1 < std::tuple_size_v<std::remove_cvref_t<TFunctionsTuple>>, "Tuple must at least contain two functions.");

			const auto caller = [&]<class TFunction, std::invocable... TBoundArgs>
			(
				TFunction&& func,
				TBoundArgs&&... boundArgs
			) -> decltype(auto)
			{
				static_assert(std::invocable<TFunction, std::invoke_result_t<TBoundArgs>..., TCallArgs...>,
					"Functional is not invokable with bound and call arguments.");

				return std::invoke(
					std::forward<TFunction>(func),
					std::invoke(std::forward<TBoundArgs>(boundArgs))...,
					std::forward<TCallArgs>(callArgs)...
				);
			};

			return std::apply(caller, std::forward<TFunctionsTuple>(functionsTuple));
		}
	};

	struct bind_back_caller_fn
	{
		static constexpr composition_strategy_t composition_strategy{ composition_strategy_t::prefer_join };

		template <class TFunctionsTuple, class... TCallArgs>
		[[nodiscard]]
		constexpr decltype(auto) operator ()
		(
			TFunctionsTuple&& functionsTuple,
			TCallArgs&&... callArgs
		) const
		{
			static_assert(1 < std::tuple_size_v<std::remove_cvref_t<TFunctionsTuple>>, "Tuple must at least contain two functions.");

			const auto caller = [&]<class TFunction, std::invocable... TBoundArgs>
			(
				TFunction&& func,
				TBoundArgs&&... boundArgs
			) -> decltype(auto)
			{
				static_assert(std::invocable<TFunction, TCallArgs..., std::invoke_result_t<TBoundArgs>...>,
					"Functional is not invokable with bound and call arguments.");

				return std::invoke(
					std::forward<TFunction>(func),
					std::forward<TCallArgs>(callArgs)...,
					std::invoke(std::forward<TBoundArgs>(boundArgs))...
				);
			};

			return std::apply(caller, std::forward<TFunctionsTuple>(functionsTuple));
		}
	};
}

namespace sl::functional::operators
{
	/**
	 * \addtogroup GROUP_FUNCTIONAL_OPERATORS operators
	 * \ingroup GROUP_FUNCTIONAL
	 * @{
	 */

	 /**
	 * \brief Helper type which enables unary front currying on functionals via operator <<.
	 * \tparam TDerived The most derived class type.
	 * \tparam TClosureBase The base closure type (with one template argument left to be specified).
	 */
	template <class TDerived, template <class> class TClosureBase>
		requires std::is_class_v<TDerived> && std::same_as<TDerived, std::remove_cvref_t<TDerived>>
	struct bind_front
	{
	private:
		using composer_t = detail::compose_helper_t<TClosureBase, detail::bind_front_caller_fn>;

	public:
		/**
		 * \brief Curries the front parameter of this functional object.
		 * \tparam TValue The type of the curried value.
		 * \param value The value to be curried.
		 * \return The curried functional, as a new functional object.
		 */
		template <class TValue>
		[[nodiscard]]
		constexpr auto operator <<
		(
			TValue&& value
		) const & noexcept(std::is_nothrow_invocable_v<composer_t, const TDerived&, value_fn<std::remove_cvref_t<TValue>>&&>)
		{
			return composer_t{}(static_cast<const TDerived&>(*this), value_fn{ std::forward<TValue>(value) });
		}

		/**
		 * \copydoc operator<<
		 */
		template <class TValue>
		[[nodiscard]]
		constexpr auto operator <<
		(
			TValue&& value
		) && noexcept(std::is_nothrow_invocable_v<composer_t, TDerived&&, value_fn<std::remove_cvref_t<TValue>>&&>)
		{
			return composer_t{}(static_cast<TDerived&&>(*this), value_fn{ std::forward<TValue>(value) });
		}
	};

	/**
	 * \brief Helper type which enables unary back currying on functionals via operator >>.
	 * \tparam TDerived The most derived class type.
	 * \tparam TClosureBase The base closure type (with one template argument left to be specified).
	 */
	template <class TDerived, template <class> class TClosureBase>
		requires std::is_class_v<TDerived> && std::same_as<TDerived, std::remove_cvref_t<TDerived>>
	struct bind_back
	{
	private:
		using composer_t = detail::compose_helper_t<TClosureBase, detail::bind_back_caller_fn>;

	public:
		/**
		 * \brief Curries the back parameter of this functional object.
		 * \tparam TValue The type of the curried value.
		 * \param value The value to be curried.
		 * \return The curried functional, as a new functional object.
		 */
		template <class TValue>
		[[nodiscard]]
		constexpr auto operator >>
		(
			TValue&& value
		) const & noexcept(std::is_nothrow_invocable_v<composer_t, const TDerived&, value_fn<std::remove_cvref_t<TValue>>&&>)
		{
			return composer_t{}(static_cast<const TDerived&>(*this), value_fn{ std::forward<TValue>(value) });
		}

		/**
		 * \copydoc operator>>
		 */
		template <class TValue>
		[[nodiscard]]
		constexpr auto operator >>
		(
			TValue&& value
		) && noexcept(std::is_nothrow_invocable_v<composer_t, TDerived&&, value_fn<std::remove_cvref_t<TValue>>&&>)
		{
			return composer_t{}(static_cast<TDerived&&>(*this), value_fn{ std::forward<TValue>(value) });
		}
	};

	/** @} */
}

#endif
