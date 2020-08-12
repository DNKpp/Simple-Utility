//          Copyright Dominic Koepke 2019 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_OVERLOAD_HPP
#define SL_OVERLOAD_HPP

#pragma once

#include <type_traits>

namespace sl
{
	template <class... TArgs>
	struct Overload :
		TArgs...
	{
		using TArgs::operator()...;
	};

	template <class ...TArgs>
	Overload(TArgs&&...) -> Overload<std::remove_cvref_t<TArgs>...>;
}

#endif
