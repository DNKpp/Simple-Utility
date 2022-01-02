//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch.hpp>

#include "helper.hpp"

#include "Simple-Utility/nullables.hpp"
#include "Simple-Utility/unique_handle.hpp"

#include <cassert>
#include <memory>
#include <optional>
#include <sstream>
#include <string>

namespace
{
	struct input_nullable_target_t
	{
		int x{};

		constexpr input_nullable_target_t() noexcept = default;

		constexpr input_nullable_target_t(int x) noexcept
			: x{ x }
		{
		}

		[[nodiscard]]
		constexpr bool operator ==(empty_t) const noexcept
		{
			return false;
		}

		[[nodiscard]]
		constexpr const int& operator *() const noexcept
		{
			return x;
		}
	};

	//! [nullables your_type definition]
	class your_type
	{
	public:
		your_type() = default;

		your_type(int value)
			: m_IsSet{ true },
			m_Value{ value }
		{
		}

		[[nodiscard]]
		int get_value() const
		{
			assert(m_IsSet && "Value is not set.");
			return m_Value;
		}

		[[nodiscard]]
		bool is_set() const
		{
			return m_IsSet;
		}

	private:
		bool m_IsSet{ false };
		int m_Value{};
	};

	//! [nullables your_type definition]

	//! [nullables your_type null-type]
	class null_your_type_t
	{
	public:
		null_your_type_t() = default;

		[[nodiscard]]
		bool operator ==(const your_type& other) const
		{
			return !other.is_set();
		}

		// this is required if you want to use your_type as nullable.
		// If you only want to satisfy input_nullable concept, this can be omitted.
		[[nodiscard]]
		operator your_type() const
		{
			return {};
		}
	};

	//! [nullables your_type null-type]

	//! [nullables your_type value_unchecked]
	[[nodiscard]]
	int value_unchecked(const your_type& target) noexcept
	{
		return target.get_value();
	}

	//! [nullables your_type value_unchecked]
}

//! [nullables your_type traits]
template <>
struct sl::nullables::nullable_traits<your_type>
{
	using value_type = int;
	constexpr static null_your_type_t null{};
};

//! [nullables your_type traits]

//! [nullables custom type traits]
template <>
struct sl::nullables::nullable_traits<input_nullable_target_t>
{
	using value_type = int;
	constexpr static empty_t null{};
};

