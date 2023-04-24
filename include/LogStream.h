#ifndef STONE_LOGSTREAM_H
#define STONE_LOGSTREAM_H

#include "LogBuffer.h"
#include "deleteCopy.h"

#include <string>
#include <type_traits>
#include <concepts>
#include <string_view>
#include <array>

#define FMT_HEADER_ONLY
#include "fmt/format.h"

namespace stone
{
    class LogStream
    {
    public:
        LogStream() = default;
        ~LogStream() = default;

        DISABLE_COPY(LogStream)

        template <typename T, typename = std::enable_if_t<std::is_integral<T>::value || std::is_floating_point<T>::value>>
        inline size_t formatBaseT(T t, const char *buf) noexcept
        {
            auto ret = fmt::format_to_n(const_cast<char *>(buf), kMaxNumericSize, "{}", t);
            return ret.size;
        }

        template <typename T, typename = std::enable_if_t<std::is_integral<T>::value || std::is_floating_point<T>::value>>
        inline LogStream &operator<<(T t)
        {

            if (buf_.avail() > kMaxNumericSize)
            {
                size_t len = formatBaseT(t, buf_.current());
                buf_.addLength(len);
            }

            return *this;
        }

        LogStream &operator<<(std::string_view s);
        LogStream &operator<<(char c);
        LogStream &operator<<(const char *s);
        LogStream &operator<<(const unsigned char *s);
        LogStream &operator<<(bool boolean);

        template <typename... Args>
        void fmt(const std::string_view &fmt, Args &&...args)
        {
            string data = fmt::vformat(fmt.data(), fmt::make_format_args(args...));
            buf_.append(data.data(), data.length());
        }

        void append(const char *line, int len);

        smallBuffer &buffer()
        {
            return buf_;
        }

    private:
        smallBuffer buf_;
        static constexpr int kMaxNumericSize = 48;
    };
}

#endif