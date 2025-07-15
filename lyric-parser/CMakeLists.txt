add_library(lyric_parser STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/lyricparser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/scopedfile.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/textfilehelper.cpp
)

target_compile_features(lyric_parser PUBLIC cxx_std_17)
target_include_directories(lyric_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)