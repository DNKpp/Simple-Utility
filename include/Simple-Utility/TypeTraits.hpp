//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_TYPE_TRAITS_HPP
#define SIMPLE_UTILITY_TYPE_TRAITS_HPP

#include <type_traits>

namespace sl
{
	/**
	 * \defgroup GROUP_TYPE_TRAITS type-traits
	 */

	/**
	 * \defgroup GROUP_TYPE_TRAITS_ADD_TYPE_CONST add_type_const
	 * \ingroup GROUP_TYPE_TRAITS
	 * \brief This trait adds the ``const`` qualification to the actual type, instead of the top-level reference or pointer category.
	 * \details Following some examples comparing the behaviour of ``std::add_const`` with ``sl::add_type_const``.
	 * \see https://en.cppreference.com/w/cpp/types/add_cv
	 * \snippet TypeTraits.cpp add_type_const
	 * \{
	 */

	/**
	 * \brief Primary template, adding const to value types.
	 * \tparam T Qualification to be applied to.
	 */
	template <class T>
	struct add_type_const
	{
		using type = const T;
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::add_type_const "add_type_const" trait.
	 * \tparam T Qualification to be applied to.
	 */
	template <class T>
	using add_type_const_t = typename add_type_const<T>::type;

	/**
	 * \brief Specialization, unwrapping lvalue reference types.
	 * \tparam T Qualification to be applied to.
	 */
	template <class T>
	struct add_type_const<T&>
	{
		using type = add_type_const_t<T>&;
	};

	/**
	 * \brief Specialization, unwrapping rvalue reference types.
	 * \tparam T Qualification to be applied to.
	 */
	template <class T>
	struct add_type_const<T&&>
	{
		using type = add_type_const_t<T>&&;
	};

	/**
	 * \brief Specialization, unwrapping pointer types.
	 * \tparam T Qualification to be applied to.
	 */
	template <class T>
	struct add_type_const<T*>
	{
		using type = add_type_const_t<T>*;
	};

	/**
	 * \brief Specialization, unwrapping volatile pointer types.
	 * \tparam T Qualification to be applied to.
	 */
	template <class T>
	struct add_type_const<T* volatile>
	{
		using type = add_type_const_t<T>* volatile;
	};

	/**
	 * \brief Specialization, unwrapping const pointer types.
	 * \tparam T Qualification to be applied to.
	 */
	template <class T>
	struct add_type_const<T* const>
	{
		using type = add_type_const_t<T>* const;
	};

	/**
	 * \brief Specialization, unwrapping const volatile pointer types.
	 * \tparam T Qualification to be applied to.
	 */
	template <class T>
	struct add_type_const<T* const volatile>
	{
		using type = add_type_const_t<T>* const volatile;
	};

	/**
	 * \}
	 */

	/**
	 * \}
	 */
}

#endif
