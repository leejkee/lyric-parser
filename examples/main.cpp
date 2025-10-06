//
// Created by 31305 on 2025/7/14.
//
#include <lyricparser.h>
#include <string>

int main()
{
    using namespace Badfish;

    // file with GBK encoding
    const std::string file_1{"../../examples/成全 - 刘若英.lrc"};

    // file with UTF8 encoding
    const std::string file_2{"../../examples/清明雨上-许嵩.lrc"};

    AudioToolkit::LyricParser parser(file_1);
    parser.change_encoding_utf8(FileKits::Encoding::GBK);
    parser.print_info();

    parser.load_file(file_2);
    parser.print_info();
}
