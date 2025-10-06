//
// Created by 31305 on 2025/6/28.
//

#pragma once
#include <filesystem>
#include <vector>

class ScopedFile
{
public:
    enum class Encoding
    {
        UTF8, GBK
    };

    explicit ScopedFile(std::filesystem::path filePath);

    ~ScopedFile();

    ScopedFile(const ScopedFile&) = delete;
    ScopedFile(ScopedFile&&) = delete;
    ScopedFile& operator=(const ScopedFile&) = delete;
    ScopedFile& operator=(ScopedFile&&) = delete;

    bool write_to_file(const std::vector<std::string>& content_lines
                       , Encoding encoding = Encoding::UTF8);

#if defined (_WIN32) || defined(_WIN64)
    bool write_to_file_winapi(const std::string& content, Encoding target_encoding);
#endif

#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    bool write_to_file_iconv(std::string_view content, Encoding encoding);
#endif

private:
    std::filesystem::path m_filePath;
    static constexpr char NEWLINE_CHAR{'\n'};
};
