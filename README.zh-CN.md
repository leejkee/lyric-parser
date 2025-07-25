# LRC Lyric Parser

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue?logo=c%2B%2B&logoColor=white&style=flat-square)](https://en.cppreference.com/w/cpp/17)
[![CMake 3.20+](https://img.shields.io/badge/CMake-3.20%2B-blue?logo=cmake&style=flat-square)](https://cmake.org/)
[![Tests 100% Passed](https://img.shields.io/badge/tests-100%25%20passing-brightgreen?style=flat-square&logo=checkmarx)]()
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square)](LICENSE)

## 简介
`LRC Lyric Parser` 是一个用 `C++17` 编写的 LRC 歌词文件解析库。支持 `标准LRC格式` 和 [`增强型LRC`](https://en.wikipedia.org/wiki/LRC_(file_format))。

## 特性
- 支持 `标准LRC` 歌词格式和 `增强型LRC` 歌词格式
- 可作为 `CMake target` 直接集成到你的项目中
- 已在 `Windows 11` 和 `Linux` 上通过测试
- 单元测试覆盖

## 安装

### 依赖
- `标准C++17`

### 构建


本项目默认不编译 `test` 和 `examples`，如需编译请手动开启：

- 编译测试：`-DBUILD_TESTS=ON`
- 编译示例：`-DBUILD_EXAMPLES=ON`

#### 只编译 `lyric-parser`（默认行为）
```sh
mkdir build && cd build
cmake ..
cmake --build .
```

#### 编译全部target，包含测试和示例
```sh
mkdir build && cd build
cmake -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON ..
cmake --build .
```

## 使用示例
在你的项目 `CMakeLists.txt` 中以子目录形式添加本项目，使用 `add_subdirectory()` 即可：
```cpp
#include <lyricparser.h>
#include <string>

int main()
{
    using namespace Badfish;

    // file with `GBK` encoding
    const std::string file_1{"../../examples/成全 - 刘若英.lrc"};

    // file with `UTF-8` encoding
    const std::string file_2{"../../examples/清明雨上-许嵩.lrc"};

    AudioToolkit::LyricParser parser(file_1);
    parser.change_encoding(FileKits::Encoding::GBK); // file_1 is `GBK` encoded
    parser.print_info();

    parser.load_file(file_2); // file_2 is `UTF-8` encoded
    parser.print_info();
}
```

> 示例说明：
> - `成全 - 刘若英.lrc` 文件编码为 `GBK`
> - `清明雨上-许嵩.lrc` 文件编码为 `UTF-8`

## 测试
测试程序创建临时 `LRC` 文件，以指定编码写入，再以对应编码解析，测试一致性。

### 使用 `ScopedFile` 类创建临时文件
`file_content` 中每一个字符串被写入时，会自动在末尾添加换行符：
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
当退出作用域时，`file_helper` 对象会调用析构函数来删除临时文件。

## License
MIT
