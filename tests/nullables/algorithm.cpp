//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "../helper.hpp"

#include "Simple-Utility/nullables/or_else.hpp"
#include "Simple-Utility/nullables/and_then.hpp"
#include "Simple-Utility/nullables/value_or.hpp"
#include "Simple-Utility/nullables/fwd_value.hpp"
#include "Simple-Utility/nullables/std_optional.hpp"

#include "Simple-Utility/functional/predicate.hpp"
#include "Simple-Utility/functional/transform.hpp"

#include <stdexcept>

using namespace sl::nullables;

inline constexpr auto do_nothing = [](auto&&...) {};

TEST_CASE("value_or returns the value of the nullable, when it's not equal to its null.", "[nullables][algorithm]")
{
	auto [sourceOptional, expected] = GENERATE(
		(table<const char*, char>)({
			{ "c", 'c' },
			{ nullptr, 'x' }
		})
	);
	algorithm_fn algorithm = value_or('x');

	const auto& lhsRefMod = GENERATE(make_all_ref_mods_generator());
	const auto& rhsRefMod = GENERATE(make_all_ref_mods_generator());
	const char s = cast(sourceOptional, lhsRefMod) | cast(algorithm, rhsRefMod);

	REQUIRE(s == expected);
}

TEST_CASE("value_or_fn can be piped with other functionals", "[nullables][algorithm][functional]")
{
	auto [sourceOptional, expected] = GENERATE(
		(table<const char*, int>)({
			{ "c", 'c' },
			{ nullptr, 'x' }
		})
	);

	const sl::functional::transform_fn transform = std::identity{}
													| value_or_fn('x')
													| sl::functional::as<int>;

	const auto& refMod = GENERATE(make_all_ref_mods_generator());
	REQUIRE(transform(cast(sourceOptional, refMod)) == expected);
}

TEST_CASE("fwd_value forwards the value of the nullable to the functional, when it's not equal to its null.","[nullables][algorithm]")
{
	auto [sourceOptional, expectedInvoked] = GENERATE(
		(table<const char*, bool>)({
			{ "c", true },
			{ nullptr, false }
		})
	);
	bool invoked{ false };
	auto algorithm = fwd_value([&invoked](const char) { invoked = true; });

	const auto& lhsRefMod = GENERATE(make_all_ref_mods_generator());
	const auto& rhsRefMod = GENERATE(make_all_ref_mods_generator());
	cast(sourceOptional, lhsRefMod) | cast(algorithm, rhsRefMod);

	REQUIRE(invoked == expectedInvoked);
}

TEST_CASE("fwd_value_fn can be piped with other functionals", "[nullables][algorithm][functional]")
{
	auto [sourceOptional, expectedInvoked] = GENERATE(
		(table<const char*, bool>)({
			{ "c", true },
			{ nullptr, false }
		})
	);

	bool invoked{ false };
	const sl::functional::transform_fn transform = std::identity{}
													| fwd_value_fn([&invoked](const char) { invoked = true; });

	const auto& refMod = GENERATE(make_all_ref_mods_generator());
	transform(cast(sourceOptional, refMod));

	REQUIRE(invoked == expectedInvoked);
}

TEST_CASE("and_then returns a new optional or its corresponding null object.", "[nullables][algorithm]")
{
	auto [sourceOptional, expected] = GENERATE(
		(table<const char*, std::optional<char>>)({
			{ "c", 'c' },
			{ nullptr, std::nullopt }
			})
	);
	auto algorithm = and_then(sl::functional::as<std::optional<char>>);

	const auto& lhsRefMod = GENERATE(make_all_ref_mods_generator());
	const auto& rhsRefMod = GENERATE(make_all_ref_mods_generator());
	const std::optional<char> s = cast(sourceOptional, lhsRefMod) | cast(algorithm, rhsRefMod);

	REQUIRE(s == expected);
}

