//
// Created by 31305 on 2025/7/10.
//
#include <textfilehelper.h>
#include <algorithm>
#include <fstream>
#include <iostream>

#include <Windows.h>

namespace Badfish::FileKits
{
TextFileHelper::TextFileHelper(const std::filesystem::path& filePath)
    : m_path(filePath),
      m_encoding(Encoding::UNKNOWN)
{
    if (read_file() == true)
    {
        detect_file_encoding();
    }
}

bool TextFileHelper::read_file()
{
    std::ifstream lyric_stream(m_path, std::ios::binary | std::ios::in);
    if (!lyric_stream.is_open())
    {
        std::cerr << "Error: Failed to open file\n";
        return false;
    }
    std::string read_line;
    while (std::getline(lyric_stream, read_line))
    {
        if (read_line.empty())
        {
            continue;
        }
        m_content.emplace_back(read_line);
    }
    if (m_content.empty())
    {
        std::cerr << "Warning: Read empty file: " << m_path.filename() << "\n";
        return false;
    }
    return true;
}

void TextFileHelper::detect_file_encoding()
{
    if (m_content.empty())
    {
        m_encoding = Encoding::UNKNOWN;
        return;
    }
    for (const auto& line : m_content)
    {
        switch (detect_str_encoding(line))
        {
        case Encoding::ASCII: {
            break;
        }
        case Encoding::GBK: {
            m_encoding = Encoding::GBK;
            return;
        }
        case Encoding::UTF8: {
            m_encoding = Encoding::UTF8;
            return;
        }
        default: {
            return;
        }
        }
    }
}


Encoding TextFileHelper::detect_str_encoding(const std::string& str)
{
    if (is_ascii(str))
    {
        return Encoding::ASCII;
    }
    if (const std::string copy_str = convert_encoding(str
        , Encoding::GBK
        , Encoding::GBK); copy_str == str)
    {
        return Encoding::GBK;
    }
    if (const std::string copy_str = convert_encoding(str
        , Encoding::UTF8
        , Encoding::UTF8); copy_str == str)
    {
        return Encoding::UTF8;
    }
    return Encoding::UNKNOWN;
}

bool TextFileHelper::is_ascii(const std::string& str)
{
    return std::all_of(str.begin()
                       , str.end()
                       , [](const char c)
                       {
                           return (static_cast<unsigned char>(c) <= 0x7F);
                       });
}


int TextFileHelper::encoding_to_codepage(const Encoding encoding)
{
    switch (encoding)
    {
    case Encoding::UTF8: {
        return CP_UTF8;
    }
    case Encoding::GBK: {
        return 936;
    }
    default: {
        return 0;
    }
    }
}

// Linux todo

#if defined(_WIN32) || defined(_WIN64)
std::string TextFileHelper::convert_encoding(const std::string& o_str
                                             , const Encoding o_encoding
                                             , const Encoding t_encoding)
{
    const UINT o_code_page = encoding_to_codepage(o_encoding);
    const UINT t_code_page = encoding_to_codepage(t_encoding);

    const int w_str_len =
            MultiByteToWideChar(o_code_page, 0, o_str.c_str(), -1, nullptr, 0);
    std::wstring w_str(w_str_len - 1, L'\0');
    MultiByteToWideChar(
                        o_code_page
                        , 0
                        , o_str.data()
                        , -1
                        , w_str.data()
                        , w_str_len - 1);

    const int t_str_len =
            WideCharToMultiByte(t_code_page
                                , 0
                                , w_str.c_str()
                                , -1
                                , nullptr
                                , 0
                                , nullptr
                                , nullptr);
    std::string t_str(t_str_len - 1, '\0');
    WideCharToMultiByte(
                        t_code_page
                        , 0
                        , w_str.c_str()
                        , -1
                        , t_str.data()
                        , t_str_len - 1
                        , nullptr
                        , nullptr);

    return t_str;
}
#endif
}
