
//          Copyright Dominic Koepke 2019 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <numeric>
#include <array>

#include "catch.hpp"
#include "Simple-Utility/container/Vector2d.hpp"

TEST_CASE("CTor, assign and iterator", "[Vector2d]")
{
	using namespace sl::container;

	SECTION("Default CTor")
	{
		Vector2d<int> vec;

		REQUIRE(std::empty(vec));
		REQUIRE(vec.height() == 0);
		REQUIRE(vec.width() == 0);
		REQUIRE(vec.cellCount() == 0);
	}

	SECTION("size and value CTor")
	{
		Vector2d<int> vec{ 10, 5 };

		REQUIRE_FALSE(std::empty(vec));
		REQUIRE(vec.height() == 5);
		REQUIRE(vec.width() == 10);
		REQUIRE(vec.cellCount() == 5 * 10);

		REQUIRE(std::begin(vec) == std::cbegin(vec));
		REQUIRE(std::begin(std::as_const(vec)) == std::cbegin(vec));
		REQUIRE(std::end(vec) == std::cend(vec));
		REQUIRE(std::end(std::as_const(vec)) == std::cend(vec));
		
		REQUIRE(std::rbegin(vec) == std::crbegin(vec));
		REQUIRE(std::rbegin(std::as_const(vec)) == std::crbegin(vec));
		REQUIRE(std::rend(vec) == std::crend(vec));
		REQUIRE(std::rend(std::as_const(vec)) == std::crend(vec));
		REQUIRE(std::end(vec) == std::rbegin(vec).base());
		REQUIRE(std::end(vec) == std::rbegin(std::as_const(vec)).base());
		REQUIRE(std::begin(vec) == std::rend(vec).base());
		REQUIRE(std::begin(vec) == std::rend(std::as_const(vec)).base());
	}

	SECTION("copy")
	{
		Vector2d<int> vec{ 10, 5 };
		auto vec2 = vec;
		
		REQUIRE_FALSE(std::empty(vec));
		REQUIRE(vec.height() == 5);
		REQUIRE(vec.width() == 10);
		REQUIRE_FALSE(std::empty(vec2));
		REQUIRE(vec2.height() == 5);
		REQUIRE(vec2.width() == 10);
		REQUIRE(vec2 == vec);

		*std::begin(vec) = 5;
		REQUIRE_FALSE(vec2 == vec);

		vec2 = vec;
		REQUIRE(vec2 == vec);
	}

	SECTION("move")
	{
		Vector2d<int> vec{ 10, 5 };
		auto vec2 = std::move(vec);
		
		REQUIRE(std::empty(vec));
		REQUIRE(vec.height() == 0);
		REQUIRE(vec.width() == 0);
		REQUIRE(vec.cellCount() == 0);
		
		REQUIRE_FALSE(std::empty(vec2));
		REQUIRE(vec2.height() == 5);
		REQUIRE(vec2.width() == 10);
		REQUIRE(vec2.cellCount() == 5 * 10);
		REQUIRE_FALSE(vec2 == vec);

		vec = std::move(vec2);
		REQUIRE(std::empty(vec2));
		REQUIRE(vec2.height() == 0);
		REQUIRE(vec2.width() == 0);
		REQUIRE(vec2.cellCount() == 0);
		
		REQUIRE_FALSE(std::empty(vec));
		REQUIRE(vec.height() == 5);
		REQUIRE(vec.width() == 10);
		REQUIRE(vec.cellCount() == 5 * 10);
		REQUIRE_FALSE(vec2 == vec);
	}
}

