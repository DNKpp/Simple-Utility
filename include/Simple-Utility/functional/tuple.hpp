//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_FUNCTIONAL_TUPLE_HPP
#define SL_UTILITY_FUNCTIONAL_TUPLE_HPP

#pragma once

#include "Simple-Utility/concepts/utility.hpp"
#include "Simple-Utility/functional/transform.hpp"

#include <utility>
#include <tuple>

namespace sl::functional::tuple
{
	/**
	 * \defgroup GROUP_FUNCTIONAL_TUPLE tuple
	 * \ingroup GROUP_FUNCTIONAL
	 * @{
	 */

	/**
	 * \brief Functional object which retrieves an object of a specific type from a tuple-like argument.
	 * \tparam T The type to be retrieved.
	 */
	template <class T>
	inline constexpr transform_fn get{
		[]<class TTuple>(TTuple&& v) -> decltype(auto)
		{
			using std::get;
			return get<T>(std::forward<TTuple>(v));
		}
	};

	/**
	 * \brief Functional object which retrieves an object at a specific index from a tuple-like argument.
	 * \tparam VIndex The index of type to be retrieved.
	 */
	template <std::size_t VIndex>
	inline constexpr transform_fn get_at{
		[]<class TTuple>(TTuple&& v) -> decltype(auto)
		{
			using std::get;
			return get<VIndex>(std::forward<TTuple>(v));
		}
	};
}

namespace sl::functional::tuple::detail
{
	// workaround
	// as of MSVC version 19.32.31328.0 the following workaround became necessary.
	template <class... TArgs, class TTuple>
	constexpr std::tuple<TArgs...> reduce(TTuple&& tuple)
	{
		return { tuple::get<TArgs>(std::forward<TTuple>(tuple))... };
	}

	// workaround
	// as of MSVC version 19.32.31328.0 the following workaround became necessary.
	template <class... TTuples>
	constexpr auto concat(TTuples&&... tuples)
	{
		return std::tuple_cat(std::forward<TTuples>(tuples)...);
	}
}

namespace sl::functional::tuple
{
	/**
	 * \brief Reduces (or permutes) the components of a tuple and returns a them as new tuple.
	 * \tparam TArgs The component types of the returned tuple.
	 * \remark The source and target tuple must consist of unique types.
	 */
	template <class... TArgs>
		requires concepts::unique_types<TArgs...>
	inline constexpr transform_fn reduce{
		[]<class T>(T&& t) { return detail::reduce<TArgs...>(std::forward<T>(t)); }
	};

	/**
	 * \brief Combines all elements from each given tuple into one tuple.
	 */
	inline constexpr transform_fn concat{
		[]<class... TTuples>(TTuples&&... tuples) { return detail::concat(std::forward<TTuples>(tuples)...); }
	};

	/**
	 * \brief Combines all given lvalue references into one tuple
	 */
	inline constexpr transform_fn tie{
		[](auto&... args) { return std::tie(args...); }
	};

	/**
	 * \brief Utility class to forward-and-apply tuple objects.
	 * \tparam TFunc Type of underlying invocable object.
	 */
	template <class TFunc>
		requires std::same_as<TFunc, std::remove_cvref_t<TFunc>>
	class apply_fn
		: public closure_base_fn<TFunc>,
		public enable_operation<transform_fn,
								operators::pipe,
								operators::bind_front,
								operators::bind_back
		>
	{
		using closure_t = closure_base_fn<TFunc>;
	public:
		using closure_t::closure_t;

		/**
		* \brief Forwards and applies the arguments with the underlying function.
		* \tparam TType Tuple-like type to forward to the underlying function.
		* \param args Parameters to forward to the underlying function.
		* \return Return value of the underlying function, if any.
		*/
		template <class TType>
		constexpr decltype(auto) operator()
		(
			TType&& args
		) const & noexcept(noexcept(std::apply(std::declval<const closure_t&>(), args)))
		{
			return std::apply(static_cast<const closure_t&>(*this), std::forward<TType>(args));
		}

		/*! \copydoc operator()() */
		template <class TType>
		constexpr decltype(auto) operator()
		(
			TType&& args
		) & noexcept(noexcept(std::apply(std::declval<closure_t&>(), args)))
		{
			return std::apply(static_cast<closure_t&>(*this), std::forward<TType>(args));
		}

		/*! \copydoc operator()() */
		template <class TType>
		constexpr decltype(auto) operator()
		(
			TType&& args
		) && noexcept(noexcept(std::apply(std::declval<closure_t&&>(), args)))
		{
			return std::apply(static_cast<closure_t&&>(*this), std::forward<TType>(args));
		}
	};

	/**
	 * \brief Deduction guide.
	 * \tparam TFunc Type of the given functional.
	 */
	template <class TFunc>
	apply_fn(TFunc) -> apply_fn<TFunc>;

	/** @} */
}

#endif
