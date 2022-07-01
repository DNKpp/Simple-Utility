#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "../helper.hpp"

#include "Simple-Utility/nullables/adapter.hpp"
#include "Simple-Utility/nullables/value_or.hpp"

#include <algorithm>
#include <vector>

namespace third_party
{
	struct third_party_type
	{
		int value{};

		int operator *() const
		{
			return value;
		}

		[[nodiscard]]
		constexpr bool operator ==(const third_party_type&) const = default;
	};
}

//
//template <class T>
//	requires std::same_as<T, third_party::third_party_type&>
//			|| std::same_as<T, const third_party::third_party_type&>
//			|| std::same_as<T, third_party::third_party_type&&>
//struct sl::nullables::customize::to_nullables_adapter_fn<T>
//{
//	using adapter_t = adapter<third_party::third_party_type, third_party::third_party_type>;
//
//	[[nodiscard]]
//	constexpr adapter_t operator()(const third_party::third_party_type& object) const
//	{
//		return adapter_t{ third_party::third_party_type{ 0 }, object };
//	}
//};
//
//template <class T>
//	requires std::same_as<T, third_party::third_party_type&>
//			|| std::same_as<T, const third_party::third_party_type&>
//			|| std::same_as<T, third_party::third_party_type&&>
//struct sl::nullables::customize::unwrap_adapted_fn<T>
//{
//	using value_type = int;
//
//	template <class TU>
//	[[nodiscard]]
//	constexpr decltype(auto) operator()(TU&& object) const
//	{
//		return std::forward<TU>(object).value;
//	}
//};

using namespace sl::nullables;

TEST_CASE("adapter adapting container iterators is valid input nullable", "[nullables][adapter][concept]")
{
	using itr_t = std::vector<int>::iterator;

	STATIC_REQUIRE(input_nullable<adapter<itr_t, itr_t>>);
}

//TEST_CASE("adapter is constructible with null object.", "[nullables][adapter]")
//{
//	using itr_t = std::vector<int>::const_iterator;
//	const std::vector v{ 1, 2, 3, 4 };
//	adapter<itr_t, itr_t> a{ std::end(v) };
//
//	REQUIRE(a == adapter_null);
//}

TEST_CASE("adapter is constructible from pair of iterators.", "[nullables][adapter]")
{
	const std::vector v{ 1, 2, 3, 4 };
	adapter a{ std::end(v), std::begin(v) };

	REQUIRE(a != adapter_null);
}

//TEST_CASE("adapter can be re-assigned later on.", "[nullables][adapter]")
//{
//	using itr_t = std::vector<int>::const_iterator;
//	const std::vector v{ 1, 2, 3, 4 };
//	adapter<itr_t, itr_t> a{ std::end(v) };
//
//	a = std::begin(v);
//
//	REQUIRE(a != adapter_null);
//}

TEMPLATE_TEST_CASE_SIG(
	"adapter can be used with value_or algorithms.",
	"[nullables][adapter][algorithm]",
	((bool VDummy, template <class> class TMod), VDummy, TMod),
	(true, as_lvalue_ref_t),
	(true, as_const_lvalue_ref_t),
	(true, as_rvalue_ref_t)
)
{
	const std::vector v{ 1, 2, 3, 4 };

	const auto [searchedValue, expectedValue] = GENERATE(
		(table<int, int>)({
			{ 2, 2 },
			{ -1, 1337 }
			})
	);
	adapter a{ std::end(v), std::ranges::find(v, searchedValue) };

	const int value = apply_mod<TMod>(a) | value_or(1337);

	REQUIRE(value == expectedValue);
}

TEST_CASE("lsjfl")
{
	const std::vector v{ 1, 2, 3, 4 };

	const int value = (adapter{ v } = std::ranges::find(v, 2))
					| value_or(1337);

	REQUIRE(value == 2);
}

//TEST_CASE("adapter can be used with custom types.", "[nullables][adapter][customization]")
//{
//	const third_party::third_party_type object{ 42 };
//
//	const int value = adapter{ object } | value_or(1337);
//}