TEST_CASE("and_then can be piped with other functionals", "[nullables][algorithm][functional]")
{
	auto [sourceOptional, expected] = GENERATE(
		(table<const char*, bool>)({
			{ "c", true },
			{ nullptr, false }
		})
	);
	const sl::functional::transform_fn transform = std::identity{}
													| and_then_fn(sl::functional::as<std::optional<char>>)
													| &std::optional<char>::has_value;

	const auto& refMod = GENERATE(make_all_ref_mods_generator());
	REQUIRE(transform(cast(sourceOptional, refMod)) == expected);
}

TEST_CASE(
	"or_else returns the given optional if it doesn't compare equally to its null object or returns the null object (provided func returns void).",
	"[nullables][algorithm]"
)
{
	const char* sourceOptional = GENERATE("c", nullptr);
	auto algorithm = or_else(do_nothing);

	const auto& lhsRefMod = GENERATE(make_all_ref_mods_generator());
	const auto& rhsRefMod = GENERATE(make_all_ref_mods_generator());
	const char* s = cast(sourceOptional, lhsRefMod) | cast(algorithm, rhsRefMod);

	REQUIRE(s == sourceOptional);
}

TEST_CASE(
	"or_else returns an optional with the same type, if the given optional compares equally to its null object and func has a return type other than void.",
	"[nullables][algorithm]"
)
{
	auto orig = "c";
	auto alt = "x";
	auto [sourceOptional, expected] = GENERATE_COPY(
		(table<const char*, const char*>)({
			{ orig, orig },
			{ nullptr, alt }
		})
	);
	auto algorithm = or_else([alt] { return alt; });

	const auto& lhsRefMod = GENERATE(make_all_ref_mods_generator());
	const auto& rhsRefMod = GENERATE(make_all_ref_mods_generator());
	const char* s = cast(sourceOptional, lhsRefMod) | cast(algorithm, rhsRefMod);

	REQUIRE(s == expected);
}

TEST_CASE("or_else can be piped with other functionals", "[nullables][algorithm][functional]")
{
	auto [sourceOptional, expectThrow] = GENERATE(
		(table<const char*, bool>)({
			{ "c", false },
			{ nullptr, true }
		})
	);

	const sl::functional::predicate_fn predicate = std::identity{}
													| or_else_fn([] { throw std::exception{}; })
													| [](auto t) { return t; }
													| sl::functional::equal >> nullptr;

	const auto& refMod = GENERATE(make_all_ref_mods_generator());
	if (expectThrow)
		REQUIRE_THROWS(predicate(cast(sourceOptional, refMod)));
	else
		REQUIRE_NOTHROW(predicate(cast(sourceOptional, refMod)));
}

/*
 * LCOV_EXCL_START
 */

TEST_CASE("value_or example showing the outcome when used with a non-null nullable.", "[nullables][algorithm][example]")
{
	//! [value_or valid]
	const std::optional result = std::optional{ 42 }
								| value_or(1337);

	REQUIRE(result == 42);
	//! [value_or valid]
}

TEST_CASE("value_or example showing the outcome when used with a null nullable.", "[nullables][algorithm][example]")
{
	//! [value_or invalid]
	const std::optional result = std::optional<int>{}
								| value_or(1337);

	REQUIRE(result == 1337);
	//! [value_or invalid]
}

TEST_CASE("fwd_value example showing the outcome when used with a non-null nullable.", "[nullables][algorithm][example]")
{
	//! [fwd_value valid]
	std::optional<int> target{};

	std::optional{ 42 }
		| fwd_value([&target](const int i) { target = i; });

	REQUIRE(target == 42);
	//! [fwd_value valid]
}

TEST_CASE("fwd_value example showing the outcome when used with a null nullable.", "[nullables][algorithm][example]")
{
	//! [fwd_value invalid]
	std::optional<int> target{};

	std::optional<int>{}
		| fwd_value([&target](const int i) { target = i; });

	REQUIRE(target == std::nullopt);
	//! [fwd_value invalid]
}

