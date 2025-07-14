cmake_minimum_required(VERSION 3.20)
project(LRC_Parser LANGUAGES CXX)

include(CTest)
enable_testing()

add_subdirectory(lyric-parser)
add_subdirectory(test)
add_subdirectory(catch2)