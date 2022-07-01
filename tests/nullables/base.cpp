//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>

#include "Simple-Utility/nullables/base.hpp"
#include "Simple-Utility/nullables/std_optional.hpp"
#include "Simple-Utility/unique_handle.hpp"

using namespace sl::nullables;

namespace
{
	struct test_nullable
	{ };

	struct test_nullable_null_t
	{ };
}

template <>
struct traits<test_nullable>
{
	using value_type = int;
	inline static constexpr test_nullable_null_t null{};
};

TEMPLATE_TEST_CASE_SIG(
	"input_nullable determines whether the requirements are satisfied.",
	"[nullables][concept]",
	((bool VExpected, class TTestType), VExpected, TTestType),
	(false, int),
	(true, int*),
	(true, const int* const&),
	(true, sl::unique_handle<int>)
)
{
	STATIC_REQUIRE(input_nullable<TTestType> == VExpected);
}

TEMPLATE_TEST_CASE_SIG(
	"nullable determines whether the requirements are satisfied.",
	"[nullables][concept]",
	((bool VExpected, class TTestType), VExpected, TTestType),
	(false, int),
	(true, int*),
	(true, const int* const&),
	(true, sl::unique_handle<int>)
)
{
	STATIC_REQUIRE(nullable<TTestType> == VExpected);
}

TEMPLATE_TEST_CASE_SIG(
	"value_t should expose expected value type.",
	"[nullables][trait]",
	((bool VDummy, class TNullable, class TExpected), VDummy, TNullable, TExpected),
	(true, int*, int),
	(true, const int* const&, const int),
	(true, test_nullable&, int),
	(true, const test_nullable, int)
)
{
	STATIC_REQUIRE(std::same_as<value_t<TNullable>, TExpected>);
}

TEMPLATE_TEST_CASE_SIG(
	"null_v should expose expected null object.",
	"[nullables][trait]",
	((bool VDummy, class TNullable, class TExpected), VDummy, TNullable, TExpected),
	(true, int*, std::nullptr_t),
	(true, const int*&, std::nullptr_t),
	(true, test_nullable&, test_nullable_null_t),
	(true, const test_nullable, test_nullable_null_t)
)
{
	STATIC_REQUIRE(std::same_as<std::remove_cvref_t<decltype(null_v<TNullable>)>, TExpected>);
}

TEST_CASE("algorithm_fn can wrap user defined functions.", "[nullables][algorithm][example]")
{
	//! [algorithm custom]
	/* That's the actual algorithm implementation. It simply checks whether the given nullable is invalid (equal to its null object)
	 * and invokes the given function. Otherwise the nullable is simply forwarded as the result of the function.
	 * In fact this is an alternative implementation of the ``value_or`` algorithm, where the value is retrieved lazily when needed.
	*/
	const auto my_lazy_value_or = []<nullable TNullable, std::invocable TFunc>
	(
		TNullable&& in,
		TFunc&& func
	) -> std::remove_cvref_t<TNullable>
	{
		if (in == null_v<TNullable>)
		{
			return std::invoke(std::forward<TFunc>(func));
		}
		return std::forward<TNullable>(in);
	};

	// This factory function isn't requires, but reduces the noise from the calling code.
	const auto lazy_value_or = [&]<std::invocable TFunc>(TFunc&& func)
	{
		// operator >> binds the given parameter to the back (like std::bind_back).
		return algorithm_fn{ my_lazy_value_or } >> std::forward<TFunc>(func);
	};

	const std::optional resultOfValid = std::optional{ 42 }
										| lazy_value_or([] { return 1337; });

	REQUIRE(resultOfValid == 42);

	const std::optional resultOfInvalid = std::optional<int>{}
										| lazy_value_or([] { return 1337; });

	REQUIRE(resultOfInvalid == 1337);
	//! [algorithm custom]
}