//! [nullables custom type traits]

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"nullable_value_t should expose expected value type.",
	"[nullables]",
	// VIgnore is necessary to disambiguate the internal catch overload
	((class TNullable, class TExpectedValueType, auto VIgnore), TNullable, TExpectedValueType, VIgnore),
	(sl::unique_handle<int>, int, false),
	(sl::unique_handle<int>&&, int, false),
	(const sl::unique_handle<int>&&, int, false),
	(sl::unique_handle<int>&, int, false),
	(const sl::unique_handle<int>&, int, false),
	(const sl::unique_handle<sl::unique_handle<int>>&, sl::unique_handle<int>, false),

	(std::optional<float>, float, false),
	(int*, int, false),
	(std::unique_ptr<int>, int, false),
	(std::shared_ptr<int>, int, false),
	// check for external customizability
	(input_nullable_target_t, int, false),
	(your_type, int, false)
)
#pragma warning(default: 26444)
{
	using sl::nullables::nullable_value_t;

	REQUIRE(std::same_as<nullable_value_t<TNullable>, TExpectedValueType>);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"nullable_null_v should expose expected null object.",
	"[nullables]",
	// VIgnore is necessary to disambiguate the internal catch overload
	((class TNullable, class TExpectedNullType, auto VIgnore), TNullable, TExpectedNullType, VIgnore),
	(sl::unique_handle<int>, sl::nullhandle_t, false),
	(sl::unique_handle<int>&&, sl::nullhandle_t, false),
	(const sl::unique_handle<int>&&, sl::nullhandle_t, false),
	(sl::unique_handle<int>&, sl::nullhandle_t, false),
	(const sl::unique_handle<int>&, sl::nullhandle_t, false),
	(const sl::unique_handle<sl::unique_handle<int>>&, sl::nullhandle_t, false),

	(std::optional<float>, std::nullopt_t, false),
	(int*, std::nullptr_t, false),
	(std::unique_ptr<int>, std::nullptr_t, false),
	(std::shared_ptr<int>, std::nullptr_t, false),
	// check for external customizability
	(input_nullable_target_t, empty_t, false),
	(your_type, null_your_type_t, false)
)
#pragma warning(default: 26444)
{
	using sl::nullables::nullable_null_v;

	REQUIRE(std::same_as<std::remove_cvref_t<decltype(nullable_null_v<TNullable>)>, TExpectedNullType>);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"input_nullable should determine whether T is a input_nullable",
	"[nullables]",
	((class T, bool VExpectedResult), T, VExpectedResult),
	(int, false),
	(int*, true),
	(sl::unique_handle<int>, true),
	(std::optional<float>, true),
	(std::unique_ptr<int>, true),
	(std::shared_ptr<int>, true),
	(input_nullable_target_t, true),
	(your_type, true)
)
#pragma warning(default: 26444)
{
	using sl::nullables::input_nullable;

	REQUIRE(input_nullable<T> == VExpectedResult);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE_SIG
(
	"nullable should determine whether T is a nullable",
	"[nullables]",
	((class T, bool VExpectedResult), T, VExpectedResult),
	(int, false),
	(int*, true),
	(sl::unique_handle<int>, true),
	(std::optional<float>, true),
	(std::unique_ptr<int>, true),
	(std::shared_ptr<int>, true),
	(input_nullable_target_t, false),
	(your_type, true)
)
#pragma warning(default: 26444)
{
	using sl::nullables::nullable;

	REQUIRE(nullable<T> == VExpectedResult);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE
(
	"value_unchecked should use the expected overload",
	"[nullables]",
	sl::unique_handle<int>,
	std::optional<int>,
	input_nullable_target_t
)
#pragma warning(default: 26444)
{
	using sl::nullables::value_unchecked;

	constexpr TestType object{ 42 };

	STATIC_REQUIRE(value_unchecked(object) == 42);
}

TEST_CASE("or_else should return the expected object when received as rvalue ref", "[nullables][algorithm]")
{
	using sl::nullables::or_else;

	constexpr auto func = [] { return 1337; };
	sl::unique_handle<int> handle{};

	SECTION("when handle is empty, the invoke result should be returned")
	{
		handle = std::move(handle) | or_else{ func };
		REQUIRE(handle == 1337);
	}

	SECTION("when handle is non-empty, the handle itself should be returned")
	{
		handle = 42;
		handle = std::move(handle) | or_else{ func };
		REQUIRE(handle == 42);
	}
}

TEST_CASE("or_else should return the expected object when received as const lvalue ref", "[nullables][algorithm]")
{
	using sl::nullables::or_else;

	constexpr auto func = [] { return 1337; };
	std::optional<int> opt{};

	SECTION("when optional is empty, the invoke result should be returned")
	{
		opt = std::as_const(opt) | or_else{ func };
		REQUIRE(opt == 1337);
	}

	SECTION("when optional is non-empty, the optional itself should be returned")
	{
		opt = 42;
		opt = std::as_const(opt) | or_else{ func };
		REQUIRE(opt == 42);
	}
}

TEST_CASE("or_else should return the expected object when used in a chain", "[nullables][algorithm]")
{
	using sl::nullables::or_else;
	std::optional<int> opt{};

	SECTION("when optional is empty, the first non empty invoke result should be returned")
	{
		opt = opt | or_else{ [] { return std::nullopt; } }
			| or_else{ [] { return 42; } };

		REQUIRE(opt == 42);
	}

	SECTION("when optional is non-empty, the optional itself should be returned")
	{
		opt = 1337;
		opt = opt | or_else{ [] { return std::nullopt; } }
			| or_else{ [] { return 42; } };

		REQUIRE(opt == 1337);
	}
}

TEST_CASE("or_else may also return void", "[nullables][algorithm]")
{
	using sl::nullables::or_else;
	std::optional<int> opt{};

	SECTION("when optional is empty, or_else branch must be executed and std::nullopt gets returned.")
	{
		int or_else_counter = 0;
		opt = opt | or_else{ [&or_else_counter] { ++or_else_counter; } };

		REQUIRE(opt == std::nullopt);
		REQUIRE(or_else_counter == 1);
	}
}

TEST_CASE("and_then minimal requirements should be satisfied by input_nullable", "[nullables][algorithm]")
{
	using sl::nullables::and_then;

	input_nullable_target_t target{ 42 };

	const sl::unique_handle result = target | and_then{ [](int value) { return sl::unique_handle{ value }; } };

	REQUIRE(result == 42);
}

TEST_CASE("and_then should return the expected value", "[nullables][algorithm]")
{
	using sl::nullables::and_then;

	constexpr auto func = [](const auto& value)-> sl::unique_handle<std::string> { return std::to_string(value); };
	sl::unique_handle<int> handle{};

	SECTION("when handle is empty, a null object should be returned")
	{
		const sl::unique_handle<std::string> result = handle | and_then{ func };
		REQUIRE(result == sl::nullhandle);
	}

	SECTION("when handle is non-empty, the invoke result should be returned")
	{
		handle = 42;
		const sl::unique_handle<std::string> result = handle | and_then{ func };
		REQUIRE(result == "42");
	}
}

TEST_CASE("and_then should return the expected value when with mixed nullable types", "[nullables][algorithm]")
{
	using sl::nullables::and_then;

	constexpr auto func = [](const auto& value)-> std::optional<std::string> { return std::to_string(value); };
	sl::unique_handle<int> handle{};

	SECTION("when handle is empty, a null object should be returned")
	{
		const std::optional<std::string> result = handle | and_then{ func };
		REQUIRE(result == std::nullopt);
	}

	SECTION("when handle is non-empty, the invoke result should be returned")
	{
		handle = 42;
		const std::optional<std::string> result = handle | and_then{ func };
		REQUIRE(result == "42");
	}
}

TEST_CASE("and_then should be usable in chains", "[nullables][algorithm]")
{
	using sl::nullables::and_then;

	constexpr auto toString = [](const auto& value) -> sl::unique_handle<std::string> { return std::to_string(value); };
	constexpr auto strToInt = [](const auto& str) -> std::optional<int> { return std::stoi(str); };

	sl::unique_handle<int> handle{};

	SECTION("when handle is empty, the last nullable object should be returned")
	{
		const std::optional<int> result = handle | and_then{ toString }
										| and_then{ strToInt };
		REQUIRE(result == std::nullopt);
	}

	SECTION("when handle is non-empty, the final invoke result should be returned")
	{
		handle = 1337;
		const std::optional<int> result = handle | and_then{ toString }
										| and_then{ strToInt };
		REQUIRE(result == 1337);
	}
}

TEST_CASE("value_or minimal requirements should be satisfied by input_nullable", "[nullables][algorithm]")
{
	using sl::nullables::value_or;

	input_nullable_target_t target{ 1337 };

	const int result = target | value_or{ 42 };

	REQUIRE(result == 1337);
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE
(
	"value_or should return the expected object",
	"[nullables][algorithm]",
	sl::unique_handle<int>&,
	const sl::unique_handle<int>&,
	sl::unique_handle<int>&&,
	const sl::unique_handle<int>&&
)
#pragma warning(default: 26444)
{
	using sl::nullables::value_or;

	sl::unique_handle<int> handle{};

	SECTION("when handle is empty, the alternative should be returned")
	{
		const int result = static_cast<TestType>(handle) | value_or{ 1337 };
		REQUIRE(result == 1337);
	}

	SECTION("when handle is non-empty, the handle itself should be returned")
	{
		handle = 42;
		const int result = static_cast<TestType>(handle) | value_or{ 1337 };
		REQUIRE(result == 42);
	}
}

TEST_CASE("value_or should favor member function of std::optional", "[nullables][algorithm]")
{
	using namespace sl::nullables;

	std::optional<std::string> opt{ "test" };

	std::string result = std::move(opt) | value_or{ "empty" };

	// Don't know how to actually test if overload has been taken, thus it's simply a compile check for now
}

TEST_CASE("nullable algorithms should be usable in chains", "[nullables][algorithm]")
{
	using namespace sl::nullables;

	constexpr auto toString = [](const auto& value) -> sl::unique_handle<std::string> { return std::to_string(value); };
	constexpr auto failString = []() -> std::string { return "fail"; };

	sl::unique_handle<int> handle{};

	SECTION("when handle is empty, the result of the or_else algorithm should be returned")
	{
		const std::string result = handle | and_then{ toString }
									| or_else{ failString }
									| value_or("value_or fail");
		REQUIRE(result == "fail");
	}

	SECTION("when handle is non-empty, the result of the and_then algorithm should be returned")
	{
		handle = 42;
		const std::string result = handle | and_then{ toString }
									| or_else{ failString }
									| value_or("value_or fail");
		REQUIRE(result == "42");
	}
}

#pragma warning(disable: 26444)
TEMPLATE_TEST_CASE
(
	"nullable algorithms should be usable with std::unique_ptr",
	"[nullables][algorithm]",
	std::unique_ptr<int>&,
	const std::unique_ptr<int>&,
	std::unique_ptr<int>&&,
	const std::unique_ptr<int>&&
)
#pragma warning(default: 26444)
{
	using namespace sl::nullables;

	constexpr auto square = [](const auto& value) { return std::make_unique<int>(value * value); };
	constexpr auto oneAndOnlyTruth = [] { return std::make_unique<int>(42); };

	std::unique_ptr<int> ptr{};

	SECTION("when ptr is empty, the result of the or_else algorithm should be returned")
	{
		const int result = static_cast<TestType>(ptr) | and_then{ square }
							| or_else{ oneAndOnlyTruth }
							| value_or(-42);
		REQUIRE(result == 42);
	}

	SECTION("when ptr is not empty, the result of the and_then algorithm should be returned")
	{
		ptr = std::make_unique<int>(1337);
		const int result = static_cast<TestType>(ptr) | and_then{ square }
							| or_else{ oneAndOnlyTruth }
							| value_or(-42);
		REQUIRE(result == 1337 * 1337);
	}
}

TEST_CASE("nullable algorithms should be usable with raw ptrs", "[nullables][algorithm]")
{
	using namespace sl::nullables;

	auto square = [](int& value)
	{
		value *= value;
		return &value;
	};
	int onlyTruth{ 42 };
	auto oneAndOnlyTruth = [&onlyTruth]() { return &onlyTruth; };

	int* ptr = nullptr;

	SECTION("when ptr is empty, the result of the or_else algorithm should be returned")
	{
		const int* result = ptr | or_else{ oneAndOnlyTruth };
		REQUIRE(*result == 42);
	}

	SECTION("when ptr is empty, and or_else returns void, result should be null")
	{
		int* result = ptr | or_else
					{
						[]
						{
						}
					};

		REQUIRE(result == nullptr);
	}

	SECTION("when ptr is empty the value_or branch should be executed")
	{
		const int result = ptr | value_or{ 42 };

		REQUIRE(result == 42);
	}

	SECTION("when ptr is non-empty, its value should be returned.")
	{
		int value = 1337;
		ptr = &value;
		const int result = ptr | value_or{ 42 };

		REQUIRE(result == 1337);
	}

	SECTION("when ptr is not empty, the result of the and_then algorithm should be returned")
	{
		int value = 1337;
		ptr = &value;
		const int* result = ptr | and_then{ square };

		REQUIRE(*result == 1337 * 1337);
	}
}

TEST_CASE("artifically more advanced example", "[nullables][algorithm]")
{
	using namespace sl::nullables;

	constexpr auto oneAndOnlyTruth = [] { return std::make_unique<int>(42); };

	std::optional<std::unique_ptr<int>> opt{};

	SECTION("let's just chain the optional through")
	{
		const std::unique_ptr<int> result = std::move(opt) |
											/*not hit*/
											and_then{ [](auto&&) -> std::optional<std::unique_ptr<int>> { return {}; } }
											| or_else{ oneAndOnlyTruth }
											| value_or(std::make_unique<int>(-42));
		REQUIRE(*result == 42);
	}

	SECTION("unwrap the optional and forward the contained unique_ptr")
	{
		constexpr auto square = []<class T>(T&& value)
		{
			*value *= *value;
			return std::forward<T>(value);
		};

		opt = std::make_unique<int>(1337);
		const int result = std::move(opt) | and_then{ square }
							| or_else{ oneAndOnlyTruth }
							| value_or{ -42 };
		REQUIRE(result == 1337 * 1337);
	}
}

TEST_CASE("or_else usage example with copyable types", "[nullables][algorithm][example]")
{
	SECTION("or_else forwards the nullable if its valid.")
	{
		//! [or_else valid copyable]
		namespace sn = sl::nullables;

		std::optional<int> opt{ 42 };
		const std::optional<int> result = opt | sn::or_else{ [] { return 1337; } };

		REQUIRE(result == 42);
		//! [or_else valid copyable]
	}

	SECTION("or_else executes the function and returns its returned value in a new nullable.")
	{
		//! [or_else invalid value non-void return copyable]
		namespace sn = sl::nullables;

		std::optional<int> opt{ std::nullopt };
		const std::optional<int> result = opt | sn::or_else{ [] { return 1337; } };

		REQUIRE(result == 1337);
		//! [or_else invalid value non-void return copyable]
	}

	SECTION("or_else executes the function and returns its returned value in a new nullable.")
	{
		std::ostringstream logger{};

		//! [or_else invalid value void return copyable]
		namespace sn = sl::nullables;

		std::optional<int> opt{ std::nullopt };
		const std::optional<int> result = opt | sn::or_else{ [&] { logger << "optional was invalid."; } };

		REQUIRE(result == std::nullopt);
		//! [or_else invalid value void return copyable]
	}
}

TEST_CASE("and_then usage example with copyable types", "[nullables][algorithm][example]")
{
	constexpr auto toString = [](const int& value) -> std::optional<std::string> { return std::to_string(value); };

	SECTION("and_then returns the null-object if the nullable is invalid.")
	{
		//! [and_then invalid copyable]
		namespace sn = sl::nullables;

		std::optional<int> opt{ std::nullopt };
		const std::optional<std::string> result = opt
												| sn::and_then{ toString }; // won't be hit, because opt is empty

		REQUIRE(result == std::nullopt);
		//! [and_then invalid copyable]
	}

	SECTION("and_then returns the null-object if the nullable is invalid.")
	{
		//! [and_then valid copyable]
		namespace sn = sl::nullables;

		std::optional<int> opt{ 1337 };
		const std::optional<std::string> result = opt
												| sn::and_then{ toString }; // will be hit, because opt contains a value

		REQUIRE(result == "1337");
		//! [and_then valid copyable]
	}
}

TEST_CASE("value_or usage example with copyable types", "[nullables][algorithm][example]")
{
	SECTION("value_or returns the alternative if the nullable is invalid.")
	{
		//! [value_or invalid copyable]
		namespace sn = sl::nullables;

		std::optional<int> opt{ std::nullopt };
		const int result = opt | sn::value_or{ 42 };

		REQUIRE(result == 42);
		//! [value_or invalid copyable]
	}

	SECTION("value_or returns the value if the nullable is invalid.")
	{
		//! [value_or valid copyable]
		namespace sn = sl::nullables;

		std::optional<int> opt{ 1337 };
		const int result = opt | sn::value_or{ 42 };

		REQUIRE(result == 1337);
		//! [value_or valid copyable]
	}
}

namespace
{
	sl::unique_handle<int> exec_transaction(std::string key)
	{
		return 42;
	}
}

TEST_CASE("chain usage example with movable type", "[nullables][algorithm][example]")
{
	SECTION("value_or returns the alternative if the nullable is invalid.")
	{
		std::ostringstream logger{};

		//! [nullables algorithm chain movable]
		namespace sn = sl::nullables;

		std::unique_ptr ptr{ std::make_unique<std::string>("someImportantTransaction") };

		const std::string transactionId = std::move(ptr)
										| sn::and_then
										{
											[](std::string& transaction) -> sl::unique_handle<int>
											{
												// some faky transaction; returns 42 wrapped in a sl::unique_handle<int> here
												return exec_transaction(std::move(transaction));
											}
										}
										| sn::or_else([&] { logger << "Unspecified database error occurred.\n"; })
										// pay attention for the nullable type on the next line
										| sn::and_then([](int id) { return std::optional{ std::to_string(id) }; })
										| sn::value_or("invalid transaction id");
		//! [nullables algorithm chain movable]

		REQUIRE(transactionId == "42");
	}
}
