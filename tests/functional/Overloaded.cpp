//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "../helper.hpp"

#include "Simple-Utility/functional/Overloaded.hpp"

#include <functional>
#include <variant>

using namespace sl::functional;

TEMPLATE_LIST_TEST_CASE(
	"Overloaded bound to arbitrary reference can be invoke.",
	"[functional][Overloaded]",
	all_ref_mods_list
)
{
	const int value = GENERATE(42, 1337);

	Overloaded func{std::identity{}};

	const auto result = std::invoke(TestType::cast(func), value);

	REQUIRE(result == value);
}

TEST_CASE("Overloaded invokes correct overload.", "[functional][Overloaded]")
{
	const auto& value = GENERATE(as<std::variant<int, std::string>>{}, 42, "Hello, World!");

	const auto intOverload = [&]([[maybe_unused]] const int) { return std::holds_alternative<int>(value); };
	const auto stringOverload = [&]([[maybe_unused]] const std::string&) { return std::holds_alternative<std::string>(value); };

	const bool result = std::visit(
		Overloaded{
			intOverload,
			stringOverload
		},
		value
	);

	REQUIRE(result);
}
