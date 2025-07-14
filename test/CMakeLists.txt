message(STATUS "Test Module LyricParser: ON")

## TestLyricParser
add_executable(TestLyricParser
        TestLyricParser.cpp
)
target_link_libraries(TestLyricParser PRIVATE catch2 lyric_parser)
add_test(NAME TestLyricParser COMMAND TestLyricParser)
## TestLyricParser