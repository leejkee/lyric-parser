message(STATUS "Test Module LyricParser: ON")

## TestLyricParser
add_executable(TestLyricParser
        scopedfile.cpp
        TestLyricParser.cpp
)
target_link_libraries(TestLyricParser PRIVATE lyric_parser)
add_test(NAME TestLyricParser COMMAND TestLyricParser)
## TestLyricParser