TEST_CASE("and_then example showing the outcome when used with a non-null nullable.", "[nullables][algorithm][example]")
{
	//! [and_then valid]
	const std::optional result = std::optional{ 42 }
								| and_then([](const int i) { return std::optional{ std::to_string(i) }; });

	REQUIRE(result == "42");
	//! [and_then valid]
}

TEST_CASE("and_then example showing the outcome when used with a null nullable.", "[nullables][algorithm][example]")
{
	//! [and_then invalid]
	const std::optional result = std::optional<int>{}
								| and_then([](const int i) { return std::optional{ std::to_string(i) }; });

	REQUIRE(result == std::nullopt);
	//! [and_then invalid]
}

TEST_CASE(
	"or_else example showing the outcome when used with a non-null nullable and returning non-void.",
	"[nullables][algorithm][example]"
)
{
	//! [or_else valid non-void return]
	const std::optional result = std::optional<int>{ 42 }
								| or_else([] { return 1337; });

	REQUIRE(result == 42);
	//! [or_else valid non-void return]
}

TEST_CASE(
	"or_else example showing the outcome when used with a null nullable and returning non-void.",
	"[nullables][algorithm][example]"
)
{
	//! [or_else invalid non-void return]
	const std::optional result = std::optional<int>{}
								| or_else([] { return 1337; });

	REQUIRE(result == 1337);
	//! [or_else invalid non-void return]
}

TEST_CASE(
	"or_else example showing the outcome when used with a null nullable and returning void.",
	"[nullables][algorithm][example]"
)
{
	//! [or_else invalid void return]
	const std::optional result = std::optional<int>{}
								| or_else([] { /* print error message for example */ });

	REQUIRE(result == std::nullopt);
	//! [or_else invalid void return]
}

TEST_CASE("example showing that nullable algorithms can be chained arbitrarily.", "[nullables][algorithm][example]")
{
	//! [algorithm chain]
	const std::string result = std::optional{ 42 }
								| or_else([] { throw std::runtime_error{ "invalid optional." }; })
								| and_then([](const int i) { return std::optional{ i * i }; })
								| and_then([](const int i) { return std::optional{ std::to_string(i) }; })
								| value_or("not set");

	REQUIRE(result == "1764");
	//! [algorithm chain]
}

TEST_CASE(
	"example showing that nullable algorithms can be chained arbitrarily with functional support.",
	"[nullables][algorithm][example]"
)
{
	//! [algorithm chain functional]
	namespace fn = sl::functional;

	const fn::transform_fn square{ [](const std::integral auto i) { return i * i; } };
	const fn::transform_fn to_string{ [](const std::integral auto i) { return std::to_string(i); } };

	const std::string result = std::optional{ 42 }
								| or_else([] { throw std::runtime_error{ "invalid optional." }; })
								| and_then(
									square
									| to_string
									| fn::as<std::optional<std::string>>
								)
								| value_or("not set");

	REQUIRE(result == "1764");
	//! [algorithm chain functional]
}

TEST_CASE(
	"example showing that nullable algorithms can be chained arbitrarily with functional composition support.",
	"[nullables][algorithm][example]"
)
{
	//! [algorithm chain functional composition]
	namespace fn = sl::functional;

	const fn::transform_fn square{ [](const std::integral auto i) { return i * i; } };
	const fn::transform_fn to_string{ [](const std::integral auto i) { return std::to_string(i); } };
	// note the _fn suffix on all algorithms
	const auto algorithmChain = or_else_fn([] { throw std::runtime_error{ "invalid optional." }; })
								| and_then_fn(
									square
									| to_string
									| fn::as<std::optional<std::string>>
								)
								| value_or_fn("not set");

	const std::string result = algorithmChain(std::optional{ 42 });

	REQUIRE(result == "1764");
	//! [algorithm chain functional composition]
}

/*
 * LCOV_EXCL_STOP
 */
