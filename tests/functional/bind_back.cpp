//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "../helper.hpp"

#include "Simple-Utility/functional/bind_back.hpp"

namespace sf = sl::functional;

TEMPLATE_LIST_TEST_CASE(
	"functional::bind_back binds value as tail to given functional.",
	"[functional][functional::bind_back]",
	all_ref_mods_list
)
{
	const auto binaryFunc = [](const int x, const std::string& str) { return x + std::stoi(str); };
	auto curriedFunc = sf::bind_back(binaryFunc, "1337");

	REQUIRE(TestType::cast(curriedFunc)(42) == 1379);
}

TEMPLATE_LIST_TEST_CASE(
	"functional::bind_back binds multiple values as tail to given functional.",
	"[functional][functional::bind_back]",
	all_ref_mods_list
)
{
	const auto binaryFunc = [](const int x, const std::string& str) { return x + std::stoi(str); };
	auto curriedFunc = sf::bind_back(binaryFunc, 42, "1337");

	REQUIRE(TestType::cast(curriedFunc)() == 1379);
}

TEMPLATE_LIST_TEST_CASE(
	"functional::bind_back treats std::reference_wrapper types appropriated.",
	"[functional][functional::bind_back]",
	all_ref_mods_list
)
{
	const auto binaryFunc = [](const std::string& prefix, std::string& suffix) { suffix = prefix + std::move(suffix); };
	std::string str{" World!"};
	auto curriedFunc = sf::bind_back(binaryFunc, std::ref(str));

	TestType::cast(curriedFunc)("Hello,");

	REQUIRE_THAT(str, Catch::Matchers::Equals("Hello, World!"));
}
