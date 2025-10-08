//
// Created by 31305 on 25-6-18.
//
#include <lyricparser.h>
#include <iostream>
#include "textfilehelper.h"

namespace AudioToolKits
{
LyricParser::LyricParser(const std::string_view file_path)
{
    reload_file(file_path);
}

LyricParser::~LyricParser() = default;

void LyricParser::parse_lrc(const std::vector<std::string>& file_content)
{
    if (file_content.empty())
    {
        std::cerr << "LyricParser::parse_lrc: empty original LRC vector" <<
                std::endl;
        return;
    }

    std::smatch results_match;
    auto o_it = file_content.begin();

    // 1. Tags match
    while (o_it != file_content.end() && std::regex_match(*o_it
        , results_match
        , s_regex_match_tag))
    {
        m_lyric_vector.emplace_back(results_match[1].str());
        ++o_it;
    }
    // 1.

    // 2. Text match
    while (o_it != file_content.end() && std::regex_match(*o_it
        , results_match
        , s_regex_match_text))
    {
        std::int64_t start_ms = time_to_ms(results_match[1].str()
                                           , results_match[2].str()
                                           , results_match[3].str());
        std::string text = results_match[5].str();
        TextFileHelper::trim_string(text);
        std::string result;
        if (m_is_enhanced ==
            EnhancedState::Uninitialized)
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
                TextFileHelper::trim_string(match_word);
                if (TextFileHelper::is_English(match_word))
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

void LyricParser::reload_file(const std::string_view file_path)
{
    clear_result();
    const TextFileHelper text_file{file_path};
    const auto content = text_file.get_content();
    parse_lrc(text_file.get_content());
}

std::vector<LyricLine> LyricParser::get_lrc() const
{
    return m_lyric_vector;
}

std::vector<std::string> LyricParser::get_tags() const{
    std::vector<std::string> tags;
    for (const auto& line : m_lyric_vector)
    {
        if (line.isTag())
        {
            tags.emplace_back(line.m_text);
        }
        else{
            break;
        }
    }
    return tags;
}

std::vector<LyricLine> LyricParser::get_text() const{
    std::vector<LyricLine> text;
    auto it = m_lyric_vector.begin();
    while(it != m_lyric_vector.end() && it->isTag())
    {
        ++it;
    }
    for (; it != m_lyric_vector.end(); ++it)
    {
        text.emplace_back(*it);
    }
    return text;
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

void LyricParser::clear_result()
{
    m_is_enhanced = EnhancedState::Uninitialized;
    m_lyric_vector.clear();
}

void LyricParser::change_encoding_utf8()
{
    if (!m_lyric_vector.empty())
    {
        for (auto& lyric : m_lyric_vector)
        {
            if (!lyric.m_text.empty())
            {
                lyric.m_text =
                        TextFileHelper::convert_encoding(lyric.m_text
                                ,Encoding::GBK
                                , Encoding::UTF8
                            );
            }
        }
    }
}

void LyricParser::print_lyric() const
{
    for (const auto& lyric : m_lyric_vector)
    {
        std::cout << lyric.m_text << std::endl;
    }
}
}
