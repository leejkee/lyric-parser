//
// Created by 31305 on 25-6-18.
//
#include <lyricparser.h>
#include <iostream>
#include <regex>

#include <windows.h>

namespace Badfish::AudioToolkit
{
class LyricParserPrivate
{
public:
    enum class EnhancedState
    {
        Uninitialized, True, False
    };

    explicit LyricParserPrivate(const std::string_view filePath)
        : m_file_helper{filePath}
    {
    }

    ~LyricParserPrivate();

    LyricParserPrivate(const LyricParserPrivate&) = delete;

    FileKits::TextFileHelper m_file_helper;

    std::vector<LyricLine> m_lyric_vector;

    std::vector<std::string> m_lyric_tags;

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

LyricParserPrivate::~LyricParserPrivate() = default;

LyricParser::LyricParser(const std::string_view file_path)
    : d(std::make_unique<LyricParserPrivate>(file_path))
{
    parse_lrc();
}

LyricParser::~LyricParser() = default;

void LyricParser::parse_lrc()
{
    auto original_vector = d->m_file_helper.get_content();
    if (original_vector.empty())
    {
        std::cerr << "LyricParser::parse_lrc: empty original LRC vector" <<
                std::endl;
        return;
    }
    for (auto& str_line : original_vector)
    {
        trim_string(str_line);
    }

    std::smatch results_match;
    auto o_it = original_vector.begin();

    // 1. Tags match
    while (o_it != original_vector.end() && std::regex_match(*o_it
        , results_match
        , LyricParserPrivate::s_regex_match_tag))
    {
        d->m_lyric_tags.emplace_back(results_match[1].str());
        ++o_it;
    }
    // 1.

    // 2. Text match
    while (o_it != original_vector.end() && std::regex_match(*o_it
        , results_match
        , LyricParserPrivate::s_regex_match_text))
    {
        std::int64_t start_ms = time_to_ms(results_match[1].str()
                                           , results_match[2].str()
                                           , results_match[3].str());
        std::string text = results_match[5].str();
        trim_string(text);
        std::string result;
        if (d->m_is_enhanced ==
            LyricParserPrivate::EnhancedState::Uninitialized)
        {
            if (std::regex_search(text
                                  , results_match
                                  , LyricParserPrivate::s_regex_search_enhanced_text))
            {
                d->m_is_enhanced = LyricParserPrivate::EnhancedState::True;
            }
            else
            {
                d->m_is_enhanced = LyricParserPrivate::EnhancedState::False;
            }
        }
        if (d->m_is_enhanced == LyricParserPrivate::EnhancedState::True)
        {
            while (std::regex_search(text
                                     , results_match
                                     , LyricParserPrivate::s_regex_search_enhanced_text))
            {
                std::string match_word = results_match[2].str();
                trim_string(match_word);
                if (is_English(match_word))
                {
                    match_word += ' ';
                }
                result.append(match_word);
                text = results_match.suffix();
            }
            if (std::isspace(static_cast<unsigned char>(result.back())))
            {
                result.pop_back();
            }
        }
        else if (d->m_is_enhanced == LyricParserPrivate::EnhancedState::False)
        {
            result = text;
        }
        d->m_lyric_vector.emplace_back(start_ms, std::move(result));
        ++o_it;
    }
    // 2.
}

void LyricParser::load_file(const std::string_view file_path)
{
    d->m_file_helper.load_file(file_path);
    clear_result();
    parse_lrc();
}

std::vector<LyricLine> LyricParser::get_lrc_text() const
{
    return d->m_lyric_vector;
}

std::vector<std::string> LyricParser::get_lyric_tags() const
{
    return d->m_lyric_tags;
}

bool LyricParser::is_enhanced() const
{
    return d->m_is_enhanced == LyricParserPrivate::EnhancedState::True;
}

std::int64_t LyricParser::time_to_ms(
    const std::string_view time_str)
{
    const std::string str{time_str};
    std::int64_t result{0};
    if (std::smatch time_match; std::regex_match(str
                                                 , time_match
                                                 , LyricParserPrivate::s_regex_match_time))
    {
        result = time_to_ms(time_match[1].str()
                            , time_match[2].str()
                            , time_match[3].str());
    }
    return result;
}

std::int64_t LyricParser::time_to_ms(
    const std::string_view min
    , const std::string_view sec
    , const std::string_view ms)
{
    const std::int64_t time_min{std::stoi(std::string{min})};
    const std::int64_t time_sec{std::stoi(std::string{sec})};
    const std::int64_t time_ms{std::stoi(std::string{ms})};
    return (time_min * 60 + time_sec) * 1000 + time_ms;
}

void LyricParser::clear_result()
{
    d->m_is_enhanced = LyricParserPrivate::EnhancedState::Uninitialized;
    d->m_lyric_tags.clear();
    d->m_lyric_vector.clear();
}

void LyricParser::trim_string(std::string& str)
{
    if (str.length() >= 3 &&
        static_cast<unsigned char>(str[0]) == 0xEF &&
        static_cast<unsigned char>(str[1]) == 0xBB &&
        static_cast<unsigned char>(str[2]) == 0xBF)
    {
        str.erase(0, 3); // Remove the 3-byte UTF-8 BOM
    }
    str.erase(str.begin()
              , std::find_if(str.begin()
                             , str.end()
                             , [](const unsigned char ch)
                             {
                                 return !std::isspace(ch);
                             }));

    str.erase(std::find_if(str.rbegin()
                           , str.rend()
                           , [](const unsigned char ch)
                           {
                               return !std::isspace(ch);
                           }).base()
              , str.end());
}

bool LyricParser::is_English(const std::string_view str)
{
    return std::all_of(str.begin()
                       , str.end()
                       , [](const unsigned char ch)
                       {
                           return std::isalpha(ch);
                       });
}

std::string LyricParser::file_name() const
{
    return d->m_file_helper.file_name();
}

void LyricParser::change_encoding(const FileKits::Encoding t_encoding)
{
    if (!d->m_lyric_tags.empty())
    {
        for (auto& tag : d->m_lyric_tags)
        {
            tag = FileKits::TextFileHelper::convert_encoding(tag
                , t_encoding
                , FileKits::Encoding::UTF8);
        }
    }

    if (!d->m_lyric_vector.empty())
    {
        for (auto& lyric : d->m_lyric_vector)
        {
            if (!lyric.m_text.empty())
            {
                lyric.m_text =
                        FileKits::TextFileHelper::convert_encoding(lyric.m_text
                                , t_encoding
                                , FileKits::Encoding::UTF8
                            );
            }
        }
    }
}

void LyricParser::print_info() const
{
    std::cout << "File name: " << file_name() << std::endl;
    std::cout << "Tags:" << std::endl;
    for (const auto& tag : d->m_lyric_tags)
    {
        std::cout << tag << std::endl;
    }
    std::cout << "Lyric text:" << std::endl;

    for (const auto& lyric : d->m_lyric_vector)
    {
        std::cout << lyric.m_text << std::endl;
    }
}
}
