//
// Created by 31305 on 2025/7/10.
//
#include <textfilehelper.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <utility>

#if defined (_WIN32) || defined(_WIN64)
#include <Windows.h>
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
#include <iconv.h>
#include <cstring>
#endif

namespace Badfish::FileKits
{
TextFileHelper::TextFileHelper(const std::string_view filePath)
    : m_path{filePath}
{
    read_file();
}

TextFileHelper::~TextFileHelper() = default;

void TextFileHelper::load_file(const std::string_view file_path)
{
    m_path = std::filesystem::path(file_path);
    m_content.clear();
    read_file();
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

// void TextFileHelper::detect_file_encoding()
// {
//     if (m_content.empty())
//     {
//         m_encoding = Encoding::UNKNOWN;
//         return;
//     }
//     for (const auto& line : m_content)
//     {
//         switch (detect_str_encoding(line))
//         {
//         case Encoding::ASCII: {
//             break;
//         }
//         case Encoding::GBK: {
//             m_encoding = Encoding::GBK;
//             return;
//         }
//         case Encoding::UTF8: {
//             m_encoding = Encoding::UTF8;
//             return;
//         }
//         default: {
//             return;
//         }
//         }
//     }
// }
//
//
// Encoding TextFileHelper::detect_str_encoding(const std::string& str)
// {
//     if (is_ascii(str))
//     {
//         return Encoding::ASCII;
//     }
//     if (const std::string copy_str = convert_encoding(str
//         , Encoding::GBK
//         , Encoding::GBK); copy_str == str)
//     {
//         return Encoding::GBK;
//     }
//     if (const std::string copy_str = convert_encoding(str
//         , Encoding::UTF8
//         , Encoding::UTF8); copy_str == str)
//     {
//         return Encoding::UTF8;
//     }
//     return Encoding::UNKNOWN;
// }

bool TextFileHelper::is_ascii(const std::string& str)
{
    return std::all_of(str.begin()
                       , str.end()
                       , [](const char c)
                       {
                           return (static_cast<unsigned char>(c) <= 0x7F);
                       });
}

std::string TextFileHelper::encoding_to_string(const Encoding& encoding)
{
    switch (encoding)
    {
        case Encoding::ASCII: {
            return "ASCII";
        }
        case Encoding::GBK: {
            return "GBK";
        }
        case Encoding::UTF8: {
            return "UTF8";
        }
        case Encoding::UNKNOWN: {
            return "UNKNOWN";
        }
        default: {
            return {};
        }
    }
}

std::string TextFileHelper::file_name() const
{
    return m_path.filename().string();
}



#if defined(_WIN32) || defined(_WIN64)
std::string TextFileHelper::convert_encoding(const std::string& o_str
                                             , const Encoding o_encoding
                                             , const Encoding t_encoding)
{
    auto encoding_to_codepage = [](const Encoding encoding) -> UINT
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
    };
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
#elif defined (__linux__) || defined(__unix__) || defined(__APPLE__)
std::string TextFileHelper::convert_encoding(const std::string& o_str
                                             , const Encoding o_encoding
                                             , const Encoding t_encoding)
{
    const char* o_encoding_str = nullptr;
    const char* t_encoding_str = nullptr;

    switch (o_encoding)
    {
    case Encoding::UTF8:
        o_encoding_str = "UTF-8";
        break;
    case Encoding::GBK:
        o_encoding_str = "GBK";
        break;
    default:
        std::cerr << "Unsupported original encoding for iconv." << std::endl;
        return {};
    }

    switch (t_encoding)
    {
    case Encoding::UTF8:
        t_encoding_str = "UTF-8";
        break;
    case Encoding::GBK:
        t_encoding_str = "GBK";
        break;
    default:
        std::cerr << "Unsupported target encoding for iconv." << std::endl;
        return {};
    }

    iconv_t cd = iconv_open(t_encoding_str, o_encoding_str);
    if (cd == (iconv_t)-1)
    {
        std::cerr << "Iconv Error: iconv_open failed: " << strerror(errno) <<
                std::endl;
        return {};
    }

    char* in_buf_ptr = const_cast<char*>(o_str.data());
    size_t in_bytes_left = o_str.length();

    // Allocate output buffer with an educated guess
    size_t out_buf_size = in_bytes_left * 2 + 4; // Plus 4 for safety margin
    std::vector<char> output_bytes(out_buf_size);
    char* out_buf_ptr = output_bytes.data();
    size_t out_bytes_left = out_buf_size;

    size_t result = iconv(cd
                          , &in_buf_ptr
                          , &in_bytes_left
                          , &out_buf_ptr
                          , &out_bytes_left);
    if (result == (size_t)-1)
    {
        std::cerr << "Iconv Error: During conversion: " << strerror(errno) <<
                std::endl;
        iconv_close(cd);
        return {};
    }

    size_t converted_bytes = out_buf_size - out_bytes_left;

    iconv_close(cd);

    return std::string(output_bytes.data(), converted_bytes);
}
#endif
}