//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// ReSharper disable CppClangTidyClangDiagnosticDocumentation
#ifndef SL_TEST_HELPER_HPP
#define SL_TEST_HELPER_HPP

#pragma once

#include <concepts>
#include <type_traits>

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
