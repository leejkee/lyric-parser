//
// Created by 31305 on 25-6-18.
//
#pragma once
#include <string>
#include <vector>
#include <optional>
#include <regex>
#include <cstdint>

namespace AudioToolKits
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

class LyricParser
{
public:
    explicit LyricParser(std::string_view file_path);

    ~LyricParser();

    void parse_lrc(const std::vector<std::string>& file_content);

    void reload_file(std::string_view file_path);


    static int64_t time_to_ms(std::string_view time_str);

<<<<<<< HEAD
    static int64_t time_to_ms(std::string_view time_str);

=======
>>>>>>> 376777115bb2fa29a5e14796eeeef19aa9038605
    static int64_t time_to_ms(std::string_view min
                                       , std::string_view sec
                                       , std::string_view ms);

    [[nodiscard]] std::vector<LyricLine> get_lrc() const;

    [[nodiscard]] std::vector<std::string> get_tags() const;

    [[nodiscard]] std::vector<LyricLine> get_text() const;

    [[nodiscard]] bool is_enhanced() const;

    void clear_result();

    void change_encoding_utf8();

    void print_lyric() const;

private:
    enum class EnhancedState
    {
        Uninitialized, True, False
    };

    std::vector<LyricLine> m_lyric_vector;

    EnhancedState m_is_enhanced{EnhancedState::Uninitialized};

    inline static const std::regex s_regex_match_tag{R"(\[(.*)\])"};

    inline static const std::regex s_regex_search_enhanced_text{
        R"(<([^>]+)>(.*?)(?=<|$))"
    };

    inline static const std::regex s_regex_match_text{
        R"(\[(\d{1,2}):(\d{1,2})\.(\d{2,3})(?:\.(\d{2,3}))?\](.*))"
    };

    inline static const std::regex s_regex_match_time{
        R"((\d{1,2}):(\d{1,2})\.(\d{2,3}))"
    };
};
}
