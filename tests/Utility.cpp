//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/Utility.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>

#include "Simple-Utility/test_util/Catch2Ext.hpp"

#include <string>

TEMPLATE_TEST_CASE_SIG(
	"Higher priority_tag always inherit from the lower ones.",
	"[utility]",
	((bool expected, class Derived, class Base), expected, Derived, Base),
	(true, sl::priority_tag<10>, sl::priority_tag<0>),
	(true, sl::priority_tag<10>, sl::priority_tag<1>),
	(true, sl::priority_tag<1>, sl::priority_tag<0>),
	(true, sl::priority_tag<1>, sl::priority_tag<1>),
	(false, sl::priority_tag<0>, sl::priority_tag<1>),
	(false, sl::priority_tag<1>, sl::priority_tag<10>)
)
{
	STATIC_REQUIRE(expected == std::derived_from<Derived, Base>);
}

namespace
{
	template <class... Ts>
	class immobile
	{
	public:
		~immobile() = default;

		immobile(const immobile&) = delete;
		immobile& operator =(const immobile&) = delete;
		immobile(immobile&&) = delete;
		immobile& operator =(immobile&&) = delete;

		template <class... Args>
			requires std::constructible_from<std::tuple<Ts...>, Args...>
		explicit immobile(Args&&... args)
			: value{std::forward<Args>(args)...}
		{
		}

		std::tuple<Ts...> value;
	};

	template <class T>
	class ForwardingCTor
	{
	public:
		template <class Arg>
		explicit ForwardingCTor(Arg&& arg)
			: value{static_cast<T>(std::forward<Arg>(arg))}
		{
		}

		T value;
	};

	template <class T>
	class InPlaceExpecting
	{
	public:
		template <class... Args>
		explicit InPlaceExpecting(sl::in_place_constructor<T, Args...>&& args)
			: value{static_cast<T>(std::move(args))}
		{
		}

		T value;
	};

	template <class T1, class T2, class T3>
	class MultiInPlaceExpecting
	{
	public:
		template <class... T1s, class... T2s, class... T3s>
		explicit MultiInPlaceExpecting(
			sl::in_place_constructor<T1, T1s...>&& args1,
			sl::in_place_constructor<T2, T2s...>&& args2,
			sl::in_place_constructor<T3, T3s...>&& args3
		)
			: value1{static_cast<T1>(std::move(args1))},
			value2{static_cast<T2>(std::move(args2))},
			value3{static_cast<T3>(std::move(args3))}
		{
		}

		T1 value1;
		T2 value2;
		T3 value3;
	};
}

TEST_CASE("in_place_constructor can be utilized from types, which expects it as its ctor argument.", "[utility]")
{
	SECTION("Supports convenient default construction")
	{
		const InPlaceExpecting<std::optional<int>> obj{{}};

		REQUIRE(std::nullopt == obj.value);
	}

	SECTION("Supports CTAD")
	{
		SECTION("When move constructing.")
		{
			std::string str{"Hello, World!"};
			const InPlaceExpecting obj{sl::in_place<std::string>(std::move(str))};

			REQUIRE("Hello, World!" == obj.value);
		}

		SECTION("When copy constructing.")
		{
			const std::string str{"Hello, World!"};
			const InPlaceExpecting obj{sl::in_place<std::string>(str)};

			REQUIRE("Hello, World!" == obj.value);
		}

		SECTION("When argument constructing.")
		{
			const InPlaceExpecting obj{sl::in_place<std::string>("Hello, World!")};

			REQUIRE("Hello, World!" == obj.value);
		}

		SECTION("Even when constructing immobile types.")
		{
			const InPlaceExpecting obj{sl::in_place<immobile<std::string, int>>("Hello, World!", 42)};

			REQUIRE(std::tuple{"Hello, World!", 42} == obj.value.value);
		}
	}
}

TEST_CASE("in_place_constructor can be utilized on custruction, when used on perfect forwarding ctors..", "[utility]")
{
	SECTION("Supports default construction")
	{
		const ForwardingCTor<std::optional<int>> obj{sl::in_place<std::optional<int>>()};

		REQUIRE(std::nullopt == obj.value);
	}

	SECTION("When move constructing.")
	{
		std::string str{"Hello, World!"};
		const ForwardingCTor<std::string> obj{sl::in_place<std::string>(std::move(str))};

		REQUIRE("Hello, World!" == obj.value);
	}

	SECTION("When copy constructing.")
	{
		const std::string str{"Hello, World!"};
		const ForwardingCTor<std::string> obj{sl::in_place<std::string>(str)};

		REQUIRE("Hello, World!" == obj.value);
	}

	SECTION("When argument constructing.")
	{
		const ForwardingCTor<std::string> obj{sl::in_place<std::string>("Hello, World!")};

		REQUIRE("Hello, World!" == obj.value);
	}

	SECTION("Even when constructing immobile types.")
	{
		const ForwardingCTor<immobile<std::string, int>> obj{sl::in_place<immobile<std::string, int>>("Hello, World!", 42)};

		REQUIRE(std::tuple{"Hello, World!", 42} == obj.value.value);
	}
}

TEST_CASE("in_place_constructor can be utilized from types, which expects some of them as its ctor arguments.", "[utility]")
{
	SECTION("Supports convenient default construction")
	{
		const MultiInPlaceExpecting<std::optional<int>, std::string, int> obj{{}, {}, {}};

		REQUIRE(std::nullopt == obj.value1);
		REQUIRE(obj.value2.empty());
		REQUIRE(0 == obj.value3);
	}

	SECTION("Supports CTAD")
	{
		const MultiInPlaceExpecting obj{
			sl::in_place<std::optional<int>>(1337),
			sl::in_place<std::string>("Hello, World!"),
			sl::in_place<int>(42)
		};

		REQUIRE(1337 == obj.value1);
		REQUIRE("Hello, World!" == obj.value2);
		REQUIRE(42 == obj.value3);
	}

	SECTION("Supports immobile types")
	{
		const MultiInPlaceExpecting obj{
			sl::in_place<immobile<std::string, int>>("World, Hello!", 1337),
			sl::in_place<std::string>("Hello, World!"),
			sl::in_place<int>(42)
		};

		REQUIRE(std::tuple{"World!, Hello", 42} == obj.value1.value);
		REQUIRE("Hello, World!" == obj.value2);
		REQUIRE(42 == obj.value3);
	}
}
