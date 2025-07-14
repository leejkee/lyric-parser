# LRC Lyric Parser

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue?logo=c%2B%2B&logoColor=white&style=flat-square)](https://en.cppreference.com/w/cpp/17)
[![CMake 3.20+](https://img.shields.io/badge/CMake-3.20%2B-blue?logo=cmake&style=flat-square)](https://cmake.org/)
[![Tests 100% Passed](https://img.shields.io/badge/tests-100%25%20passing-brightgreen?style=flat-square&logo=checkmarx)]()
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square)](LICENSE)

## 简介
LRC Lyric Parser 是一个用 C++17 编写的 LRC 歌词文件解析库。支持`常见LRC格式`和[`增强型LRC`](https://en.wikipedia.org/wiki/LRC_(file_format))

## 特性
- 支持`标准LRC`歌词格式和`增强型LRC`歌词格式
- 可作为 `CMake target` 直接集成到你的项目中
- 已在 Windows 11 和 Linux 上通过测试
- 单元测试覆盖

## 安装

### 依赖
- 标准C++17

### 构建
```sh
# 使用 CMake 构建
mkdir build && cd build
cmake ..
cmake --build .
```

## 使用示例
```cpp
#include "lrc_parser.h"

LrcParser parser;
parser.loadFromFile("example.lrc");
for (const auto& line : parser.getLyrics()) {
    std::cout << line.time << " " << line.text << std::endl;
}
```

## 测试
```sh
# 在 build 目录下运行
ctest
```

## License
MIT


