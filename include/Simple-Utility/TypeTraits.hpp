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
	 * \defgroup GROUP_TYPE_TRAITS_ADD_CONST add_type_const
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
	 * \brief Specialization, adding const to lvalue reference types.
	 * \tparam T Qualification to be applied to.
	 */
	template <class T>
	struct add_type_const<T&>
	{
		using type = const std::remove_reference_t<T>&;
	};

	/**
	 * \brief Specialization, adding const to rvalue reference types.
	 * \tparam T Qualification to be applied to.
	 */
	template <class T>
	struct add_type_const<T&&>
	{
		using type = const std::remove_reference_t<T>&&;
	};

	/**
	 * \brief Specialization, adding const pointer types.
	 * \tparam T Qualification to be applied to.
	 */
	template <class T>
	struct add_type_const<T*>
	{
		using type = std::add_pointer_t<typename add_type_const<std::remove_pointer_t<T>>::type>;
	};

	/**
	 * \brief Specialization, adding const pointer types.
	 * \tparam T Qualification to be applied to.
	 */
	template <class T>
	struct add_type_const<T* const>
	{
		using type = const std::add_pointer_t<typename add_type_const<std::remove_pointer_t<T>>::type>;
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::add_const "add_type_const" trait.
	 * \tparam T Qualification to be applied to.
	 */
	template <class T>
	using add_type_const_t = typename add_type_const<T>::type;

	/**
	 * \}
	 */
}

#endif
