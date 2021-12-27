//          Copyright Dominic Koepke 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch.hpp>

#include "Simple-Utility/concepts/operators.hpp"

using namespace sl::concepts;

namespace
{
	enum shift
	{
		none = 0,
		left = 1 << 1,
		right = 1 << 2,
		bidirectional = left | right,

		left_assign = 1 << 3,
		right_assign = 1 << 4,
		bidirectional_assign = left_assign | right_assign,

		fully = bidirectional | bidirectional_assign
	};

	template <int VShifts = none>
	struct shift_target
	{
		shift_target() = default;

		explicit shift_target(int)
		{}

		shift_target operator <<(int) const
			requires ((VShifts & left) != 0)
		{
			return shift_target{};
		}

		shift_target operator >>(int) const
			requires ((VShifts & right) != 0)
		{
			return shift_target{};
		}

		shift_target& operator <<=(int)
			requires ((VShifts & left_assign) != 0)
		{
			return *this;
		}

		shift_target& operator >>=(int)
			requires ((VShifts & right_assign) != 0)
		{
			return *this;
		}
	};

	struct no_shift
	{
	};

	template <class TLhs, class TRhs, bool VResult>
	struct right_shiftable_with_testable
	{
		static constexpr bool value{ right_shiftable_with<TLhs, TRhs> == VResult };
	};

	template <class TLhs, class TRhs, bool VResult, class... TArgs>
	struct right_shiftable_with_r_testable
	{
		static constexpr bool value{ right_shiftable_with_r<TLhs, TRhs, TArgs...> == VResult };
	};

	template <class T, bool VResult>
	struct right_shiftable_testable
	{
		static constexpr bool value{ right_shiftable<T> == VResult };
	};

	template <class T, bool VResult, class... TArgs>
	struct right_shiftable_r_testable
	{
		static constexpr bool value{ right_shiftable_r<T, TArgs...> == VResult };
	};

	template <class TLhs, class TRhs, bool VResult>
	struct left_shiftable_with_testable
	{
		static constexpr bool value{ left_shiftable_with<TLhs, TRhs> == VResult };
	};

	template <class TLhs, class TRhs, bool VResult, class... TArgs>
	struct left_shiftable_with_r_testable
	{
		static constexpr bool value{ left_shiftable_with_r<TLhs, TRhs, TArgs...> == VResult };
	};

	template <class T, bool VResult>
	struct left_shiftable_testable
	{
		static constexpr bool value{ left_shiftable<T> == VResult };
	};

	template <class T, bool VResult, class... TArgs>
	struct left_shiftable_r_testable
	{
		static constexpr bool value{ left_shiftable_r<T, TArgs...> == VResult };
	};

	template <class TLhs, class TRhs, bool VResult>
	struct bidirectional_shiftable_with_testable
	{
		static constexpr bool value{ bidirectional_shiftable_with<TLhs, TRhs> == VResult };
	};

	template <class TLhs, class TRhs, bool VResult, class... TArgs>
	struct bidirectional_shiftable_with_r_testable
	{
		static constexpr bool value{ bidirectional_shiftable_with_r<TLhs, TRhs, TArgs...> == VResult };
	};

	template <class T, bool VResult>
	struct bidirectional_shiftable_testable
	{
		static constexpr bool value{ bidirectional_shiftable<T> == VResult };
	};

	template <class T, bool VResult, class... TArgs>
	struct bidirectional_shiftable_r_testable
	{
		static constexpr bool value{ bidirectional_shiftable_r<T, TArgs...> == VResult };
	};

	template <class TLhs, class TRhs, bool VResult>
	struct right_shift_assignable_with_testable
	{
		static constexpr bool value{ right_shift_assignable_with<TLhs, TRhs> == VResult };
	};

	template <class TLhs, class TRhs, bool VResult, class... TArgs>
	struct right_shift_assignable_with_r_testable
	{
		static constexpr bool value{ right_shift_assignable_with_r<TLhs, TRhs, TArgs...> == VResult };
	};

	template <class T, bool VResult>
	struct right_shift_assignable_testable
	{
		static constexpr bool value{ right_shift_assignable<T> == VResult };
	};

