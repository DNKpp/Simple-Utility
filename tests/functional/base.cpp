//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>

#include "Simple-Utility/functional/base.hpp"

using namespace sl::functional;

namespace
{
	struct operation_mock_fn
	{
		template <class TFunctionsTuple, class... TCallArgs>
			requires (1 < std::tuple_size_v<std::remove_cvref_t<TFunctionsTuple>>)
		constexpr bool operator ()(TFunctionsTuple&& functionsTuple, TCallArgs&&... callArgs) const
		{
			return std::apply(
				[&]<class... TArgs>(TArgs&&... functions) -> decltype(auto)
				{
					return (std::invoke(std::forward<TArgs>(functions), std::forward<TCallArgs>(callArgs)...) == ...);
				},
				std::forward<TFunctionsTuple>(functionsTuple)
			);
		}
	};

	inline constexpr auto trueFunc = [](auto&&...) { return true; };
	inline constexpr auto falseFunc = [](auto&&...) { return false; };
}

TEST_CASE("composition_fn is constructible with functionals", "[functional][base]")
{
	composition_fn composition{ operation_mock_fn{}, trueFunc, trueFunc, trueFunc };
	composition_fn falseComposition{ operation_mock_fn{}, falseFunc, trueFunc, trueFunc };

	REQUIRE(composition(42));
	REQUIRE(!falseComposition(42, "test"));
}

TEMPLATE_TEST_CASE_SIG(
	"unwrap_functional_result determines the first nested non-closure type.",
	"[functional][base][trait]",
	((bool VDummy, class T, class TExpected), VDummy, T, TExpected),
	(true, bool(*)(int&), bool(*)(int&)),
	(true, bool(* const&)(int&), bool(* const&)(int&)),
	(true, bool(* &&)(int&), bool(* &&)(int&)),
	(true, decltype(trueFunc), decltype(trueFunc)),
	(true, std::remove_cvref_t<decltype(trueFunc)>&&, std::remove_cvref_t<decltype(trueFunc)>&&),
	(true, closure_base_fn<bool(*)(int&)>, bool(*)(int&)),
	(true, closure_base_fn<std::remove_cvref_t<decltype(trueFunc)>>, std::remove_cvref_t<decltype(trueFunc)>),
	(true, closure_base_fn<closure_base_fn<bool(*)(int&)>>, bool(*)(int&)),
	(true, closure_base_fn<closure_base_fn<std::remove_cvref_t<decltype(trueFunc)>>>, std::remove_cvref_t<decltype(trueFunc)>)
)
{
	STATIC_REQUIRE(std::same_as<detail::unwrap_functional_result_t<T>, TExpected>);
}
