# Simple-Utility C++20 header-only library

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/ffee59527a4d43f09b5aeabbac1eaa52)](https://www.codacy.com/gh/DNKpp/Simple-Utility/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=DNKpp/Simple-Utility&amp;utm_campaign=Badge_Grade)
[![Codacy Badge](https://app.codacy.com/project/badge/Coverage/ffee59527a4d43f09b5aeabbac1eaa52)](https://www.codacy.com/gh/DNKpp/Simple-Utility/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=DNKpp/Simple-Utility&amp;utm_campaign=Badge_Coverage)
[![codecov](https://codecov.io/gh/DNKpp/Simple-Utility/branch/master/graph/badge.svg?token=R5ZUSJPZ57)](https://codecov.io/gh/DNKpp/Simple-Utility)
[![Documentation](https://img.shields.io/badge/docs-docsforge-blue)](https://simple-utility.docsforge.com/)

## Author
Dominic Koepke  
Mail: [DNKpp2011@gmail.com](mailto:dnkpp2011@gmail.com)

## License

[BSL-1.0](LICENSE_1_0.txt) (free, open source)

```text
          Copyright Dominic Koepke 2019 - 2022.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          https://www.boost.org/LICENSE_1_0.txt)
```

## Description
This library it merely a collection of useful utilities. It is not ment as an replacement for anything out there, it just couples things nicely into one library which doesn't relate closely to anything else but might be useful
on future projects. All parts are well tested and the documentation can be found either on the gh-pages section or directly at [docsforge.com](https://simple-utility.docsforge.com/). If you run into any kind of bugs or have ideas for
improvements, feel free to add an issue or even compile a pull-request.

## Installation with CMake
This library can easily be integrated into your project via CMake target_link_libraries command.

```cmake
target_link_libraries(
	<your_target_name>
	PRIVATE
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
	GIT_TAG		"master"
)

FetchContent_MakeAvailable(Simple-Utility)
# do not forget linking via target_link_libraries as shown above
```

### CPM
The [CPM](https://github.com/cpm-cmake/CPM.cmake) CMake module is a featureful wrapper around the ``FetchContent`` module. To use it simply add ``CPM.cmake`` or ``get_cmp.make`` (which will pull ``CPM.cmake`` on the fly)
from the latest release into your project folder and include it into your ``CMakeLists.txt``. 

```cmake
include(CPM.cmake) # or include(get_cpm.cmake)


CPMAddPackage("gh:DNKpp/Simple-Utility#master")
# do not forget linking via target_link_libraries as shown above
```