	template <class T, bool VResult, class... TArgs>
	struct right_shift_assignable_r_testable
	{
		static constexpr bool value{ right_shift_assignable_r<T, TArgs...> == VResult };
	};

	template <class TLhs, class TRhs, bool VResult>
	struct left_shift_assignable_with_testable
	{
		static constexpr bool value{ left_shift_assignable_with<TLhs, TRhs> == VResult };
	};

	template <class TLhs, class TRhs, bool VResult, class... TArgs>
	struct left_shift_assignable_with_r_testable
	{
		static constexpr bool value{ left_shift_assignable_with_r<TLhs, TRhs, TArgs...> == VResult };
	};

	template <class T, bool VResult>
	struct left_shift_assignable_testable
	{
		static constexpr bool value{ left_shift_assignable<T> == VResult };
	};

	template <class T, bool VResult, class... TArgs>
	struct left_shift_assignable_r_testable
	{
		static constexpr bool value{ left_shift_assignable_r<T, TArgs...> == VResult };
	};

	template <class TLhs, class TRhs, bool VResult>
	struct bidirectional_shift_assignable_with_testable
	{
		static constexpr bool value{ bidirectional_shift_assignable_with<TLhs, TRhs> == VResult };
	};

	template <class TLhs, class TRhs, bool VResult, class... TArgs>
	struct bidirectional_shift_assignable_with_r_testable
	{
		static constexpr bool value{ bidirectional_shift_assignable_with_r<TLhs, TRhs, TArgs...> == VResult };
	};

	template <class T, bool VResult>
	struct bidirectional_shift_assignable_testable
	{
		static constexpr bool value{ bidirectional_shift_assignable<T> == VResult };
	};

	template <class T, bool VResult, class... TArgs>
	struct bidirectional_shift_assignable_r_testable
	{
		static constexpr bool value{ bidirectional_shift_assignable_r<T, TArgs...> == VResult };
	};

	template <class TLhs, class TRhs, bool VResult>
	struct fully_shiftable_with_testable
	{
		static constexpr bool value{ fully_shiftable_with<TLhs, TRhs> == VResult };
	};

	template <class TLhs, class TRhs, bool VResult, class... TArgs>
	struct fully_shiftable_with_r_testable
	{
		static constexpr bool value{ fully_shiftable_with_r<TLhs, TRhs, TArgs...> == VResult };
	};

	template <class T, bool VResult>
	struct fully_shiftable_testable
	{
		static constexpr bool value{ fully_shiftable<T> == VResult };
	};

