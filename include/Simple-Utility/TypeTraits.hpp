//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_TYPE_TRAITS_HPP
#define SIMPLE_UTILITY_TYPE_TRAITS_HPP

#include <type_traits>

#include "Simple-Utility/concepts/stl_extensions.hpp"

namespace sl
{
	/**
	 * \defgroup GROUP_TYPE_TRAITS type-traits
	 */

	/**
	 * \defgroup GROUP_TYPE_TRAITS_CV_QUALIFIED_TYPE cv_qualified_type
	 * \ingroup GROUP_TYPE_TRAITS
	 * \brief This trait decays every reference or pointer category and yields the (possibly cv-qualified) underlying type.
	 * \{
	 */

	/**
	 * \brief Primary template, taking the ``T`` as-is.
	 * \tparam T Type to be decayed.
	 */
	template <class T>
	struct cv_qualified_type
	{
		using type = T;
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::cv_qualified_type "cv_qualified_type" trait.
	 * \tparam T Qualification to be applied to.
	 */
	template <class T>
	using cv_qualified_type_t = typename cv_qualified_type<T>::type;

	/**
	 * \brief Specialization, unwrapping reference types.
	 * \tparam T Type to be decayed.
	 */
	template <concepts::reference T>
	struct cv_qualified_type<T>
	{
		using type = cv_qualified_type_t<std::remove_reference_t<T>>;
	};

	/**
	 * \brief Specialization, unwrapping pointer types.
	 * \tparam T Type to be decayed.
	 */
	template <concepts::pointer T>
	struct cv_qualified_type<T>
	{
		using type = cv_qualified_type_t<std::remove_pointer_t<T>>;
	};

	/**
	 * \}
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
	 * \brief Primary template, adding ``const`` to value types.
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
	 * \defgroup GROUP_TYPE_TRAITS_REMOVE_TYPE_CONST remove_type_const
	 * \ingroup GROUP_TYPE_TRAITS
	 * \brief This trait removes the ``const`` qualification of the actual type, instead of the top-level reference or pointer category.
	 * \details Following some examples comparing the behaviour of ``std::remove_const`` with ``sl::remove_type_const``.
	 * \see https://en.cppreference.com/w/cpp/types/remove_cv
	 * \snippet TypeTraits.cpp remove_type_const
	 * \{
	 */

	/**
	 * \brief Primary template, removing ``const`` from value types.
	 * \tparam T Qualification be removed from.
	 */
	template <class T>
	struct remove_type_const
	{
		using type = std::remove_const_t<T>;
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::remove_type_const "remove_type_const" trait.
	 * \tparam T Qualification be removed from.
	 */
	template <class T>
	using remove_type_const_t = typename remove_type_const<T>::type;

	/**
	 * \brief Specialization, unwrapping lvalue reference types.
	 * \tparam T Qualification be removed from.
	 */
	template <class T>
	struct remove_type_const<T&>
	{
		using type = remove_type_const_t<T>&;
	};

	/**
	 * \brief Specialization, unwrapping rvalue reference types.
	 * \tparam T Qualification be removed from.
	 */
	template <class T>
	struct remove_type_const<T&&>
	{
		using type = remove_type_const_t<T>&&;
	};

	/**
	 * \brief Specialization, unwrapping pointer types.
	 * \tparam T Qualification to be removed from.
	 */
	template <class T>
	struct remove_type_const<T*>
	{
		using type = remove_type_const_t<T>*;
	};

	/**
	 * \brief Specialization, unwrapping volatile pointer types.
	 * \tparam T Qualification to be removed from.
	 */
	template <class T>
	struct remove_type_const<T* volatile>
	{
		using type = remove_type_const_t<T>* volatile;
	};

	/**
	 * \brief Specialization, unwrapping const pointer types.
	* \tparam T Qualification to be removed from.
	 */
	template <class T>
	struct remove_type_const<T* const>
	{
		using type = remove_type_const_t<T>* const;
	};

