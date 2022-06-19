//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_PREDICATE_HPP
#define SL_UTILITY_FUNCTIONAL_PREDICATE_HPP

#pragma once

#include "Simple-Utility/functional/transform.hpp"

namespace sl::functional::detail
{
	struct binary_conjunction_fn
	{
		template <class TFunc1, class TFunc2, class... TArgs>
		[[nodiscard]]
		constexpr decltype(auto) operator ()
		(
			TFunc1&& func1,
			TFunc2&& func2,
			const TArgs&... v
		) const
		noexcept(std::is_nothrow_invocable_v<TFunc1, std::add_lvalue_reference_t<std::add_const_t<TArgs>>...>
				&& std::is_nothrow_invocable_v<TFunc2, std::add_lvalue_reference_t<std::add_const_t<TArgs>>...>
		)
			requires std::predicate<TFunc1, TArgs...>
					&& std::predicate<TFunc2, TArgs...>
		{
			return std::invoke(std::forward<TFunc1>(func1), v...)
					&& std::invoke(std::forward<TFunc2>(func2), v...);
		}
	};

	struct binary_disjunction_fn
	{
		template <class TFunc1, class TFunc2, class... TArgs>
		[[nodiscard]]
		constexpr decltype(auto) operator ()
		(
			TFunc1&& func1,
			TFunc2&& func2,
			const TArgs&... v
		) const
		noexcept(std::is_nothrow_invocable_v<TFunc1, std::add_lvalue_reference_t<std::add_const_t<TArgs>>...>
				&& std::is_nothrow_invocable_v<TFunc2, std::add_lvalue_reference_t<std::add_const_t<TArgs>>...>
		)
			requires std::predicate<TFunc1, TArgs...>
					&& std::predicate<TFunc2, TArgs...>
		{
			return std::invoke(std::forward<TFunc1>(func1), v...)
					|| std::invoke(std::forward<TFunc2>(func2), v...);
		}
	};
}

namespace sl::functional
{
	template <class TDerived, template <class> class TClosureBase>
		requires std::is_class_v<TDerived> && std::same_as<TDerived, std::remove_cvref_t<TDerived>>
	struct conjunction_operator
	{
	private:
		using composer_t = detail::composer<TClosureBase, detail::binary_conjunction_fn>;

	public:
		template <class TOther>
		[[nodiscard]]
		constexpr auto operator &&
		(
			TOther&& other
		) && noexcept(detail::is_nothrow_composable_v<composer_t, TDerived&&, TOther>)
		{
			return composer_t::compose(static_cast<TDerived&&>(*this), std::forward<TOther>(other));
		}

		template <class TOther>
		[[nodiscard]]
		constexpr auto operator &&
		(
			TOther&& other
		) const & noexcept(detail::is_nothrow_composable_v<composer_t, const TDerived&, TOther>)
		{
			return composer_t::compose(static_cast<const TDerived&>(*this), std::forward<TOther>(other));
		}

		template <class TLhs>
		[[nodiscard]]
		friend constexpr auto operator &&
		(
			TLhs&& lhs,
			conjunction_operator&& rhs
		)
		noexcept(detail::is_nothrow_composable_v<composer_t, TLhs, TDerived&&>)
			requires (!requires { lhs.operator|(std::move(rhs)); })

		{
			return composer_t::compose(std::forward<TLhs>(lhs), static_cast<TDerived&&>(rhs));
		}

		template <class TLhs>
		[[nodiscard]]
		friend constexpr auto operator &&
		(
			TLhs&& lhs,
			const conjunction_operator& rhs
		)
		noexcept(detail::is_nothrow_composable_v<composer_t, TLhs, const TDerived&>)
			requires (!requires { lhs.operator|(rhs); })
		{
			return composer_t::compose(std::forward<TLhs>(lhs), static_cast<const TDerived&>(rhs));
		}
	};

