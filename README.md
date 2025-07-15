# LRC Lyric Parser

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue?logo=c%2B%2B&logoColor=white&style=flat-square)](https://en.cppreference.com/w/cpp/17)
[![CMake 3.20+](https://img.shields.io/badge/CMake-3.20%2B-blue?logo=cmake&style=flat-square)](https://cmake.org/)
[![Tests 100% Passed](https://img.shields.io/badge/tests-100%25%20passing-brightgreen?style=flat-square&logo=checkmarx)]()
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square)](LICENSE)

## [简体中文](./README.zh-CN.md)

## Introduction
LRC Lyric Parser is a C++17 library for parsing standard and enhanced LRC lyric files. It supports both common LRC format and [enhanced LRC](https://en.wikipedia.org/wiki/LRC_(file_format)).

## Features
- Supports standard and enhanced LRC lyric formats
- Can be integrated as a CMake target in your project
- Tested on Windows 11 and Linux
- Unit test coverage

## Installation

### Dependencies
- C++17

### Build

By default, tests and examples are not built. Enable them manually if needed:

- Build tests: `-DBUILD_TESTS=ON`
- Build examples: `-DBUILD_EXAMPLES=ON`

#### Build `lyric-parser` only (default)
```sh
mkdir build && cd build
cmake ..
cmake --build .
```

#### Build with tests and examples
```sh
mkdir build && cd build
cmake -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON ..
cmake --build .
```

## Usage Example
```cpp
#include <lyricparser.h>
#include <string>

int main()
{
    using namespace Badfish;

    // file with GBK encoding
    const std::string file_1{"../../examples/成全 - 刘若英.lrc"};

    // file with UTF8 encoding
    const std::string file_2{"../../examples/清明雨上-许嵩.lrc"};

    AudioToolkit::LyricParser parser(file_1);
    parser.change_encoding(FileKits::Encoding::GBK); // file_1 is GBK encoded
    parser.print_info();

    parser.load_file(file_2); // file_2 is UTF-8 encoded
    parser.print_info();
}
```

> Example notes:
> - `成全 - 刘若英.lrc` is GBK encoded
> - `清明雨上-许嵩.lrc` is UTF-8 encoded

## Testing

The test program creates temporary LRC files, writes them in the specified encoding, and parses them to verify correctness.

### Using ScopedFile to create temporary files
Each string in `file_content` will be written with a newline at the end.
```cpp
{
    std::string filename {"filename.lrc"};
    std::vector<std::string> file_content{
        "",
        "",
        ""
    };
    ScopedFile file_helper(filename);
    file_helper.write_to_file(file_content, ScopedFile::Encoding::UTF8);
}
```
When the scope ends, the `file_helper` object will delete the temporary file in its destructor.

## License
MIT
