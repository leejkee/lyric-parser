//
// Created by 31305 on 2025/6/28.
//
#include "scopedfile.h"
#include <fstream>
#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
// Windows API specific includes
#include <windows.h>
#include <utility>
#endif

#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
// headers unix only
#include <iconv.h>
#include <errno.h>
#include <string.h>
#endif

namespace LPTest
{

ScopedFile::ScopedFile(std::filesystem::path filePath)
    : m_filePath(std::move(filePath))
{
}

ScopedFile::~ScopedFile()
{
    if (std::filesystem::exists(m_filePath))
    {
        if (std::error_code ec; std::filesystem::remove(m_filePath, ec))
        {
            std::cout << "FileHelper: Successfully deleted file: " << m_filePath
                    << std::endl;
        }
        else
        {
            std::cerr << "FileHelper Error: Failed to delete file: " <<
                    m_filePath << ". Reason: " << ec.message() << std::endl;
        }
    }
    else
    {
        std::cout << "FileHelper: File does not exist, no need to delete: " <<
                m_filePath << std::endl;
    }
}

// -----------------------------------------------------------------------------
// Private helper for Windows API implementation
// -----------------------------------------------------------------------------
#if defined(_WIN32) || defined(_WIN64)
bool ScopedFile::write_to_file_winapi(const std::string& content
                                      , const Encoding target_encoding)
{
    int code_page = 0;
    std::vector<char> output_bytes;
    bool add_bom = false;
    switch (target_encoding)
    {
    case Encoding::UTF8:
        code_page = CP_UTF8;
        add_bom = true;
        break;
    case Encoding::GBK:
        code_page = 936;
        break;
    default:
        std::cerr << "Unsupported encoding for WinAPI." << std::endl;
        return false;
    }

    // 1. convert to Unicode UTF16LE for using Win API
    const int w_str_size = MultiByteToWideChar(CP_UTF8
                                               , 0
                                               , content.c_str()
                                               , -1
                                               , nullptr
                                               , 0);
    if (w_str_size == 0)
    {
        std::cerr <<
                "WinAPI Error: MultiByteToWideChar (UTF-8 to UTF-16) failed, GetLastError: "
                << GetLastError() << std::endl;
        return false;
    }
    std::wstring w_str(w_str_size - 1, L'\0');
    if (w_str_size != MultiByteToWideChar(CP_UTF8
                                          , 0
                                          , content.c_str()
                                          , -1
                                          , &w_str[0]
                                          , w_str_size))
    {
        std::cerr <<
                "WinAPI Error: MultiByteToWideChar (UTF-8 to UTF-16) failed, GetLastError: "
                << GetLastError() << std::endl;
        return false;
    }
    // 1.

    // 2. convert to the target encoding by Win API
    const int mb_chars_needed = WideCharToMultiByte(code_page
                                                    , 0
                                                    , w_str.c_str()
                                                    , -1
                                                    , nullptr
                                                    , 0
                                                    , nullptr
                                                    , nullptr);
    if (mb_chars_needed == 0)
    {
        std::cerr <<
                "WinAPI Error: WideCharToMultiByte (UTF-16 to target) failed, GetLastError: "
                << GetLastError() << std::endl;
        return false;
    }
    // -1 to exclude null terminator that WideCharToMultiByte adds
    output_bytes.resize(mb_chars_needed);
    WideCharToMultiByte(code_page
                        , 0
                        , w_str.c_str()
                        , -1
                        , output_bytes.data()
                        , static_cast<int>(output_bytes.size())
                        , nullptr
                        , nullptr);
    if (output_bytes.back() == '\0')
    {
        output_bytes.pop_back();
    }
    // 2.

    // 3. write wstring to file
    std::ofstream outFile(m_filePath, std::ios::binary | std::ios::out);
    if (!outFile.is_open())
    {
        std::cerr << "WinAPI Error: Could not open file: " << m_filePath <<
                std::endl;
        return false;
    }
    if (add_bom)
    {
        constexpr unsigned char bom[] = {0xEF, 0xBB, 0xBF};
        outFile.write(reinterpret_cast<const char*>(bom), sizeof(bom));
    }
    outFile.write(output_bytes.data()
                  , static_cast<long long>(output_bytes.size()));
    outFile.close();
    // 3.
    return true;
}
#endif // _WIN32 || _WIN64

// -----------------------------------------------------------------------------
// Private helper for iconv implementation
// -----------------------------------------------------------------------------
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
bool ScopedFile::write_to_file_iconv(std::string_view content
                                     , Encoding encoding)
{
    const char* target_encoding_str = nullptr;
    // bool add_bom = false;

    switch (encoding)
    {
    case Encoding::UTF8:
        target_encoding_str = "UTF-8";
        break;
    case Encoding::GBK:
        target_encoding_str = "GBK";
        break;
    default:
        std::cerr << "Unsupported encoding for iconv." << std::endl;
        return false;
    }

    iconv_t cd = iconv_open(target_encoding_str, "UTF-8");
    if (cd == (iconv_t) - 1)
    {
        std::cerr << "Iconv Error: iconv_open failed from UTF-8 to " <<
                target_encoding_str << ": " << strerror(errno) << std::endl;
        return false;
    }

    char* in_buf_ptr = const_cast<char*>(content.data());
    size_t in_bytes_left = content.length();

    // Allocate output buffer with an educated guess
    // For UTF-8 to UTF-16, it can be up to 2x. For others, it might be more.
    size_t out_buf_size = in_bytes_left * 2;
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
        return false;
    }

    size_t converted_bytes = out_buf_size - out_bytes_left;

    iconv_close(cd);

    // Write to file
    std::ofstream outFile(m_filePath, std::ios::binary);
    if (!outFile.is_open())
    {
        std::cerr << "Iconv Error: Could not open file: " << m_filePath <<
                std::endl;
        return false;
    }

    outFile.write(output_bytes.data(), converted_bytes);
    outFile.close();
    return true;
}

#endif // __linux__ || __unix__ || __APPLE__

// -----------------------------------------------------------------------------
// Public write_to_file function implementation
// -----------------------------------------------------------------------------
bool ScopedFile::write_to_file(const std::vector<std::string>& content_lines
                               , const Encoding encoding)
{
    std::string full_content;
    for (const auto& line : content_lines)
    {
        full_content += line;
        full_content += NEWLINE_CHAR;
    }

    if (!full_content.empty() && full_content.back() == '\n')
    {
        full_content.pop_back();
    }


#if defined(_WIN32) || defined(_WIN64)
    return write_to_file_winapi(full_content, encoding);
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    return write_to_file_iconv(full_content, encoding);
#else
    std::cerr << "Error: write_to_file not implemented for this platform." << std::endl;
    return false;
#endif
}
}