TEST_CASE("functions", "[Vector2d]")
{
	using namespace sl::container;

	SECTION("isInRange on empty")
	{
		Vector2d<int> vec;

		REQUIRE_FALSE(vec.isInRange(0, 0));
	}

	SECTION("isInRange")
	{
		Vector2d<int> vec{ 8, 4 };

		REQUIRE(vec.isInRange(0, 0));
		REQUIRE(vec.isInRange(7, 3));
		REQUIRE_FALSE(vec.isInRange(7, 4));
		REQUIRE_FALSE(vec.isInRange(8, 3));
		REQUIRE_FALSE(vec.isInRange(8, 4));
	}

	SECTION("clear")
	{
		Vector2d<int> vec{ 8, 4 };
		vec.clear();

		REQUIRE(vec == Vector2d<int>{});
	}

	SECTION("rawData")
	{
		Vector2d<int> vec{ 8, 4 };
		REQUIRE_FALSE(vec.rawData() == nullptr);
		
		*std::begin(vec) = 5;
		REQUIRE(*vec.rawData() == 5);
		REQUIRE(*std::as_const(vec).rawData() == 5);
	}

	SECTION("element access")
	{
		Vector2d<int> vec{ 4, 4 };
		std::iota(std::begin(vec), std::end(vec), 0);

		int i = 0;
		for (std::size_t c = 0; c < vec.width(); ++c)
		{
			for (std::size_t r = 0; r < vec.height(); ++r)
			{
				auto val = vec.at(c, r);
				REQUIRE(val == i++);
				REQUIRE(val == std::as_const(vec.at(c, r)));
			}
		}
	}

	SECTION("SubView")
	{
		Vector2d<int> vec{ 4, 4 };
		std::iota(std::begin(vec), std::end(vec), 0);

		int i = 0;
		for (std::size_t c = 0; c < vec.width(); ++c)
		{
			for (auto view = std::as_const(vec)[c]; auto val : view)
			{
				REQUIRE(val == i++);
			}
		}

		REQUIRE(std::as_const(vec)[0].data() == vec[0].data());
	}

	SECTION("resize")
	{
		Vector2d<int> vec{ 4, 4 };
		std::iota(std::begin(vec), std::end(vec), 0);

		SECTION("set width zero")
		{
			vec.setWidth(0);
			REQUIRE(vec.width() == 0);
			REQUIRE(vec.cellCount() == 0);
			REQUIRE(std::empty(vec));
		}

		SECTION("set height zero")
		{
			vec.setHeight(0);
			REQUIRE(vec.height() == 0);
			REQUIRE(vec.cellCount() == 0);
			REQUIRE(std::empty(vec));
		}

		SECTION("increase width")
		{
			vec.setWidth(15, 99);
			REQUIRE(vec.width() == 15);
			REQUIRE(vec.cellCount() == 60);
			REQUIRE_FALSE(std::empty(vec));

			std::array arr{
				0, 1, 2, 3,
				4, 5, 6, 7,
				8, 9, 10, 11,
				12, 13, 14, 15,
				99, 99, 99, 99,
				99, 99, 99, 99,
				99, 99, 99, 99,
				99, 99, 99, 99,
				99, 99, 99, 99,
				99, 99, 99, 99,
				99, 99, 99, 99,
				99, 99, 99, 99,
				99, 99, 99, 99,
				99, 99, 99, 99,
				99, 99, 99, 99
			};

			REQUIRE(std::equal(std::begin(vec), std::end(vec), std::begin(arr)));
		}

		SECTION("decrease width")
		{
			vec.setWidth(3);
			REQUIRE(vec.width() == 3);
			REQUIRE(vec.cellCount() == 12);
			REQUIRE_FALSE(std::empty(vec));

			std::array arr{
				0, 1, 2, 3,
				4, 5, 6, 7,
				8, 9, 10, 11
			};

			REQUIRE(std::equal(std::begin(vec), std::end(vec), std::begin(arr)));
		}

		SECTION("increase height")
		{
			vec.setHeight(15, 99);
			REQUIRE(vec.height() == 15);
			REQUIRE(vec.cellCount() == 60);
			REQUIRE_FALSE(std::empty(vec));

			std::array arr{
				0, 1, 2, 3, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
				4, 5, 6, 7, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
				8, 9, 10, 11, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
				12, 13, 14, 15, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99
			};

			REQUIRE(std::equal(std::begin(vec), std::end(vec), std::begin(arr)));
		}

		SECTION("decrease height")
		{
			vec.setHeight(3);
			REQUIRE(vec.height() == 3);
			REQUIRE(vec.cellCount() == 12);
			REQUIRE_FALSE(std::empty(vec));

			std::array arr{
				0, 1, 2,
				4, 5, 6,
				8, 9, 10,
				12, 13, 14
			};

			REQUIRE(std::equal(std::begin(vec), std::end(vec), std::begin(arr)));
		}

		SECTION("decrease height & width")
		{
			vec.resize(3, 3);
			REQUIRE(vec.height() == 3);
			REQUIRE(vec.width() == 3);
			REQUIRE(vec.cellCount() == 9);
			REQUIRE_FALSE(std::empty(vec));

			std::array arr{
				0, 1, 2,
				4, 5, 6,
				8, 9, 10
			};

			REQUIRE(std::equal(std::begin(vec), std::end(vec), std::begin(arr)));
		}

		SECTION("increase height & width")
		{
			vec.resize(9, 9, 99);
			REQUIRE(vec.height() == 9);
			REQUIRE(vec.width() == 9);
			REQUIRE(vec.cellCount() == 81);
			REQUIRE_FALSE(std::empty(vec));

			std::array arr{
				0, 1, 2, 3, 99, 99, 99, 99, 99,
				4, 5, 6, 7, 99, 99, 99, 99, 99,
				8, 9, 10, 11, 99, 99, 99, 99, 99,
				12, 13, 14, 15, 99, 99, 99, 99, 99,
				99, 99, 99, 99, 99, 99, 99, 99, 99,
				99, 99, 99, 99, 99, 99, 99, 99, 99,
				99, 99, 99, 99, 99, 99, 99, 99, 99,
				99, 99, 99, 99, 99, 99, 99, 99, 99,
				99, 99, 99, 99, 99, 99, 99, 99, 99
			};

			REQUIRE(std::equal(std::begin(vec), std::end(vec), std::begin(arr)));
		}

		SECTION("increase height, decrease width")
		{
			vec.resize(2, 9, 99);
			REQUIRE(vec.height() == 9);
			REQUIRE(vec.width() == 2);
			REQUIRE(vec.cellCount() == 18);
			REQUIRE_FALSE(std::empty(vec));

			std::array arr{
				0, 1, 2, 3, 99, 99, 99, 99, 99,
				4, 5, 6, 7, 99, 99, 99, 99, 99
			};

			REQUIRE(std::equal(std::begin(vec), std::end(vec), std::begin(arr)));
		}

		SECTION("decrease height, increase width")
		{
			vec.resize(9, 2, 99);
			REQUIRE(vec.height() == 2);
			REQUIRE(vec.width() == 9);
			REQUIRE(vec.cellCount() == 18);
			REQUIRE_FALSE(std::empty(vec));

			std::array arr{
				0, 1,
				4, 5,
				8, 9,
				12, 13,
				99, 99,
				99, 99,
				99, 99,
				99, 99,
				99, 99
			};

			REQUIRE(std::equal(std::begin(vec), std::end(vec), std::begin(arr)));
		}
	}
}
