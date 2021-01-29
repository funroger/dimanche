// copyright (c) 2013 Victor Cherepanov
// distributed under BSD 3-clause license

#pragma once

#if !defined(__DIMANCHE_IO_CONSOLE_H)
#define __DIMANCHE_IO_CONSOLE_H

#include <dimanche/basic/types.h>

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

namespace dimanche {
namespace io {
namespace con {

struct COLOR {
    uint16_t color;
};

namespace font {

const COLOR green(void);
const COLOR red(void);
const COLOR reset(void);
const COLOR yellow(void);
const COLOR white(void);

} // namespace font

template <typename char_t>
class CCmdArgs
{
public:
    using string_t = std::basic_string<char_t>;

    CCmdArgs(const size_t argc, const char_t * const *argv)
    {
        for (size_t i = 1; i < argc; ++i) {
            const string_t option(argv[i]);

            if (('-' == option[0]) || ('+' == option[0])) {
                const string_t::size_type equalCharIdx = option.find('=');
                string_t key, value;

                if (string_t::npos == equalCharIdx) {
                    key = option.substr(1);
                } else {
                    key = option.substr(1, equalCharIdx - 1);
                    value = option.substr(equalCharIdx + 1);
                }

                m_lut[key] = value;
            } else {
                m_fileNames.push_back(option);
            }
        }
    }

    // is a given parameter passed
    inline
    bool Find(const string_t &name) const {
        auto iter = m_lut.find(name);
        return ((m_lut.end() == iter) ? (false) : (true));
    }

    template <typename type_t> inline
    type_t Get(const string_t& key) const {
        return CArgParser::Convert<type_t> (m_lut.at(key));
    }

    template <typename type_t> inline
    type_t Get(const string_t &key, const type_t &default) const
    {
        auto iter = m_lut.find(key);

        if (m_lut.end() != iter) {
            return Convert<type_t> ((*iter).second);
        } else {
            return default;
        }
    }

    inline
    const string_t & GetFileName(const size_t index) const {
        return (index < m_fileNames.size()) ? (m_fileNames[index]) :  (nullptr);
    }

    const string_t & GetFileName(const string_t extension) const
    {
        for (auto &fileName : m_fileNames) {
            const size_t length = fileName.length();
            const size_t extLength = extension.length();

            if (length >= extLength) {
                if (fileName.substr(length - extLength, extLength) == extension) {
                    return fileName;
                }
            }
        }

        return nullptr;
    }

private:

    template <typename data_t> static inline
    data_t Convert(const std::string &s);

    template <> static inline
    const char * Convert<const char *> (const std::string &s) {return s.c_str();}
    template <> static inline
    int32_t Convert<int32_t> (const std::string &s) {return std::stoi(s);}
    template <> static inline
    uint32_t Convert<uint32_t> (const std::string &s) {return std::stoul(s);}
    template <> static inline
    uint64_t Convert<uint64_t> (const std::string &s) {return std::stoull(s);}
    template <> static inline
    float Convert<float> (const std::string &s) {return std::stof(s);}
    template <> static inline
    double Convert<double> (const std::string &s) {return std::stod(s);}
    template <> static inline
    const std::string Convert<const std::string> (const std::string &s) {return s;}

    template <typename data_t> static inline
    data_t Convert(const std::wstring &s);

    template <> static inline
    const wchar_t * Convert<const wchar_t *> (const std::wstring &s) {return s.c_str();}
    template <> static inline
    int32_t Convert<int32_t> (const std::wstring &s) {return std::stoi(s);}
    template <> static inline
    uint32_t Convert<uint32_t> (const std::wstring &s) {return std::stoul(s);}
    template <> static inline
    uint64_t Convert<uint64_t> (const std::wstring &s) {return std::stoull(s);}
    template <> static inline
    float Convert<float> (const std::wstring &s) {return std::stof(s);}
    template <> static inline
    double Convert<double> (const std::wstring &s) {return std::stod(s);}
    template <> static inline
    const std::wstring Convert<const std::wstring> (const std::wstring &s) {return s;}

    // command line options
    std::unordered_map<string_t, string_t> m_lut;
    // file pathes
    std::vector<string_t> m_fileNames;
};

} // namespace con
} // namespace io
} // namespace dimanche

std::ostream & operator << (std::ostream &out,
    const dimanche::io::con::COLOR && color);
std::wostream & operator << (std::wostream &out,
    const dimanche::io::con::COLOR && color);

// declare the library name
#if defined(_WINDOWS)
#pragma comment (lib, "io")
#endif // defined(_WINDOWS)

#endif // !defined(__DIMANCHE_IO_CONSOLE_H)
