//
// Created by 31305 on 2025/7/10.
//
#pragma once
#include <filesystem>
#include <string>
#include <vector>

namespace AudioToolKits
{
enum class Encoding
{
    UTF8, GBK, UNKNOWN
};

class TextFileHelper
{
public:
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

    static std::string convert_encoding(const std::string& o_str
                                        , Encoding o_encoding
                                        , Encoding t_encoding);

    static void trim_string(std::string& str);

    static bool is_English(std::string_view str);

private:
    std::filesystem::path m_path;

    std::vector<std::string> m_content;

    bool read_file();

};
}
