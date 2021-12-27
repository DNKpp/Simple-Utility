//          Copyright Dominic Koepke 2019 - 2021.
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

#define TESTABLE_UNARY_CONCEPT(test_concept)										\
template <class T, bool VExpected>													\
struct testable_##test_concept : std::bool_constant<test_concept<T> == VExpected>	\
{																					\
}

#define TESTABLE_UNARY_CONCEPT_R(test_concept)									\
template <class T, bool VExpected, class TReturn = empty_t>						\
struct testable_##test_concept : std::bool_constant<							\
									std::same_as<TReturn, empty_t>				\
									? test_concept<T> == VExpected				\
									: test_concept<T, TReturn> == VExpected		\
								>												\
{																				\
}

#define TESTABLE_BINARY_CONCEPT(test_concept)													\
template <class TLhs, class TRhs, bool VExpected>												\
struct testable_##test_concept : std::bool_constant<test_concept<TLhs, TRhs> == VExpected>		\
{																								\
}

#define TESTABLE_BINARY_CONCEPT_R(test_concept)											\
template <class TLhs, class TRhs, bool VExpected, class TReturn = empty_t>				\
struct testable_##test_concept : std::bool_constant<									\
									std::same_as<TReturn, empty_t>						\
									? test_concept<TLhs, TRhs> == VExpected				\
									: test_concept<TLhs, TRhs, TReturn> == VExpected	\
								>														\
{																						\
}

#endif
