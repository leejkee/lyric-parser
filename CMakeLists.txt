cmake_minimum_required(VERSION 3.20)
project(LRC_Parser LANGUAGES CXX)

option(BUILD_TESTS "Build tests" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)

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