//
// Created by 31305 on 25-6-18.
//
#pragma once
#include "textfilehelper.h"
#include <memory>
#include <string>
#include <vector>
#include <optional>


namespace AudioToolkit
{
struct LyricLine
{
    std::optional<int64_t> m_start_ms;

    std::string m_text;

    LyricLine() = default;

    explicit LyricLine(std::string&& text)
        :m_text(std::move(text))
    {
    }

    LyricLine(const int64_t time_ms
              , std::string&& text)
        : m_start_ms(time_ms),
          m_text(std::move(text))
    {
    }

    LyricLine(const LyricLine& other) = default;

    LyricLine(LyricLine&& other) = default;

    LyricLine& operator=(const LyricLine& other) = default;

    LyricLine& operator=(LyricLine&& other) noexcept = default;

    bool operator==(const LyricLine& other) const
    {
        return (m_start_ms == other.m_start_ms) && (m_text == other.m_text);
    }

    bool operator!=(const LyricLine& other) const
    {
        return (m_start_ms != other.m_start_ms) || (m_text != other.m_text);
    }

    [[nodiscard]] bool isTag() const {
        return !m_start_ms.has_value();
    }

    [[nodiscard]] bool isText() const
    {
        return m_start_ms.has_value();
    }

    [[nodiscard]] int64_t start_ms() const
    {
        return m_start_ms.value();
    }
};

class LyricParserPrivate;

class LyricParser
{
public:
    LyricParser();

    explicit LyricParser(std::string_view file_path);

    ~LyricParser();

    void parse_lrc();

    void load_file(std::string_view file_path);

    static void trim_string(std::string& str);

    static bool is_English(std::string_view str);

    static int64_t time_to_ms(std::string_view time_str);

    static int64_t time_to_ms(std::string_view min
                                       , std::string_view sec
                                       , std::string_view ms);

    [[nodiscard]] std::vector<LyricLine> get_lrc() const;

    [[nodiscard]] std::vector<std::string> get_tags() const;

    [[nodiscard]] std::vector<LyricLine> get_text() const;

    [[nodiscard]] bool is_enhanced() const;

    void clear_result();

    [[nodiscard]] std::string file_name() const;

    void change_encoding_utf8(FileKits::Encoding t_encoding);

    void print_info() const;

private:
    std::unique_ptr<LyricParserPrivate> d;
};
}
