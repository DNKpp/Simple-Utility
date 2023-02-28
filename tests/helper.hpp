//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// ReSharper disable CppClangTidyClangDiagnosticDocumentation
#ifndef SL_TEST_HELPER_HPP
#define SL_TEST_HELPER_HPP

#pragma once

#include <array>
#include <concepts>
#include <type_traits>

template <class T>
using as_lvalue_ref_t = std::remove_cvref_t<T>&;

template <class T>
using as_const_lvalue_ref_t = const std::remove_cvref_t<T>&;

template <class T>
using as_rvalue_ref_t = std::remove_cvref_t<T>&&;

template <class T>
using as_const_rvalue_ref_t = const std::remove_cvref_t<T>&&;

template <template <class> class TMod>
struct type_mod
{
	template <class T>
	using type = TMod<T>;

	template <class T>
	static constexpr type<T> cast(T& arg) noexcept
	{
		return static_cast<type<T>>(arg);
	}
};

using all_ref_mods_list = std::tuple<
	type_mod<as_const_lvalue_ref_t>,
	type_mod<as_lvalue_ref_t>,
	type_mod<as_const_rvalue_ref_t>,
	type_mod<as_rvalue_ref_t>
>;

template <template <class> class TLhsMod, template <class> class TRhsMod>
struct binary_type_mod
{
	template <class T>
	using lhs_type = TLhsMod<T>;

	template <class T>
	using rhs_type = TRhsMod<T>;

	template <class T>
	static constexpr lhs_type<T> cast_lhs(T& arg) noexcept
	{
		return static_cast<lhs_type<T>>(arg);
	}

	template <class T>
	static constexpr rhs_type<T> cast_rhs(T& arg) noexcept
	{
		return static_cast<rhs_type<T>>(arg);
	}
};

using all_ref_mods_cart2_list = std::tuple<
	binary_type_mod<as_const_lvalue_ref_t, as_const_lvalue_ref_t>,
	binary_type_mod<as_const_lvalue_ref_t, as_lvalue_ref_t>,
	binary_type_mod<as_const_lvalue_ref_t, as_const_rvalue_ref_t>,
	binary_type_mod<as_const_lvalue_ref_t, as_rvalue_ref_t>,

	binary_type_mod<as_lvalue_ref_t, as_const_lvalue_ref_t>,
	binary_type_mod<as_lvalue_ref_t, as_lvalue_ref_t>,
	binary_type_mod<as_lvalue_ref_t, as_const_rvalue_ref_t>,
	binary_type_mod<as_lvalue_ref_t, as_rvalue_ref_t>,

	binary_type_mod<as_const_rvalue_ref_t, as_const_lvalue_ref_t>,
	binary_type_mod<as_const_rvalue_ref_t, as_lvalue_ref_t>,
	binary_type_mod<as_const_rvalue_ref_t, as_const_rvalue_ref_t>,
	binary_type_mod<as_const_rvalue_ref_t, as_rvalue_ref_t>,

	binary_type_mod<as_rvalue_ref_t, as_const_lvalue_ref_t>,
	binary_type_mod<as_rvalue_ref_t, as_lvalue_ref_t>,
	binary_type_mod<as_rvalue_ref_t, as_const_rvalue_ref_t>,
	binary_type_mod<as_rvalue_ref_t, as_rvalue_ref_t>
>;

struct empty_t
{
};

struct fail_t
{
};

template <bool VConceptResult>
struct trait_from_concept
	: std::bool_constant<VConceptResult>
{
};

#define TESTABLE_UNARY_CONCEPT(test_concept)								\
template <class T, bool VExpected>											\
using is_##test_concept = trait_from_concept<test_concept<T> == VExpected>

#define TESTABLE_UNARY_CONCEPT_R(test_concept)											\
TESTABLE_UNARY_CONCEPT(test_concept);													\
template <class T, class TReturn, bool VExpected>										\
using is_r_##test_concept = trait_from_concept<test_concept<T, TReturn> == VExpected>

#define TESTABLE_UNARY_CONCEPT_R_R(test_concept)													\
TESTABLE_UNARY_CONCEPT(test_concept);																\
template <class T, class TReturn1, class TReturn2, bool VExpected>									\
using is_rr_##test_concept = trait_from_concept<test_concept<T, TReturn1, TReturn2> == VExpected>


#define TESTABLE_BINARY_CONCEPT(test_concept)										\
template <class TLhs, class TRhs, bool VExpected>									\
using is_##test_concept = trait_from_concept<test_concept<TLhs, TRhs> == VExpected>

#define TESTABLE_BINARY_CONCEPT_R(test_concept)													\
TESTABLE_BINARY_CONCEPT(test_concept);															\
template <class TLhs, class TRhs, class TReturn, bool VExpected>								\
using is_r_##test_concept = trait_from_concept<test_concept<TLhs, TRhs, TReturn> == VExpected>

#define TESTABLE_BINARY_CONCEPT_R_R(test_concept)															\
TESTABLE_BINARY_CONCEPT(test_concept);																		\
template <class TLhs, class TRhs, class TReturn1, class TReturn2, bool VExpected>							\
using is_rr_##test_concept = trait_from_concept<test_concept<TLhs, TRhs, TReturn1, TReturn2> == VExpected>

#endif
