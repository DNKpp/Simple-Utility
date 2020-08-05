
//          Copyright Dominic Koepke 2019 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_OVERLOAD_HPP
#define SL_OVERLOAD_HPP

#pragma once

#include <type_traits>
#include <utility>

namespace sl
{
	template <class ...Fs>
	struct overload :
		Fs...
	{
		template <class ...Ts>
		overload(Ts&& ...ts) :
			Fs{ std::forward<Ts>(ts) }...
		{}

		using Fs::operator()...;
	};

	template <class ...Ts>
	overload(Ts&&...)->overload<std::remove_cvref_t<Ts>...>;
}

#endif
