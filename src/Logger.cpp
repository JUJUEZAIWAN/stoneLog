#include "Logger.h"

#include <thread>
#include <string>
#include <string_view>
#include <sstream>
#include <chrono>

// thread_loacl variables
thread_local char t_time[64] = {0};
thread_local time_t t_last_sec = 0;
thread_local time_t t_last_output = 0;

// using the midBuffer to store this thread's log, decrease the lock time
thread_local std::unique_ptr<stone::midBuffer> t_localBuffer = std::make_unique<stone::midBuffer>();

namespace stone
{
    using std::chrono::system_clock;

    Logger::LogLevel initLogLevel()
    {
        if (::getenv("LOG_TRACE"))
            return Logger::TRACE;
        else if (::getenv("LOG_DEBUG"))
            return Logger::DEBUG;
        else
            return Logger::INFO;
    }

    // default output function, can be changed by Logger::setOutput()
    void defaultOutput(const char *line, int len)
    {
        size_t n = fwrite(line, 1, len, stdout);
    }

    // default flush function, can be changed by Logger::setFlush()
    void defaultFlush()
    {
        fflush(stdout);
    }

    Logger::LogLevel g_logLevel = initLogLevel();
    Logger::OutputFunc g_output = defaultOutput;
    Logger::FlushFunc g_flush = defaultFlush;

    void Logger::setLogLevel(LogLevel level)
    {
        g_logLevel = level;
    }

    void Logger::setOutput(OutputFunc func)
    {
        g_output = func;
    }
    void Logger::setFlush(FlushFunc func)
    {
        g_flush = func;
    }

    constexpr const char *LogLevelName[Logger::NUM_LOG_LEVELS] = {
        "TRACE ",
        "DEBUG ",
        "INFO  ",
        "WARN  ",
        "ERROR ",
        "FATAL ",
    };

    Logger::Logger(SourceFile filename, int line)
        : impl_(LogLevel::INFO, filename, line, 0)
    {
    }
    Logger::Logger(SourceFile filename, int line, LogLevel level)
        : impl_(level, filename, line, 0)
    {
    }
    Logger::Logger(SourceFile filename, int line, LogLevel level, const char *func)
        : impl_(level, filename, line, 0)
    {
        impl_.stream_ << func << ' ';
    }
    Logger::Logger(SourceFile filename, int line, bool toAbort)
        : impl_(toAbort ? FATAL : ERROR, filename, line, errno)
    {
    }

    // when the Logger object is destructed, the log will be cached in t_localBuffer
    // if the t_localBuffer is full or 1 sec passed , the log will be flushed to g_output
    Logger::~Logger()
    {
        impl_.finish();
        smallBuffer &buffer(stream().buffer());

        auto now_c = system_clock::to_time_t(system_clock::now());

        // cache this thread's log in t_midBuffer, and flush it to g_output when it's full or 1 sec passed
        if (t_localBuffer->avail() > buffer.length() && now_c == t_last_output)
        {
            t_localBuffer->append(buffer.begin(), buffer.length());
        }
        else
        {
            // flush the t_localBuffer to g_output
            g_output(t_localBuffer->begin(), t_localBuffer->length());
            t_localBuffer->reset();
            t_localBuffer->append(buffer.begin(), buffer.length());
        }

        if (impl_.level_ == FATAL)
        {
            g_output(t_localBuffer->begin(), t_localBuffer->length());
            g_flush();
            abort();
        }
    }

    LogStream &Logger::stream()
    {
        return impl_.stream_;
    }



    Logger::Impl::Impl(LogLevel level, const SourceFile &filename, int line, int old_errno)
        : level_(level), filename_(filename), line_(line), stream_()
    {
        formatTime();
        std::ostringstream ss;
        ss << std::this_thread::get_id();
        stream_ << std::string_view(ss.str()) << ' ';
        stream_ << std::string_view(LogLevelName[level]) << ": ";
        stream_ << filename_.filename() << ':' << line << ' ';

        if (!old_errno)
        {
            // TODO deal with errno
        }
    }



    void Logger::Impl::finish()
    {
        stream_ << '\n';
    }



    // format the time to "YYYY-MM-DD HH:MM:SS.microseconds"
    // if the time is not the same second, reformat the time
    // else just format the microseconds
    void Logger::Impl::formatTime()
    {
        using microseconds = std::chrono::microseconds;
        using system_clock = std::chrono::system_clock;

        auto now = system_clock::now();
        time_t now_c = system_clock::to_time_t(now);
        int microSeconds = std::chrono::duration_cast<microseconds>(now.time_since_epoch()).count() % 1000000;

        // if the time is not the same second, update the time
        if (now_c != t_last_sec)
        {
            t_last_sec = now_c;
            auto &[second, minute, hour, day, month, year, _1, _2, _3, _4, _5] = *std::localtime(&now_c);

            fmt::format_to(std::begin(t_time), "{:4d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}", year + 1900, month + 1, day, hour, minute, second);
        }
        stream_ << std::string_view(t_time) << '.' << microSeconds << ' ';
    }
}