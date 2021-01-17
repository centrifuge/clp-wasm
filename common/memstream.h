#pragma once
#include <algorithm>
#include <istream>
#include <locale>
#include <streambuf>

struct Membuf : std::streambuf
{
    Membuf(char const * base, const size_t size)
    {
        char * p(const_cast<char *>(base));
        this->setg(p, p, p + size);
    }
    ~Membuf() override = default;
};
struct Imemstream : virtual Membuf, std::istream
{
    Imemstream(char const * base, const size_t size)
    : Membuf(base, size)
    , std::istream(static_cast<std::streambuf *>(this))
    {
    }
};

inline void ltrim(std::string & s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
}

inline void rtrim(std::string & s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
}

inline void trim(std::string & s)
{
    ltrim(s);
    rtrim(s);
}

inline std::string toLower(std::string data)
{
    std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c) { return std::tolower(c); });
    return data;
}