	/**
	 * \brief Specialization, unwrapping const volatile pointer types.
	 * \tparam T Qualification to be removed from.
	 */
	template <class T>
	struct remove_type_const<T* const volatile>
	{
		using type = remove_type_const_t<T>* const volatile;
	};

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_TRAITS_ADD_TYPE_VOLATILE add_type_volatile
	 * \ingroup GROUP_TYPE_TRAITS
	 * \brief This trait adds the ``volatile`` qualification to the actual type, instead of the top-level reference or pointer category.
	 * \details Following some examples comparing the behaviour of ``std::add_volatile`` with ``sl::add_type_volatile``.
	 * \see https://en.cppreference.com/w/cpp/types/add_cv
	 * \snippet TypeTraits.cpp add_type_volatile
	 * \{
	 */

	/**
	 * \brief Primary template, adding ``volatile`` to value types.
	 * \tparam T Qualification to be applied to.
	 */
	template <class T>
	struct add_type_volatile
	{
		using type = volatile T;
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::add_type_volatile "add_type_volatile" trait.
	 * \tparam T Qualification to be applied to.
	 */
	template <class T>
	using add_type_volatile_t = typename add_type_volatile<T>::type;

	/**
	 * \brief Specialization, unwrapping lvalue reference types.
	 * \tparam T Qualification to be applied to.
	 */
	template <class T>
	struct add_type_volatile<T&>
	{
		using type = add_type_volatile_t<T>&;
	};

	/**
	 * \brief Specialization, unwrapping rvalue reference types.
	 * \tparam T Qualification to be applied to.
	 */
	template <class T>
	struct add_type_volatile<T&&>
	{
		using type = add_type_volatile_t<T>&&;
	};

	/**
	 * \brief Specialization, unwrapping pointer types.
	 * \tparam T Qualification to be applied to.
	 */
	template <class T>
	struct add_type_volatile<T*>
	{
		using type = add_type_volatile_t<T>*;
	};

	/**
	 * \brief Specialization, unwrapping volatile pointer types.
	 * \tparam T Qualification to be applied to.
	 */
	template <class T>
	struct add_type_volatile<T* volatile>
	{
		using type = add_type_volatile_t<T>* volatile;
	};

	/**
	 * \brief Specialization, unwrapping const pointer types.
	 * \tparam T Qualification to be applied to.
	 */
	template <class T>
	struct add_type_volatile<T* const>
	{
		using type = add_type_volatile_t<T>* const;
	};

	/**
	 * \brief Specialization, unwrapping const volatile pointer types.
	 * \tparam T Qualification to be applied to.
	 */
	template <class T>
	struct add_type_volatile<T* const volatile>
	{
		using type = add_type_volatile_t<T>* const volatile;
	};

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_TRAITS_REMOVE_TYPE_VOLATILE remove_type_volatile
	 * \ingroup GROUP_TYPE_TRAITS
	 * \brief This trait removes the ``volatile`` qualification of the actual type, instead of the top-level reference or pointer category.
	 * \details Following some examples comparing the behaviour of ``std::remove_volatile`` with ``sl::remove_type_volatile``.
	 * \see https://en.cppreference.com/w/cpp/types/remove_cv
	 * \snippet TypeTraits.cpp remove_type_volatile
	 * \{
	 */

	/**
	 * \brief Primary template, removing ``volatile`` from value types.
	 * \tparam T Qualification be removed from.
	 */
	template <class T>
	struct remove_type_volatile
	{
		using type = std::remove_volatile_t<T>;
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::remove_type_volatile "remove_type_volatile" trait.
	 * \tparam T Qualification be removed from.
	 */
	template <class T>
	using remove_type_volatile_t = typename remove_type_volatile<T>::type;

	/**
	 * \brief Specialization, unwrapping lvalue reference types.
	 * \tparam T Qualification be removed from.
	 */
	template <class T>
	struct remove_type_volatile<T&>
	{
		using type = remove_type_volatile_t<T>&;
	};

	/**
	 * \brief Specialization, unwrapping rvalue reference types.
	 * \tparam T Qualification be removed from.
	 */
	template <class T>
	struct remove_type_volatile<T&&>
	{
		using type = remove_type_volatile_t<T>&&;
	};

	/**
	 * \brief Specialization, unwrapping pointer types.
	 * \tparam T Qualification to be removed from.
	 */
	template <class T>
	struct remove_type_volatile<T*>
	{
		using type = remove_type_volatile_t<T>*;
	};

	/**
	 * \brief Specialization, unwrapping volatile pointer types.
	 * \tparam T Qualification to be removed from.
	 */
	template <class T>
	struct remove_type_volatile<T* volatile>
	{
		using type = remove_type_volatile_t<T>* volatile;
	};

	/**
	 * \brief Specialization, unwrapping const pointer types.
	* \tparam T Qualification to be removed from.
	 */
	template <class T>
	struct remove_type_volatile<T* const>
	{
		using type = remove_type_volatile_t<T>* const;
	};

	/**
	 * \brief Specialization, unwrapping const volatile pointer types.
	 * \tparam T Qualification to be removed from.
	 */
	template <class T>
	struct remove_type_volatile<T* const volatile>
	{
		using type = remove_type_volatile_t<T>* const volatile;
	};

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_TRAITS_ADD_TYPE_CV add_type_cv
	 * \ingroup GROUP_TYPE_TRAITS
	 * \brief This trait adds the ``const`` and ``volatile`` qualifications to the actual type, instead of the top-level reference or pointer category.
	 * \details Following some examples comparing the behaviour of ``std::add_cv`` with ``sl::add_type_cv``.
	 * \see https://en.cppreference.com/w/cpp/types/add_cv
	 * \snippet TypeTraits.cpp add_type_cv
	 * \{
	 */

	/**
	 * \brief Primary template, adding ``const`` and ``volatile`` to value types.
	 * \tparam T Qualification to be applied to.
	 */
	template <class T>
	struct add_type_cv
	{
		using type = add_type_const_t<add_type_volatile_t<T>>;
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::add_type_cv "add_type_cv" trait.
	 * \tparam T Qualification to be applied to.
	 */
	template <class T>
	using add_type_cv_t = typename add_type_cv<T>::type;

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_TRAITS_REMOVE_TYPE_CV remove_type_cv
	 * \ingroup GROUP_TYPE_TRAITS
	 * \brief This trait removes the ``const`` and ``volatile`` qualifications from the actual type, instead of the top-level reference or pointer category.
	 * \details Following some examples comparing the behaviour of ``std::remove_cv`` with ``sl::remove_type_cv``.
	 * \see https://en.cppreference.com/w/cpp/types/remove_cv
	 * \snippet TypeTraits.cpp remove_type_cv
	 * \{
	 */

	/**
	 * \brief Primary template, removing ``const`` and ``volatile`` from value types.
	 * \tparam T Qualification to be removed from.
	 */
	template <class T>
	struct remove_type_cv
	{
		using type = remove_type_const_t<remove_type_volatile_t<T>>;
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::remove_type_cv "remove_type_cv" trait.
	  * \tparam T Qualification to be removed from.
	 */
	template <class T>
	using remove_type_cv_t = typename remove_type_cv<T>::type;

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_TRAITS_VALUE_CATEGORY_AS value_category_as
	 * \ingroup GROUP_TYPE_TRAITS
	 * \brief This trait modifies the value category (value, lvalue-reference, rvalue-reference) of the target type appropriately to the source type.
	 * 
	 * \{
	 */

	/**
	* \brief Primary template, removing any reference category.
	* \tparam To Value category to be applied to.
	* \tparam From Value category to be taken from.
	*/
	template <class To, class From>
	struct value_category_as
	{
		using type = std::remove_reference_t<To>;
	};

	/**
	 * \brief Specialization, applying the lvalue reference category.
	 * \tparam To Value category to be applied to.
	 * \tparam From Value category to be taken from.
	 */
	template <class To, class From>
	struct value_category_as<To, From&>
	{
		using type = std::add_lvalue_reference_t<To>;
	};

	/**
	 * \brief Specialization, applying the rvalue reference category.
	 * \tparam To Value category to be applied to.
	 * \tparam From Value category to be taken from.
	 */
	template <class To, class From>
	struct value_category_as<To, From&&>
	{
		using type = std::add_rvalue_reference_t<std::remove_reference_t<To>>;
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::value_category_as "value_category_as" trait.
	 * \tparam To Value category to be applied to.
	 * \tparam From Value category to be taken from.
	 */
	template <class To, class From>
	using value_category_as_t = typename value_category_as<To, From>::type;

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_TRAITS_REMOVE_TYPE_CONSTNESS_AS type_constness_as
	 * \ingroup GROUP_TYPE_TRAITS
	 * \brief This trait either applies the ``const`` qualification on the actual type of ``To``, or removes it.
	 * \{
	 */

	/**
	 * \brief Applies or removes the ``const`` qualification of ``To`` in regards of ``From``.
	 * \tparam To Qualification to be modified on.
	 * \tparam From Qualification to be taken from.
	 */
	template <class To, class From>
	struct type_constness_as
	{
		using type = std::conditional_t<
			std::is_const_v<cv_qualified_type_t<From>>,
			add_type_const_t<To>,
			remove_type_const_t<To>>;
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::type_constness_as "type_constness_as" trait.
	 * \tparam To Qualification to be modified on.
	 * \tparam From Qualification to be taken from.
	 */
	template <class To, class From>
	using type_constness_as_t = typename type_constness_as<To, From>::type;

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_TRAITS_REMOVE_TYPE_VOLATILENESS_AS type_volatileness_as
	 * \ingroup GROUP_TYPE_TRAITS
	 * \brief This trait either applies the ``volatile`` qualification on the actual type of ``To``, or removes it.
	 * \{
	 */

	/**
	 * \brief Applies or removes the ``volatile`` qualification of ``To`` in regards of ``From``.
	 * \tparam To Qualification to be modified on.
	 * \tparam From Qualification to be taken from.
	 */
	template <class To, class From>
	struct type_volatileness_as
	{
		using type = std::conditional_t<
			std::is_volatile_v<cv_qualified_type_t<From>>,
			add_type_volatile_t<To>,
			remove_type_volatile_t<To>>;
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::type_volatileness_as "type_volatileness_as" trait.
	 * \tparam To Qualification to be applied to.
	 * \tparam From Qualification to be taken from.
	 */
	template <class To, class From>
	using type_volatileness_as_t = typename type_volatileness_as<To, From>::type;

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_TRAITS_REMOVE_TYPE_QUALIFICATION_AS type_qualification_as
	 * \ingroup GROUP_TYPE_TRAITS
	 * \brief This trait either applies the ``const`` and/or ``volatile`` qualifications on the actual type of ``To``, or removes it.
	 * \{
	 */

	/**
	 * \brief Applies or removes the ``const`` and/or ``volatile`` qualifications of ``To`` in regards of ``From``.
	 * \tparam To Qualification to be modified on.
	 * \tparam From Qualification to be taken from.
	 */
	template <class To, class From>
	struct type_qualification_as
	{
		using type = type_constness_as_t<
			type_volatileness_as_t<To, From>,
			From>;
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::type_qualification_as "type_qualification_as" trait.
	 * \tparam To Qualification to be applied to.
	 * \tparam From Qualification to be taken from.
	 */
	template <class To, class From>
	using type_qualification_as_t = typename type_qualification_as<To, From>::type;

	/**
	 * \}
	 */

	/**
	 * \defgroup GROUP_TYPE_TRAITS_QUALIFIED_CATEGORY_AS qualified_category_as
	 * \ingroup GROUP_TYPE_TRAITS
	 * \brief This trait either applies any qualification and value-category from the source type on the target type.
	 * 
	 * \{
	 */

	template <class To, class From>
	struct qualified_category_as
	{
		using type = value_category_as_t<
			type_qualification_as_t<To, From>,
			From>;
	};

	/**
	 * \brief Convenience alias, exposing the ``type`` member alias of the \ref sl::qualified_category_as "qualified_category_as" trait.
	 * \tparam To Value category to be applied to.
	 * \tparam From Value category to be taken from.
	 */
	template <class To, class From>
	using qualified_category_as_t = typename qualified_category_as<To, From>::type;

	/**
	 * \}
	 */
}

#endif
