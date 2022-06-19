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
	/**
	 * \addtogroup GROUP_FUNCTIONAL
	 *
	 * @{
	 */

	/**
	 * \brief Helper type which enables conjunctive composing of two functional objects via operator &&.
	 * \tparam TDerived The most derived class type.
	 * \tparam TClosureBase The base closure type (with one template argument left to be specified).
	 */
	template <class TDerived, template <class> class TClosureBase>
		requires std::is_class_v<TDerived> && std::same_as<TDerived, std::remove_cvref_t<TDerived>>
	struct conjunction_operator
	{
	private:
		using composer_t = detail::composer<TClosureBase, detail::binary_conjunction_fn>;

	public:
		/**
		 * \brief Composes this and the other functional object as conjunction.
		 * \tparam TOther The type of the other functional object.
		 * \param other The right-hand-side functional object.
		 * \return The conjunctive composition of this and other as new functional object.
		 */
		template <class TOther>
		[[nodiscard]]
		constexpr auto operator &&
		(
			TOther&& other
		) && noexcept(detail::is_nothrow_composable_v<composer_t, TDerived&&, TOther>)
		{
			return composer_t::compose(static_cast<TDerived&&>(*this), std::forward<TOther>(other));
		}

		/**
		 * \copydoc operator&&()
		 */
		template <class TOther>
		[[nodiscard]]
		constexpr auto operator &&
		(
			TOther&& other
		) const & noexcept(detail::is_nothrow_composable_v<composer_t, const TDerived&, TOther>)
		{
			return composer_t::compose(static_cast<const TDerived&>(*this), std::forward<TOther>(other));
		}

		/**
		 * \brief Composes both functional objects as conjunction.
		 * \tparam TLhs The left-hand-side functional type.
		 * \param lhs The left-hand-side functional object.
		 * \param rhs The right-hand-side functional object.
		 * \return The conjunctive composition of both functional objects as new functional object.
		 */
		template <class TLhs>
		[[nodiscard]]
		friend constexpr auto operator &&
		(
			TLhs&& lhs,
			conjunction_operator&& rhs
		)
		noexcept(detail::is_nothrow_composable_v<composer_t, TLhs, TDerived&&>)
			requires (!requires { lhs.operator&&(std::move(rhs)); })

		{
			return composer_t::compose(std::forward<TLhs>(lhs), static_cast<TDerived&&>(rhs));
		}

		/**
		 * \copydoc operator&&(TLhs&&, conjunction_operator&&)
		 */
		template <class TLhs>
		[[nodiscard]]
		friend constexpr auto operator &&
		(
			TLhs&& lhs,
			const conjunction_operator& rhs
		)
		noexcept(detail::is_nothrow_composable_v<composer_t, TLhs, const TDerived&>)
			requires (!requires { lhs.operator&&(rhs); })
		{
			return composer_t::compose(std::forward<TLhs>(lhs), static_cast<const TDerived&>(rhs));
		}
	};

	/**
	 * \brief Helper type which enables disjunctive composing of two functional objects via operator ||.
	 * \tparam TDerived The most derived class type.
	 * \tparam TClosureBase The base closure type (with one template argument left to be specified).
	 */
	template <class TDerived, template <class> class TClosureBase>
		requires std::is_class_v<TDerived> && std::same_as<TDerived, std::remove_cvref_t<TDerived>>
	struct disjunction_operator
	{
	private:
		using composer_t = detail::composer<TClosureBase, detail::binary_disjunction_fn>;

	public:
		/**
		 * \brief Composes this and the other functional object as disjunction.
		 * \tparam TOther The type of the other functional object.
		 * \param other The right-hand-side functional object.
		 * \return The disjunctive composition of this and other as new functional object.
		 */
		template <class TOther>
		[[nodiscard]]
		constexpr auto operator ||
		(
			TOther&& other
		) && noexcept(detail::is_nothrow_composable_v<composer_t, TDerived&&, TOther>)
		{
			return composer_t::compose(static_cast<TDerived&&>(*this), std::forward<TOther>(other));
		}

		/**
		 * \copydoc operator||()
		 */
		template <class TOther>
		[[nodiscard]]
		constexpr auto operator ||
		(
			TOther&& other
		) const & noexcept(detail::is_nothrow_composable_v<composer_t, const TDerived&, TOther>)
		{
			return composer_t::compose(static_cast<const TDerived&>(*this), std::forward<TOther>(other));
		}

		/**
		 * \brief Composes both functional objects as disjunction.
		 * \tparam TLhs The left-hand-side functional type.
		 * \param lhs The left-hand-side functional object.
		 * \param rhs The right-hand-side functional object.
		 * \return The disjunctive composition of both functional objects as new functional object.
		 */
		template <class TLhs>
		[[nodiscard]]
		friend constexpr auto operator ||
		(
			TLhs&& lhs,
			disjunction_operator&& rhs
		)
		noexcept(detail::is_nothrow_composable_v<composer_t, TLhs, TDerived&&>)
			requires (!requires { lhs.operator||(std::move(rhs)); })
		{
			return composer_t::compose(std::forward<TLhs>(lhs), static_cast<TDerived&&>(rhs));
		}

		/**
		 * \copydoc operator||(TLhs&&, disjunction_operator&&)
		 */
		template <class TLhs>
		[[nodiscard]]
		friend constexpr auto operator ||
		(
			TLhs&& lhs,
			const disjunction_operator& rhs
		)
		noexcept(detail::is_nothrow_composable_v<composer_t, TLhs, const TDerived&>)
			requires (!requires { lhs.operator||(rhs); })
		{
			return composer_t::compose(std::forward<TLhs>(lhs), static_cast<const TDerived&>(rhs));
		}
	};

	/**
	 * \brief Helper type which enables negation on functionals via operator !.
	 * \tparam TDerived The most derived class type.
	 * \tparam TClosureBase The base closure type (with one template argument left to be specified).
	 */
	template <class TDerived, template <class> class TClosureBase>
		requires std::is_class_v<TDerived> && std::same_as<TDerived, std::remove_cvref_t<TDerived>>
	struct negation_operator
	{
	private:
		template <class TFunc>
		using negated_fn = std::remove_cvref_t<decltype(std::not_fn(std::declval<TFunc>()))>;

	public:
		/**
		 * \brief Negates this functional object.
		 * \return The negation of this functional, as a new functional object.
		 */
		[[nodiscard]]
		constexpr auto operator !() && noexcept(
			std::is_nothrow_constructible_v<TClosureBase<negated_fn<TDerived&&>>, negated_fn<TDerived&&>>
		)
		{
			return TClosureBase<negated_fn<TDerived&&>>{ std::not_fn(static_cast<TDerived&&>(*this)) };
		}

		/**
		 * \copydoc operator!()
		 */
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
		requires std::same_as<TFunc, std::remove_cvref_t<TFunc>>
	class predicate_fn
		: public closure_base_fn<TFunc>,
		public pipe_operator<predicate_fn<TFunc>, predicate_fn>,
		public conjunction_operator<predicate_fn<TFunc>, predicate_fn>,
		public disjunction_operator<predicate_fn<TFunc>, predicate_fn>,
		public negation_operator<predicate_fn<TFunc>, predicate_fn>
	{
		using closure_t = closure_base_fn<TFunc>;
	public:
		using closure_t::closure_t;
	};

	/**
	 * \brief Deduction guide.
	 * \tparam TFunc Type of the given functional.
	 */
	template <class TFunc>
	predicate_fn(TFunc) -> predicate_fn<std::remove_cvref_t<TFunc>>;

	/** @} */
}

#endif
