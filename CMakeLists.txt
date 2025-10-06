cmake_minimum_required(VERSION 3.20)
project(LRC_Parser LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)

add_subdirectory(lyric-parser)

if(BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
    add_subdirectory(catch2)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()