# Simple-Utility C++20 header-only library

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/ffee59527a4d43f09b5aeabbac1eaa52)](https://app.codacy.com/gh/DNKpp/Simple-Utility/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)
[![Codacy Badge](https://app.codacy.com/project/badge/Coverage/ffee59527a4d43f09b5aeabbac1eaa52)](https://app.codacy.com/gh/DNKpp/Simple-Utility/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_coverage)
[![codecov](https://codecov.io/gh/DNKpp/Simple-Utility/branch/master/badge.svg?token=R5ZUSJPZ57)](https://codecov.io/gh/DNKpp/Simple-Utility)
[![Coverage Status](https://coveralls.io/repos/github/DNKpp/Simple-Utility/badge.svg?branch=ci/coverage-report)](https://coveralls.io/github/DNKpp/Simple-Utility?branch=master)

## Author
Dominic Koepke  
Mail: [DNKpp2011@gmail.com](mailto:dnkpp2011@gmail.com)

## License

[BSL-1.0](LICENSE_1_0.txt) (free, open source)

```text
          Copyright Dominic Koepke 2019 - 2023.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          https://www.boost.org/LICENSE_1_0.txt)
```

## Description
This library is merely a collection of useful utilities. It is not ment as an replacement for anything out there, it just couples things nicely into one library which doesn't relate closely to anything else but might be useful
on future projects. All parts are well tested and the documentation can be found either on the gh-pages section. If you run into any kind of bugs or have ideas for
improvements, feel free to add an issue or even compile a pull-request.

### functional
In the last few versions, the functional part of this library grew. This library lets you simply couple together functions, whithout the need to explicitly create a lambda or something like this. As a quick example:

```cpp
namespace fn = sl::functional;

constexpr fn::transform_fn square = [](const int i) { return i * i; };
constexpr fn::transform_fn sum = [](const int lhs, const int rhs) { return lhs + rhs; };

std::vector<int> ints{ 1, 2, 3, 4 };
std::ranges::transform(
	ints,
	std::begin(ints),
	square
	| sum << 42	// this binds 42 as the first param of sum
);
```
Well, this calculates the square of each element, adds 42 on it and writes it back into the vector. As a side note, you may also pipe ``raw functionals`` (e.g. function pointers or plain lambdas) into.
Either the first or the second argument must be of that special type.

Additionally there also exist ``predicate_fn`` objects, which offer even more composition operations.

### nullables & unique_handle
One of the current main features are the ``sl::nullables`` namespace and the ``sl::unique_handle`` type, which benefit from each other. ``sl::unique_handle`` is an optional like class (in fact it wraps currently an ``std::optional``) but
offers only a movable interface and guarantees resetting the internal value after a move happend, and is thus semantically closer at ``std::unique_ptr`` but holds its value still on the stack.

The ``sl::nullables`` namespace offers four simple algorithms:
*   ``and_then``
*   ``or_else``
*   ``value_or``
*	``fwd_value``

which are chainable and can be applied on objects which satisfy the ``nullable`` concept. What's a ``nullable`` you may ask? It's a type which has an explicit invalid state; e.g. ``std::optional``, ``std::unique_ptr`` or the ``sl::unique_handle``.
Feel free to have a look into the docs pages.

### type-list
The ``type-list`` module offers several algorithms which operate on any template type accepting variadic template arguments. It is explicitly designed to work
with any such ``type-list`` type interchangingly.

```cpp
namespace tl = sl::type_list;

using MyTypes = tl::TypeList<int, int&>;
using MyTypesCart2 = tl::cartesian_product_t<MyTypes, MyTypes>;

static_assert(std::same_as<
	MyTypesCart2,
	tl::TypeList<
		tl::TypeList<int, int>,
		tl::TypeList<int, int&>,
		tl::TypeList<int&, int>,
		tl::TypeList<int, int&>>>);

using Zipped = tl::zip_as_t<std::tuple, MyTypesCart2, std::tuple<float, double>>;	// note the _as suffix. This is necessary if different type-list templates are combined.

static_assert(std::same_as<
	Zipped,
	std::tuple<
		std::tuple<tl::TypeList<int, int>, float>,
		std::tuple<tl::TypeList<int, int&>, double>>>);
```

## Installation with CMake
This library can easily be integrated into your project via CMake target_link_libraries command.

```cmake
target_link_libraries(
	<your_target_name>
	PUBLIC
	Simple-Utility::Simple-Utility
)
```

### FetchContent
It is possible fetching this library via CMakes FetchContent module.

```cmake
include(FetchContent)

FetchContent_Declare(
	Simple-Utility
	GIT_REPOSITORY	https://github.com/DNKpp/Simple-Utility
	GIT_TAG		"v2.0.0"	# or any other tag name
)

FetchContent_MakeAvailable(Simple-Utility)
# do not forget linking via target_link_libraries as shown above
```

### CPM
The [CPM](https://github.com/cpm-cmake/CPM.cmake) CMake module is a featureful wrapper around the ``FetchContent`` module. To use it simply add ``CPM.cmake`` or ``get_cmp.make`` (which will pull ``CPM.cmake`` on the fly)
from the latest release into your project folder and include it into your ``CMakeLists.txt``. 

```cmake
include(CPM.cmake) # or include(get_cpm.cmake)


CPMAddPackage("gh:DNKpp/Simple-Utility@2.0.0") # or any other tag name
# do not forget linking via target_link_libraries as shown above
```