	template <class TDerived, template <class> class TClosureBase>
		requires std::is_class_v<TDerived> && std::same_as<TDerived, std::remove_cvref_t<TDerived>>
	struct disjunction_operator
	{
	private:
		using composer_t = detail::composer<TClosureBase, detail::binary_disjunction_fn>;

	public:
		template <class TOther>
		[[nodiscard]]
		constexpr auto operator ||
		(
			TOther&& other
		) && noexcept(detail::is_nothrow_composable_v<composer_t, TDerived&&, TOther>)
		{
			return composer_t::compose(static_cast<TDerived&&>(*this), std::forward<TOther>(other));
		}

		template <class TOther>
		[[nodiscard]]
		constexpr auto operator ||
		(
			TOther&& other
		) const & noexcept(detail::is_nothrow_composable_v<composer_t, const TDerived&, TOther>)
		{
			return composer_t::compose(static_cast<const TDerived&>(*this), std::forward<TOther>(other));
		}

		template <class TLhs>
		[[nodiscard]]
		friend constexpr auto operator ||
		(
			TLhs&& lhs,
			disjunction_operator&& rhs
		)
		noexcept(detail::is_nothrow_composable_v<composer_t, TLhs, TDerived&&>)
			requires (!requires { lhs.operator|(std::move(rhs)); })

		{
			return composer_t::compose(std::forward<TLhs>(lhs), static_cast<TDerived&&>(rhs));
		}

		template <class TLhs>
		[[nodiscard]]
		friend constexpr auto operator ||
		(
			TLhs&& lhs,
			const disjunction_operator& rhs
		)
		noexcept(detail::is_nothrow_composable_v<composer_t, TLhs, const TDerived&>)
			requires (!requires { lhs.operator|(rhs); })
		{
			return composer_t::compose(std::forward<TLhs>(lhs), static_cast<const TDerived&>(rhs));
		}
	};

	template <class TDerived, template <class> class TClosureBase>
		requires std::is_class_v<TDerived> && std::same_as<TDerived, std::remove_cvref_t<TDerived>>
	struct negation_operator
	{
	private:
		template <class TFunc>
		using negated_fn = std::remove_cvref_t<decltype(std::not_fn(std::declval<TFunc>()))>;

	public:
		[[nodiscard]]
		constexpr auto operator !() && noexcept(
			std::is_nothrow_constructible_v<TClosureBase<negated_fn<TDerived&&>>, negated_fn<TDerived&&>>
		)
		{
			return TClosureBase<negated_fn<TDerived&&>>{ std::not_fn(static_cast<TDerived&&>(*this)) };
		}

		[[nodiscard]]
		constexpr auto operator !() const & noexcept(
			std::is_nothrow_constructible_v<TClosureBase<negated_fn<const TDerived&>>, negated_fn<const TDerived&>>
		)
		{
			return TClosureBase<negated_fn<const TDerived&>>{ std::not_fn(static_cast<const TDerived&>(*this)) };
		}
	};

	/**
	 * \brief Helper type which accepts a functional type and enables pipe, conjunctive and disjunctive chaining.
	 * \tparam TFunc The functional type.
	 */
	template <class TFunc>
	class predicate_fn
		: public closure_base_fn<std::remove_cvref_t<TFunc>>,
		public pipe_operator<predicate_fn<TFunc>, predicate_fn>,
		public conjunction_operator<predicate_fn<TFunc>, predicate_fn>,
		public disjunction_operator<predicate_fn<TFunc>, predicate_fn>,
		public negation_operator<predicate_fn<TFunc>, predicate_fn>
	{
		using closure_t = closure_base_fn<std::remove_cvref_t<TFunc>>;
	public:
		using closure_t::closure_t;
	};

	/**
	 * \brief Deduction guide.
	 * \tparam TFunc Type of the given functional.
	 */
	template <class TFunc>
	predicate_fn(TFunc) -> predicate_fn<std::remove_cvref_t<TFunc>>;
}

#endif
