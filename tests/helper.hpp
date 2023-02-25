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
#include <variant>

#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>

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

template <template <class> class... TMods>
class RefModGenerator
	: public Catch::Generators::IGenerator<std::variant<type_mod<TMods>...>>
{
public:
	using Value = std::variant<type_mod<TMods>...>;
	static constexpr std::size_t values{ sizeof...(TMods) };

private:
	std::size_t m_Index{};
	static constexpr std::array<Value, values> m_Values{ (type_mod<TMods>{}, ...) };

	bool next() override
	{
		return ++m_Index < values;
	}

	const Value& get() const override
	{
		return m_Values[m_Index];
	}
};

template <template <class> class... TMods>
inline auto make_ref_mods_generator()
{
	using Generator = RefModGenerator<TMods...>;
	return Catch::Generators::GeneratorWrapper<typename Generator::Value>{
		Catch::Detail::make_unique<Generator>()
	};
}

inline auto make_lvalue_ref_mods_generator()
{
	return make_ref_mods_generator<as_lvalue_ref_t, as_const_lvalue_ref_t>();
}

inline auto make_rvalue_ref_mods_generator()
{
	return make_ref_mods_generator<as_rvalue_ref_t, as_const_rvalue_ref_t>();
}

inline auto make_all_ref_mods_generator()
{
	return make_ref_mods_generator<as_lvalue_ref_t, as_const_lvalue_ref_t, as_rvalue_ref_t, as_const_rvalue_ref_t>();
}


template <class T, class... TArgs>
constexpr decltype(auto) cast(T&& arg, const std::variant<TArgs...>& mod)
{
	return std::visit([&](const auto& m) { return m.cast(std::forward<T>(arg)); }, mod);
}

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
