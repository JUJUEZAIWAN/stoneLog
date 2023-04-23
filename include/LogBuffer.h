#ifndef STONE_LOGBUFFER_H
#define STONE_LOGBUFFER_H

#include "deleteCopy.h"
#include <string>
#include <algorithm>
#include <array>
#include <cstring>

namespace stone
{
    using std::string;

    template <size_t N>
    class LogBuffer
    {

    public:
        LogBuffer() : currentiter_(buf_.begin())
        {
        }
        ~LogBuffer() = default;
        DISABLE_COPY(LogBuffer)



    public:
        size_t avail()
        {
            return buf_.size() - length();
        }

        void append(const char *s, size_t len)
        {
            if (len <= avail())
            {
                std::copy(s, s + len, currentiter_);
                currentiter_ += len;
            }
        }

        void reset()
        {
            currentiter_ = buf_.begin();
        }

        size_t length()
        {
            return currentiter_ - buf_.begin();
        }

        void clear()
        {
            std::fill(buf_.begin(), buf_.end(), 0);
            currentiter_ = buf_.begin();
        }

        string toString()
        {
            return string(buf_.begin(), currentiter_);
        }

        inline constexpr typename std::array<char, N>::iterator begin()
        {
            return buf_.begin();
        }

        inline constexpr typename std::array<char, N>::iterator end()
        {
            return buf_.end();
        }

        inline constexpr typename std::array<char, N>::iterator current()
        {
            return currentiter_;
        }

        void addLength(size_t len)
        {
            currentiter_ += len;
        }

    private:
        std::array<char, N> buf_;
        typename std::array<char, N>::iterator currentiter_;
        
    };

    constexpr size_t kSmallBuffer = 1024 * 4;   // 4k ,a line of log
    constexpr size_t kMidBuffer = kSmallBuffer * 32; // 128k , cache current thread's log
    constexpr size_t kBigBuffer = 1024 * 1024 * 8;  // 8M , backgroud buffer ,accept log from other thread

    using smallBuffer = LogBuffer<kSmallBuffer>;
    using midBuffer = LogBuffer<kMidBuffer>;
    using bigBuffer = LogBuffer<kBigBuffer>;
}

#endif