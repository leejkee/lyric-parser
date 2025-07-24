//
// Created by 31305 on 25-6-18.
//
#pragma once
#include "textfilehelper.h"
#include <memory>
#include <string>
#include <vector>
#include <optional>


namespace Badfish::AudioToolkit
{
/// Enhanced LRC supported
/// todo
// struct EnhancedLyricLineInfo
// {
//     std::int64_t m_end_ms{0};
//     std::queue<std::int64_t> m_timing_queue;
//
//     explicit EnhancedLyricLineInfo() = default;
//
//     explicit EnhancedLyricLineInfo(const std::int64_t end_ms
//                                    , std::queue<std::int64_t>&& timing_q)
//         : m_end_ms(end_ms),
//           m_timing_queue(std::move(timing_q))
//     {
//     }
//
//     EnhancedLyricLineInfo& operator=(const EnhancedLyricLineInfo& other)
//     = default;
//
//     EnhancedLyricLineInfo& operator=(EnhancedLyricLineInfo&& other) noexcept
//     {
//         m_end_ms = other.m_end_ms;
//         m_timing_queue = std::move(other.m_timing_queue);
//         return *this;
//     }
// };
/// Enhanced LRC supported

struct LyricLine
{
    std::optional<int64_t> m_start_ms;

    std::string m_text;

    explicit LyricLine(std::string&& text)
        :m_text(std::move(text))
    {
    }

    LyricLine(const std::int64_t time_ms
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

    bool isTag() const {
        return !m_start_ms.has_value();
    }

    bool isText() const
    {
        return m_start_ms.has_value();
    }

    int64_t start_ms() const
    {
        return m_start_ms.value();
    }
};

class LyricParserPrivate;

class LyricParser
{
public:
    LyricParser() = default;

    explicit LyricParser(std::string_view file_path);

    ~LyricParser();

    void parse_lrc();

    void load_file(std::string_view file_path);

    static void trim_string(std::string& str);

    static bool is_English(std::string_view str);

    static std::int64_t time_to_ms(std::string_view time_str);

    static std::int64_t time_to_ms(std::string_view min
                                       , std::string_view sec
                                       , std::string_view ms);

    [[nodiscard]] std::vector<LyricLine> get_lrc() const;

    [[nodiscard]] std::vector<std::string> get_tags() const;

    [[nodiscard]] std::vector<LyricLine> get_text() const;

    [[nodiscard]] bool is_enhanced() const;

    void clear_result();

    [[nodiscard]] std::string file_name() const;

    void change_encoding(FileKits::Encoding t_encoding);

    void print_info() const;

private:
    std::unique_ptr<LyricParserPrivate> d;
};
}
