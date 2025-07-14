//
// Created by 31305 on 25-6-18.
//
#include <lyricparser.h>
#include <iostream>
#include <fstream>
#include <windows.h>

namespace Badfish::AudioToolkit
{
LyricParser::LyricParser(const std::vector<std::string>& string_vector
                         , const FileKits::Encoding encoding)
    : m_encoding(encoding)
{
    if (m_encoding == FileKits::Encoding::GBK)
    {
        for (const auto& str_line : string_vector)
        {
            m_lyric_original.
                    emplace_back(FileKits::TextFileHelper::convert_encoding(str_line
                                     , FileKits::Encoding::GBK
                                     , FileKits::Encoding::UTF8));
        }
    }
    else if (m_encoding == FileKits::Encoding::UTF8)
    {
        m_lyric_original = string_vector;
    }
    else
    {
        std::cerr << "Unknown encoding type" << std::endl;
        return;
    }
    parse_lrc();
}

LyricParser::LyricParser(const FileKits::TextFileHelper& text_file)
    : LyricParser(text_file.get_content(), text_file.get_encoding())
{
}


LyricParser::~LyricParser() = default;

void LyricParser::parse_lrc()
{
    if (m_lyric_original.empty())
    {
        std::cerr << "LyricParser::parse_lrc: empty original LRC vector" <<
                std::endl;
        return;
    }
    for (auto& str_line : m_lyric_original)
    {
        trim_string(str_line);
    }

    std::smatch results_match;
    auto o_it = m_lyric_original.begin();

    // 1. Tags match
    while (o_it != m_lyric_original.end() && std::regex_match(*o_it
        , results_match
        , s_regex_match_tag))
    {
        m_lyric_tags.emplace_back(results_match[1].str());
        ++o_it;
    }
    // 1.

    // 2. Text match
    while (o_it != m_lyric_original.end() && std::regex_match(*o_it
        , results_match
        , s_regex_match_text))
    {
        std::int64_t start_ms = time_to_ms(results_match[1].str()
                                           , results_match[2].str()
                                           , results_match[3].str());
        std::string text = results_match[5].str();
        trim_string(text);
        std::string result;
        if (m_is_enhanced == EnhancedState::Uninitialized)
        {
            if (std::regex_search(text
                                  , results_match
                                  , s_regex_search_enhanced_text))
            {
                m_is_enhanced = EnhancedState::True;
            }
            else
            {
                m_is_enhanced = EnhancedState::False;
            }
        }
        if (m_is_enhanced == EnhancedState::True)
        {
            while (std::regex_search(text
                                     , results_match
                                     , s_regex_search_enhanced_text))
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
        else if (m_is_enhanced == EnhancedState::False)
        {
            result = text;
        }
        m_lyric_vector.emplace_back(start_ms, std::move(result));
        ++o_it;
    }
    // 2.
}

void LyricParser::parse_lrc(std::ifstream& file_stream)
{
    std::string read_line;
    std::smatch results_match;
    while (std::getline(file_stream, read_line))
    {
        trim_string(read_line);
        if (read_line.empty())
        {
            continue;
        }
        // 1. Tags match
        if (std::regex_match(read_line, results_match, s_regex_match_tag))
        {
            m_lyric_tags.emplace_back(results_match[1].str());
            continue;
        }
        // 1.

        // 2. Text match
        if (std::regex_match(read_line, results_match, s_regex_match_text))
        {
            std::int64_t start_ms = time_to_ms(results_match[1].str()
                                               , results_match[2].str()
                                               , results_match[3].str());
            std::string text = results_match[5].str();
            trim_string(text);
            std::string result;
            if (m_is_enhanced == EnhancedState::Uninitialized)
            {
                if (std::regex_search(text
                                      , results_match
                                      , s_regex_search_enhanced_text))
                {
                    m_is_enhanced = EnhancedState::True;
                }
                else
                {
                    m_is_enhanced = EnhancedState::False;
                }
            }

            if (m_is_enhanced == EnhancedState::True)
            {
                while (std::regex_search(text
                                         , results_match
                                         , s_regex_search_enhanced_text))
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
            else if (m_is_enhanced == EnhancedState::False)
            {
                result = text;
            }
            m_lyric_vector.emplace_back(start_ms, std::move(result));
        }
        // 2.
    }
}

std::vector<LyricLine> LyricParser::get_lrc_text() const
{
    return m_lyric_vector;
}

std::vector<std::string> LyricParser::get_lyric_tags() const
{
    return m_lyric_tags;
}

bool LyricParser::is_enhanced() const
{
    return m_is_enhanced == EnhancedState::True;
}

std::int64_t LyricParser::time_to_ms(
    const std::string_view time_str)
{
    const std::string str{time_str};
    std::int64_t result{0};
    if (std::smatch time_match; std::regex_match(str
                                                 , time_match
                                                 , s_regex_match_time))
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

FileKits::Encoding LyricParser::get_encoding() const
{
    return m_encoding;
}

void LyricParser::clear_result()
{
    m_is_enhanced = EnhancedState::Uninitialized;
    m_lyric_tags.clear();
    m_lyric_vector.clear();
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
    return std::all_of(str.begin(), str.end(), [](const unsigned char ch)
    {
        return std::isalpha(ch);
    });
}


const std::regex LyricParser::s_regex_match_tag{R"(\[(.*)\])"};

const std::regex LyricParser::s_regex_search_enhanced_text{
    R"(<([^>]+)>(.*?)(?=<|$))"
};

const std::regex LyricParser::s_regex_match_text{
    R"(\[(\d{1,2}):(\d{1,2})\.(\d{2,3})(?:\.(\d{2,3}))?\](.*))"
};

const std::regex LyricParser::s_regex_match_time{
    R"((\d{1,2}):(\d{1,2})\.(\d{2,3}))"
};
}
