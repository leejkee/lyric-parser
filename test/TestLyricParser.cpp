//
// Created by 31305 on 2025/6/24.
//
#define CATCH_CONFIG_MAIN
#include "scopedfile.h"
#include <lyricparser.h>
#include <catch.hpp>

TEST_CASE("LyricParserNormalTest", "Normal-LRC Test")
{
    const std::string filename{"test.lyc"};
    const std::vector<std::string> normal_lrc_toT{
        " [t1: Test tag] "
        , " [标签: 测试标签] "
        , " [00:00.001] Test lyric"
        , " [00:01.010] 歌词测试"
    };

    const std::vector<std::string> expected_tags{
        "t1: Test tag"
        , "标签: 测试标签"
    };

    const std::vector<Badfish::AudioToolkit::LyricLine> expected_content_lines{
        {1, "Test lyric"}
        , {1010, "歌词测试"}
    };

    SECTION("Normal-LRC Test, Encode: UTF8")
    {
        ScopedFile fileHelper(filename);
        fileHelper.write_to_file(normal_lrc_toT, ScopedFile::Encoding::UTF8);
        const Badfish::AudioToolkit::LyricParser lyric_parser{filename};
        REQUIRE(lyric_parser.is_enhanced() == false);
        REQUIRE(lyric_parser.get_tags() == expected_tags);
        REQUIRE(lyric_parser.get_text() == expected_content_lines);
    }

    SECTION("Normal-LRC Test, Encode: GBK")
    {
        ScopedFile fileHelper(filename);
        fileHelper.write_to_file(normal_lrc_toT, ScopedFile::Encoding::GBK);
        Badfish::AudioToolkit::LyricParser lyric_parser{filename};
        lyric_parser.change_encoding_utf8(Badfish::FileKits::Encoding::GBK);
        REQUIRE(lyric_parser.is_enhanced() == false);
        REQUIRE(lyric_parser.get_tags() == expected_tags);
        REQUIRE(lyric_parser.get_text() == expected_content_lines);
    }
}

TEST_CASE("LyricParserEnhancedTest", "Enhanced-LRC Test")
{
    const std::string filename{"test_enhanced_complex.lyc"};
    const std::vector<std::string> enhanced_lrc_toT{
        " [t1: Tags Test] "
        , " [标签: 诗意歌词测试] "
        , "[00:05.123] <00:05.123> And <00:05.300> I <00:05.450> remember <00:06.000> all <00:06.200> my <00:06.500> childhood <00:07.000> dreams"
        , "[00:08.500] <00:08.500> I <00:08.650> find <00:08.800> it <00:09.000> hard <00:09.250> to <00:09.400> get <00:09.600> them <00:09.800> out <00:10.000> of <00:10.150> my <00:10.300> mind"
        , "[00:15.000] <00:15.000> 窗 <00:15.200> 透 <00:15.400> 初 <00:15.600> 晓 <00:16.000> 日 <00:16.200> 照 <00:16.400> 西 <00:16.600> 桥 <00:17.000> 云 <00:17.200> 自 <00:17.400> 摇"
        , "[00:20.000] <00:20.000> 想 <00:20.200> 你 <00:20.400> 当 <00:20.600> 年 <00:21.000> 荷 <00:21.200> 风 <00:21.400> 微 <00:21.600> 摆 <00:22.000> 的 <00:22.200> 衣 <00:22.400> 角"
        , "[00:25.000] <00:25.000> 木 <00:25.200> 雕 <00:25.400> 流 <00:25.600> 金 <00:26.000> 岁 <00:26.200> 月 <00:26.400> 涟 <00:26.600> 漪 <00:27.000> 七 <00:27.200> 年 <00:27.400> 前 <00:27.600> 封 <00:27.800> 笔"
        , "[00:30.000] <00:30.000> 因 <00:30.200> 为 <00:30.400> 我 <00:30.600> 今 <00:31.000> 生 <00:31.200> 挥 <00:31.400> 毫 <00:31.600> 只 <00:31.800> 为 <00:32.000> 你"
    };

    const std::vector<std::string> expected_tags{
        "t1: Tags Test",
        "标签: 诗意歌词测试"
    };

    const std::vector<Badfish::AudioToolkit::LyricLine> expected_content_lines{
        {5123, "And I remember all my childhood dreams"},
        {8500, "I find it hard to get them out of my mind"},
        {15000, "窗透初晓日照西桥云自摇"},
        {20000, "想你当年荷风微摆的衣角"},
        {25000, "木雕流金岁月涟漪七年前封笔"},
        {30000, "因为我今生挥毫只为你"}
    };

    SECTION("Enhanced-LRC Test, Encode: UTF8")
    {
        ScopedFile fileHelper(filename);
        fileHelper.write_to_file(enhanced_lrc_toT, ScopedFile::Encoding::UTF8);
        const Badfish::AudioToolkit::LyricParser lyric_parser{filename};

        REQUIRE(lyric_parser.is_enhanced() == true);
        REQUIRE(lyric_parser.get_tags() == expected_tags);
        REQUIRE(lyric_parser.get_text() == expected_content_lines);
    }

    SECTION("Enhanced-LRC Test, Encode: GBK")
    {
        ScopedFile fileHelper(filename);
        fileHelper.write_to_file(enhanced_lrc_toT, ScopedFile::Encoding::GBK);
        Badfish::AudioToolkit::LyricParser lyric_parser{filename};
        lyric_parser.change_encoding_utf8(Badfish::FileKits::Encoding::GBK);

        REQUIRE(lyric_parser.is_enhanced() == true);
        REQUIRE(lyric_parser.get_tags() == expected_tags);
        REQUIRE(lyric_parser.get_text() == expected_content_lines);
    }
}
