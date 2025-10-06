# LRC Lyric Parser

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue?logo=c%2B%2B&logoColor=white&style=flat-square)](https://en.cppreference.com/w/cpp/17)
[![CMake 3.20+](https://img.shields.io/badge/CMake-3.20%2B-blue?logo=cmake&style=flat-square)](https://cmake.org/)
[![Tests 100% Passed](https://img.shields.io/badge/tests-100%25%20passing-brightgreen?style=flat-square&logo=checkmarx)]()
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square)](LICENSE)

## [简体中文](./README.zh-CN.md)

## Introduction

> - `LRC Lyric Parser` is a C++17 library for parsing LRC lyric files. It supports both the **standard LRC format** and the **[enhanced LRC format](https://en.wikipedia.org/wiki/LRC_(file_format))**.
> - This project includes functionality to convert files from **GBK encoding** to **UTF-8 encoding**.

## Features
- Supports both **standard LRC** and **enhanced LRC** lyric formats
- Integrates directly into your project as a **CMake target**
- Cross-platform support for **Windows 11** and **Linux**

### Dependencies
- Standard **C++17**

### Building
- Tests: `-DBUILD_TESTS=ON`
- Examples: `-DBUILD_EXAMPLES=ON`

#### Build only the `lyric-parser` library (default behavior)
```sh
mkdir build && cd build
cmake ..
cmake --build .
```

#### Build all targets, including tests and examples
```sh
mkdir build && cd build
cmake -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON ..
cmake --build .
```

## Usage Example
Add this project as a subdirectory in your project's `CMakeLists.txt` using `add_subdirectory()`:

```cpp
// main.cpp
#include <lyricparser.h>
#include <string>

int main()
{
    using namespace Badfish;

    // file with `GBK` encoding
    const std::string file_1{"../../examples/ChengQuan - LiuRuoYing.lrc"};

    // file with `UTF-8` encoding
    const std::string file_2{"../../examples/QingMingYuShang - XuSong.lrc"};

    AudioToolkit::LyricParser parser(file_1);
    parser.change_encoding(FileKits::Encoding::GBK); // file_1 is `GBK` encoded
    parser.print_info();

    parser.load_file(file_2); // file_2 is `UTF-8` encoded
    parser.print_info();
}
```

> Note:
> - `成全 - 刘若英.lrc` is encoded in **GBK**
> - `清明雨上-许嵩.lrc` is encoded in **UTF-8**
> - The paths for `file_1` and `file_2` assume the files reside in the project root directory, while the executable is located under `build/examples/`

- Run examples
```sh
cd build/examples
./lyric_parser_examples
```

## Testing
The test program creates temporary LRC files, writes content in a specified encoding, then parses them using the corresponding encoding to verify consistency.

### Using the `ScopedFile` class to create temporary files
Each string in `file_content` is automatically appended with a newline character when written:
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
When exiting the scope, the temporary test file is automatically deleted upon destruction of the `ScopedFile` object.

## Todo
- [ ] Support unformatted but complete LRC files containing valid timestamps (e.g., files without line breaks)
- [ ] Fully support parsing of enhanced LRC format, including per-character timestamps within a line

## License
MIT
