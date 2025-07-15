//
// Created by 31305 on 2025/7/10.
//
#pragma once
#include <filesystem>
#include <string>
#include <vector>

namespace Badfish::FileKits
{
enum class Encoding
{
    ASCII, UTF8, GBK, UNKNOWN
};

class TextFileHelper
{
public:
    TextFileHelper() = default;
    explicit TextFileHelper(std::string_view filePath);
    TextFileHelper(const TextFileHelper&) = delete;
    TextFileHelper(TextFileHelper&&) = delete;
    TextFileHelper& operator=(const TextFileHelper&) = delete;
    TextFileHelper& operator=(TextFileHelper&&) = delete;
    ~TextFileHelper();

    void load_file(std::string_view file_path);


    [[nodiscard]] std::vector<std::string> get_content() const
    {
        return m_content;
    }

    static bool is_ascii(const std::string& str);

    static std::string encoding_to_string(const Encoding& encoding);

    [[nodiscard]] std::string file_name() const;

#if defined(_WIN32) || defined(_WIN64)
    static std::string convert_encoding(const std::string& o_str
                                        , Encoding o_encoding
                                        , Encoding t_encoding);
#endif

private:
    std::filesystem::path m_path;

    // Encoding m_encoding{Encoding::UNKNOWN};

    std::vector<std::string> m_content;

    // static Encoding detect_str_encoding(const std::string& str);

    static int encoding_to_codepage(Encoding encoding);

    // void detect_file_encoding();

    bool read_file();
};
}
