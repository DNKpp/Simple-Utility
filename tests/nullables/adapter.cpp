//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "../helper.hpp"

#include "Simple-Utility/nullables/adapter.hpp"
#include "Simple-Utility/nullables/and_then.hpp"
#include "Simple-Utility/nullables/fwd_value.hpp"
#include "Simple-Utility/nullables/or_else.hpp"
#include "Simple-Utility/nullables/value_or.hpp"

#include "Simple-Utility/nullables/std_optional.hpp"

#include "Simple-Utility/functional/tuple.hpp"

#include <algorithm>
#include <map>
#include <vector>

namespace your_ns
{
	struct your_type
	{
		int value{};

		[[nodiscard]]
		constexpr bool operator ==(const your_type&) const = default;
	};

	[[nodiscard]]
	constexpr int unwrap_adapted(const your_type& object)
	{
		return object.value;
	}

	[[nodiscard]]
	constexpr sl::nullables::adapter<your_type, your_type> to_nullables_adapter(const your_type& object)
	{
		return {your_type{0}, object};
	}
}

using namespace sl::nullables;

TEMPLATE_TEST_CASE_SIG(
	"adaptable_with determines whether two types satisfy the requirements.",
	"[nullables][adapter][concept]",
	((bool VExpected, class TNull, class TAdapted), VExpected, TNull, TAdapted),
	(true, std::vector<int>::iterator, std::vector<int>::iterator),
	(false, int, std::vector<int>::iterator),
	(false, int, int)
)
{
	STATIC_REQUIRE(adaptable_with<TAdapted, TNull> == VExpected);
}

TEST_CASE("adapter satisfies the requirements of input_nullables.", "[nullables][adapter][concept]")
{
	STATIC_REQUIRE(input_nullable<adapter<std::vector<int>::iterator, std::vector<int>::iterator>>);
}

TEST_CASE("adapter is constructible with null object.", "[nullables][adapter]")
{
	using itr_t = std::vector<int>::const_iterator;
	const std::vector v{1, 2, 3, 4};
	adapter<itr_t, itr_t> a{std::end(v)};

	REQUIRE(a == adapter_null);
}

TEST_CASE("adapter is constructible with adapter_null_t when null object can be default constructed.", "[nullables][adapter]")
{
	using itr_t = std::vector<int>::const_iterator;
	adapter<itr_t, itr_t> a{adapter_null};

	REQUIRE(a == adapter_null);
}

TEST_CASE("adapter is constructible with null object with explizit in_place_null tag.", "[nullables][adapter]")
{
	using itr_t = std::vector<int>::const_iterator;
	const std::vector v{1, 2, 3, 4};
	adapter<itr_t, itr_t> a{in_place_null, std::end(v)};

	REQUIRE(a == adapter_null);
}

TEST_CASE("adapter is constructible from pair of iterators.", "[nullables][adapter]")
{
	const std::vector v{1, 2, 3, 4};
	adapter a{std::end(v), std::begin(v)};

	REQUIRE(a != adapter_null);
}

TEST_CASE("adapter can be null-assigned.", "[nullables][adapter]")
{
	using itr_t = std::vector<int>::const_iterator;
	const std::vector v{1, 2, 3, 4};
	adapter<itr_t, itr_t> a{std::end(v)};

	a = adapter_null;

	REQUIRE(a == adapter_null);
}

TEST_CASE("adapter can be re-assigned later on.", "[nullables][adapter]")
{
	using itr_t = std::vector<int>::const_iterator;
	const std::vector v{1, 2, 3, 4};
	adapter<itr_t, itr_t> a{std::end(v)};

	a = std::begin(v);

	REQUIRE(a != adapter_null);
}

TEMPLATE_LIST_TEST_CASE(
	"adapter can be used with value_or algorithms.",
	"[nullables][adapter][algorithm]",
	all_ref_mods_list
)
{
	const std::vector v{1, 2, 3, 4};

	const auto [searchedValue, expectedValue] = GENERATE(
		(table<int, int>)({
			{ 2, 2 },
			{ -1, 1337 }
			})
	);
	adapter a{std::end(v), std::ranges::find(v, searchedValue)};

	const int value = TestType::cast(a) | value_or(1337);

	REQUIRE(value == expectedValue);
}

TEST_CASE("adapter can be constructed from borrowed ranges and reassigned later on.", "[nullables][adapter]")
{
	const std::vector v{1, 2, 3, 4};

	const int value = (adapter{v} = std::ranges::find(v, 2))
					| value_or(1337);

	REQUIRE(value == 2);
}

TEST_CASE("fsdadapter can be constructed from borrowed ranges and reassigned later on.", "[nullables][adapter]")
{
	const std::vector v{1, 2, 3, 4};

	const int value = (adapter{v} = std::ranges::find(v, 2))
					| and_then(sl::functional::as<std::optional<int>>)
					| value_or(1337);

	REQUIRE(value == 2);
}

TEST_CASE("adapter can be used with custom types.", "[nullables][adapter][customization]")
{
	const your_ns::your_type object{42};

	const int value = adapter{object} | value_or(1337);

	REQUIRE(value == 42);
}

/*
 * LCOV_EXCL_START
 */

TEST_CASE("adapter can simplfy code dealing with iterators.", "[nullables][adapter][example]")
{
	//! [adapter comparison]
	namespace fn = sl::functional;
	namespace na = sl::nullables;

	const std::map<int, const char*> global_storage{
		{42, "Hello, World!"},
		{1337, "Insert Coin"}
	};

	const auto get_from_storage_stl = [&](const int key) -> std::optional<std::string_view>
	{
		if (const auto itr = global_storage.find(key); itr != std::end(global_storage))
		{
			return itr->second;
		}
		return std::nullopt;
	};

	const auto get_from_storage_adapted = [&](const int key)
	{
		return na::adapter{global_storage.end(), global_storage.find(key)}
				| na::and_then(fn::tuple::get_at<1> | fn::as<std::optional<std::string_view>>);
	};
	//! [adapter comparison]

	REQUIRE(get_from_storage_stl(1337) == get_from_storage_adapted(1337));
	REQUIRE(get_from_storage_stl(42) == get_from_storage_adapted(42));
	REQUIRE(get_from_storage_stl(1) == get_from_storage_adapted(1));
}

/*
 * LCOV_EXCL_STOP
 */
