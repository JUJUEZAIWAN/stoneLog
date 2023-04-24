#include "LogStream.h"
#include <cstring>


using namespace stone;

LogStream &LogStream::operator<<(char c)
{
    buf_.append(&c, 1);
    return *this;
}

LogStream &LogStream::operator<<(const char *s)
{
    return operator<<(std::string_view(s));
}

LogStream &LogStream::operator<<(bool boolean)
{
    buf_.append(boolean ? "1" : "0", 1);
    return *this;
}

LogStream &LogStream::operator<<(const unsigned char *s)
{
    return operator<<(std::string_view(reinterpret_cast<const char *>(s)));
}

void LogStream::append(const char *line, int len)
{
    buf_.append(line, len);
}

LogStream &LogStream::operator<<(std::string_view s)
{
    buf_.append(s.data(), s.length());
    return *this;
}