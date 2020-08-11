
//          Copyright Dominic Koepke 2019 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "catch.hpp"
#include "Simple-Utility/operators/arithmetic.hpp"
#include "Simple-Utility/operators/bitwise.hpp"

TEST_CASE("Bitwise operators", "[Operators]")
{
	using namespace sl;

	struct Test :
		operators::BitwiseLogical,
		operators::BitwiseShift
	{
		unsigned int data = 0;

		explicit Test(unsigned int data_) noexcept :
			data{ data_ }
		{
		}
		
		Test& operator &=(const Test& other) noexcept
		{
			data &= other.data;
			return *this;
		}

		Test& operator |=(const Test& other) noexcept
		{
			data |= other.data;
			return *this;
		}

		Test& operator ^=(const Test& other) noexcept
		{
			data ^= other.data;
			return *this;
		}

		Test& operator <<=(int range) noexcept
		{
			data <<= range;
			return *this;
		}

		Test& operator >>=(int range) noexcept
		{
			data >>= range;
			return *this;
		}
	};

	Test test1{ 15 };
	Test test2{ 3 };

	SECTION("test logical and")
	{
		REQUIRE((test1 & test2).data == (test1.data & test2.data));
	}
	SECTION("test logical or")
	{
		REQUIRE((test1 | test2).data == (test1.data | test2.data));
	}

	SECTION("test logical xor")
	{
		REQUIRE((test1 ^ test2).data == (test1.data ^ test2.data));
	}

	SECTION("test right shift")
	{
		REQUIRE((test1 >> 2).data == test2.data);
	}

	SECTION("test left shift")
	{
		REQUIRE((test1 << 1).data == (test2 << 3).data + 6);	// 30 == 30
	}
}

TEST_CASE("Arithmetic operators", "[Operators]")
{
	using namespace sl;

	struct Test :
		operators::Arithmetic
	{
		unsigned int data = 0;

		explicit Test(unsigned int data_) noexcept :
			data{ data_ }
		{
		}
		
		Test& operator +=(const Test& other) noexcept
		{
			data += other.data;
			return *this;
		}

		Test& operator -=(const Test& other) noexcept
		{
			data -= other.data;
			return *this;
		}

		Test& operator *=(const Test& other) noexcept
		{
			data *= other.data;
			return *this;
		}

		Test& operator /=(const Test& other) noexcept
		{
			data /= other.data;
			return *this;
		}

		Test& operator %=(const Test& other) noexcept
		{
			data %= other.data;
			return *this;
		}
	};

	Test test1{ 15 };
	Test test2{ 3 };

	SECTION("test arithmetic plus")
	{
		REQUIRE((test1 + test2).data == (test1.data + test2.data));
	}
	SECTION("test arithmetic minus")
	{
		REQUIRE((test1 - test2).data == (test1.data - test2.data));
	}
	SECTION("test arithmetic multiply")
	{
		REQUIRE((test1 * test2).data == (test1.data * test2.data));
	}
	SECTION("test arithmetic divide")
	{
		REQUIRE((test1 / test2).data == (test1.data / test2.data));
	}
	SECTION("test arithmetic modulo")
	{
		REQUIRE((test1 % test2).data == (test1.data % test2.data));
	}
}

