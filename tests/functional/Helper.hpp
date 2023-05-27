//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UTILITY_TEST_FUNCTIONAL_HELPER_HPP
#define SL_UTILITY_TEST_FUNCTIONAL_HELPER_HPP

#pragma once

#include <catch2/trompeloeil.hpp>

template <class Ret, class Param>
class GenericOperation1Mock
{
public:
	static constexpr bool trompeloeil_movable_mock = true;

	Ret operator ()(Param param) & { return call_lvalue_ref(std::forward<Param>(param)); }

	Ret operator ()(Param param) const & { return call_const_lvalue_ref(std::forward<Param>(param)); }

	Ret operator ()(Param param) && { return call_rvalue_ref(std::forward<Param>(param)); }

	Ret operator ()(Param param) const && { return call_const_rvalue_ref(std::forward<Param>(param)); }

	MAKE_MOCK1(call_lvalue_ref, Ret(Param));
	MAKE_CONST_MOCK1(call_const_lvalue_ref, Ret(Param));
	MAKE_MOCK1(call_rvalue_ref, Ret(Param));
	MAKE_CONST_MOCK1(call_const_rvalue_ref, Ret(Param));
};

template <class Ret, class Param1, class Param2>
class GenericOperation2Mock
{
public:
	static constexpr bool trompeloeil_movable_mock = true;

	Ret operator ()(Param1 p1, Param2 p2) &
	{
		return call_lvalue_ref(std::forward<Param1>(p1), std::forward<Param2>(p2));
	}

	Ret operator ()(Param1 p1, Param2 p2) const &
	{
		return call_const_lvalue_ref(std::forward<Param1>(p1), std::forward<Param2>(p2));
	}

	Ret operator ()(Param1 p1, Param2 p2) &&
	{
		return call_rvalue_ref(std::forward<Param1>(p1), std::forward<Param2>(p2));
	}

	Ret operator ()(Param1 p1, Param2 p2) const &&
	{
		return call_const_rvalue_ref(std::forward<Param1>(p1), std::forward<Param2>(p2));
	}

	MAKE_MOCK2(call_lvalue_ref, Ret(Param1, Param2));
	MAKE_CONST_MOCK2(call_const_lvalue_ref, Ret(Param1, Param2));
	MAKE_MOCK2(call_rvalue_ref, Ret(Param1, Param2));
	MAKE_CONST_MOCK2(call_const_rvalue_ref, Ret(Param1, Param2));
};

template <bool isNoexcept>
struct NoThrowInvokable
{
	template <class First>
	constexpr First operator ()(First&& first, [[maybe_unused]] auto&&...) const noexcept(isNoexcept)
	{
		return std::forward<First>(first);
	}
};

template <bool isNoexceptCopyable, bool isNoexceptMovable>
struct NoThrowConstructible
{
	constexpr NoThrowConstructible() = default;

	constexpr NoThrowConstructible(const NoThrowConstructible&) noexcept(isNoexceptCopyable)
	{
	}

	constexpr NoThrowConstructible& operator =(const NoThrowConstructible&) noexcept(isNoexceptCopyable)
	{
		return *this;
	}

	constexpr NoThrowConstructible(NoThrowConstructible&&) noexcept(isNoexceptMovable)
	{
	}

	constexpr NoThrowConstructible& operator =(NoThrowConstructible&&) noexcept(isNoexceptMovable)
	{
		return *this;
	}
};


#endif
