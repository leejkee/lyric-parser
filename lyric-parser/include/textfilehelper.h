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
    explicit TextFileHelper(const std::filesystem::path& filePath);

    TextFileHelper(const TextFileHelper&) = delete;
    TextFileHelper(TextFileHelper&&) = delete;
    TextFileHelper& operator=(const TextFileHelper&) = delete;
    TextFileHelper& operator=(TextFileHelper&&) = delete;


    ~TextFileHelper() = default;

    bool read_file();

    // getter
    [[nodiscard]] Encoding get_encoding() const
    {
        return m_encoding;
    };

    [[nodiscard]] std::vector<std::string> get_content() const
    {
        return m_content;
    }

    static bool is_ascii(const std::string& str);

#if defined(_WIN32) || defined(_WIN64)
    static std::string convert_encoding(const std::string& o_str
                                        , Encoding o_encoding
                                        , Encoding t_encoding);
#endif

private:
    std::filesystem::path m_path;
    Encoding m_encoding;
    std::vector<std::string> m_content;

    static Encoding detect_str_encoding(const std::string& str);


    static int encoding_to_codepage(Encoding encoding);


    void detect_file_encoding();
};
}