	template <class T, bool VResult, class... TArgs>
	struct fully_shiftable_r_testable
	{
		static constexpr bool value{ fully_shiftable_r<T, TArgs...> == VResult };
	};
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"right_shiftable_with(_r) should determine if two types can be used within an operator >> expression.",
	"[concepts][operators][shift]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(right_shiftable_with_testable, right_shiftable_with_r_testable),
	(
		(int, int, true),
		(float, int, false),
		(int, float, false),
		(shift_target<right>, int, true),
		(const shift_target<right>, int, true),
		(shift_target<right>, no_shift, false),
		(shift_target<>, int, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"right_shiftable(_r) should determine if a type can be used within an operator >> expression.",
	"[concepts][operators][shift]",
	((class T, bool VExpected), T, VExpected),
	(right_shiftable_testable, right_shiftable_r_testable),
	(
		(int, true),
		(float, false),
		(shift_target<right>, false),
		(const shift_target<right>, false),
		(shift_target<>, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"left_shiftable_with(_r) should determine if two types can be used within an operator << expression.",
	"[concepts][operators][shift]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(left_shiftable_with_testable, left_shiftable_with_r_testable),
	(
		(int, int, true),
		(float, int, false),
		(int, float, false),
		(shift_target<left>, int, true),
		(const shift_target<left>, int, true),
		(shift_target<left>, no_shift, false),
		(shift_target<>, int, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"left_shiftable(_r) should determine if a type can be used within an operator << expression.",
	"[concepts][operators][shift]",
	((class T, bool VExpected), T, VExpected),
	(left_shiftable_testable, left_shiftable_r_testable),
	(
		(int, true),
		(float, false),
		(shift_target<left>, false),
		(const shift_target<left>, false),
		(shift_target<>, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"bidirectional_shiftable_with(_r) should determine if two types can be used within operator << and operator >> expressions.",
	"[concepts][operators][shift]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(bidirectional_shiftable_with_testable, bidirectional_shiftable_with_r_testable),
	(
		(int, int, true),
		(float, int, false),
		(int, float, false),
		(shift_target<bidirectional>, int, true),
		(const shift_target<bidirectional>, int, true),
		(shift_target<left>, int, false),
		(shift_target<right>, int, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"bidirectional_shiftable(_r) should determine if a type can be used within operator << and operator >> expressions.",
	"[concepts][operators][shift]",
	((class T, bool VExpected), T, VExpected),
	(bidirectional_shiftable_testable, bidirectional_shiftable_r_testable),
	(
		(int, true),
		(float, false),
		(shift_target<bidirectional>, false),
		(const shift_target<bidirectional>, false),
		(shift_target<left>, false),
		(shift_target<right>, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"right_shift_assignable_with(_r) should determine if two types can be used within an operator >>= expression.",
	"[concepts][operators][shift]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(right_shift_assignable_with_testable, right_shift_assignable_with_r_testable),
	(
		(int, int, true),
		(float, int, false),
		(int, float, false),
		(shift_target<right_assign>, int, true),
		(const shift_target<right_assign>, int, false),
		(shift_target<right_assign>, no_shift, false),
		(shift_target<>, int, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"right_shift_assignable(_r) should determine if a type can be used within an operator >>= expression.",
	"[concepts][operators][shift]",
	((class T, bool VExpected), T, VExpected),
	(right_shift_assignable_testable, right_shift_assignable_r_testable),
	(
		(int, true),
		(float, false),
		(shift_target<right_assign>, false),
		(const shift_target<right_assign>, false),
		(shift_target<>, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"left_shift_assignable_with(_r) should determine if two types can be used within an operator <<= expression.",
	"[concepts][operators][shift]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(left_shift_assignable_with_testable, left_shift_assignable_with_r_testable),
	(
		(int, int, true),
		(float, int, false),
		(int, float, false),
		(shift_target<left_assign>, int, true),
		(const shift_target<left_assign>, int, false),
		(shift_target<left_assign>, no_shift, false),
		(shift_target<>, int, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"left_shift_assignable(_r) should determine if a type can be used within an operator <<= expression.",
	"[concepts][operators][shift]",
	((class T, bool VExpected), T, VExpected),
	(left_shift_assignable_testable, left_shift_assignable_r_testable),
	(
		(int, true),
		(float, false),
		(shift_target<right_assign>, false),
		(const shift_target<right_assign>, false),
		(shift_target<>, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"bidirectional_shift_assignable_with(_r) should determine if two types can be used within operator <<= and operator >>= expressions.",
	"[concepts][operators][shift]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(bidirectional_shift_assignable_with_testable, bidirectional_shift_assignable_with_r_testable),
	(
		(int, int, true),
		(float, int, false),
		(int, float, false),
		(shift_target<bidirectional_assign>, int, true),
		(const shift_target<bidirectional_assign>, int, false),
		(shift_target<left_assign>, int, false),
		(shift_target<right_assign>, int, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"bidirectional_shift_assignable(_r) should determine if a type can be used within operator <<= and operator =>> expressions.",
	"[concepts][operators][shift]",
	((class T, bool VExpected), T, VExpected),
	(bidirectional_shift_assignable_testable, bidirectional_shift_assignable_r_testable),
	(
		(int, true),
		(float, false),
		(shift_target<bidirectional_assign>, false),
		(const shift_target<bidirectional_assign>, false),
		(shift_target<left_assign>, false),
		(shift_target<right_assign>, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"fully_shiftable_with(_r) should determine if two types can be used within all shift operator expressions.",
	"[concepts][operators][shift]",
	((class TLhs, class TRhs, bool VExpected), TLhs, TRhs, VExpected),
	(fully_shiftable_with_testable, fully_shiftable_with_r_testable),
	(
		(int, int, true),
		(float, int, false),
		(int, float, false),
		(shift_target<fully>, int, true),
		(const shift_target<fully>, int, false),
		(shift_target<fully ^ right>, int, false),
		(shift_target<fully ^ left>, int, false),
		(shift_target<fully ^ right_assign>, int, false),
		(shift_target<fully ^ left_assign>, int, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"fully_shiftable(_r) should determine if a type can be used within all shift operator expressions.",
	"[concepts][operators][shift]",
	((class T, bool VExpected), T, VExpected),
	(fully_shiftable_testable, fully_shiftable_r_testable),
	(
		(int, true),
		(float, false),
		(shift_target<fully>, false),
		(const shift_target<fully>, false),
		(shift_target<fully ^ right>, false),
		(shift_target<fully ^ left>, false),
		(shift_target<fully ^ right_assign>, false),
		(shift_target<fully ^ left_assign>, false)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"(right|left|bidirectional)_shiftable_with_r should determine if return type satisfies expectations.",
	"[concepts][operators][shift]",
	((class TLhs, class TRhs, bool VExpected, class TReturn), TLhs, TRhs, VExpected, TReturn),
	(right_shiftable_with_r_testable, left_shiftable_with_r_testable, bidirectional_shiftable_with_r_testable),
	(
		(int, int, true, int),
		(int, int, true, const int),
		(int, int, true, int&&),
		(int, int, true, const int&),
		(int, int, false, int&),
		(int, int, false, shift_target<>),	// explicit ctor
		(shift_target<fully>, int, true, shift_target<fully>),
		(shift_target<fully>, int, true, const shift_target<fully>),
		(shift_target<fully>, int, true, shift_target<fully>&&),
		(shift_target<fully>, int, true, const shift_target<fully>&),
		(shift_target<fully>, int, false, int),
		(shift_target<fully>, int, false, shift_target<fully>&)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"(right|left|bidirectional)_shift_r should determine if return type satisfies expectations.",
	"[concepts][operators][shift]",
	((class T, bool VExpected, class TReturn), T, VExpected, TReturn),
	(right_shiftable_r_testable, left_shiftable_r_testable, bidirectional_shiftable_r_testable),
	(
		(int, true, int),
		(int, true, const int),
		(int, true, int&&),
		(int, true, const int&),
		(int, false, int&),
		(int, false, shift_target<>)	// explicit ctor
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"(right|left|bidirectional)_shift_assignable_with_r should determine if return type satisfies expectations.",
	"[concepts][operators][shift]",
	((class TLhs, class TRhs, bool VExpected, class TReturn), TLhs, TRhs, VExpected, TReturn),
	(right_shift_assignable_with_r_testable, left_shift_assignable_with_r_testable, bidirectional_shift_assignable_with_r_testable),
	(
		(int, int, true, int),
		(int, int, true, const int),
		(int, int, true, int&),
		(int, int, true, const int&),
		(int, int, false, int&&),
		(int, int, false, no_shift),
		(int, int, false, shift_target<>),	// explicit ctor
		(shift_target<fully>, int, true, shift_target<fully>),
		(shift_target<fully>, int, true, const shift_target<fully>),
		(shift_target<fully>, int, false, int),
		(shift_target<fully>, int, true, shift_target<fully>&),
		(shift_target<fully>, int, true, const shift_target<fully>&),
		(shift_target<fully>, int, false, shift_target<fully>&&),
		(shift_target<fully>, int, false, no_shift)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}

#pragma warning(disable: 26444)
TEMPLATE_PRODUCT_TEST_CASE_SIG
(
	"(right|left|bidirectional)_shift_assignable_r should determine if return type satisfies expectations.",
	"[concepts][operators][shift]",
	((class T, bool VExpected, class TReturn), T, VExpected, TReturn),
	(right_shift_assignable_r_testable, left_shift_assignable_r_testable, bidirectional_shift_assignable_r_testable),
	(
		(int, true, int),
		(int, true, const int),
		(int, true, int&),
		(int, true, const int&),
		(int, false, int&&),
		(int, false, shift_target<>),	// explicit ctor
		(int, false, no_shift)
	)
)
#pragma warning(default: 26444)
{
	REQUIRE(TestType::value);
}
