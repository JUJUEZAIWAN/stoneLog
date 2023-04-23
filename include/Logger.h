#ifndef STONE_LOGGER_H
#define STONE_LOGGER_H
#include "LogStream.h"
#include <functional>

namespace stone
{

    class Logger
    {
    public:

        enum LogLevel
        {
            TRACE,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL,
            NUM_LOG_LEVELS
        };


        
        class SourceFile
        {
        public:
            constexpr SourceFile(std::string_view filename) : filename_(filename)
            {
                auto slash = filename_.rfind('/');
                if (slash != std::string_view::npos)
                {
                    filename_ = filename_.substr(slash + 1);
                }
            }

            constexpr std::string_view filename()
            {
                return filename_;
            }

        private:
            std::string_view filename_;
        };

        Logger(SourceFile filename, int line);
        Logger(SourceFile filename, int line, LogLevel level);
        Logger(SourceFile filename, int line, LogLevel level, const char *func);
        Logger(SourceFile filename, int line, bool toAbort);
        ~Logger();

        LogStream &stream();

        using OutputFunc = std::function<void(const char *line, int len)>;
        using FlushFunc = std::function<void()>;
        static LogLevel logLevel();
        // static LogLevel g_logLevel;
        static void setLogLevel(LogLevel level);
        static void setOutput(OutputFunc);
        static void setFlush(FlushFunc);




        

    private:
        
        class Impl
        {
        public:
            using LogLevel = Logger::LogLevel;
            Impl(LogLevel level, const SourceFile &filename, int line, int old_errno);
            void formatTime();
            void finish();

            LogStream stream_;
            LogLevel level_;
            int line_;
            SourceFile filename_;
        };

        Impl impl_;
    };

    extern Logger::LogLevel g_logLevel;

    inline Logger::LogLevel Logger::logLevel()
    {
        return g_logLevel;
    }

// stream style log

using sourcefile = Logger::SourceFile;

#define LOG_TRACE                                          \
    if (stone::Logger::logLevel() <= stone::Logger::TRACE) \
    stone::Logger(sourcefile(__FILE__) , __LINE__, stone::Logger::TRACE, __func__).stream()
#define LOG_DEBUG                                          \
    if (stone::Logger::logLevel() <= stone::Logger::DEBUG) \
    stone::Logger(sourcefile(__FILE__), __LINE__, stone::Logger::DEBUG, __func__).stream()
#define LOG_INFO                                          \
    if (stone::Logger::logLevel() <= stone::Logger::INFO) \
    stone::Logger(sourcefile(__FILE__), __LINE__).stream()
#define LOG_WARN stone::Logger(sourcefile(__FILE__), __LINE__, stone::Logger::WARN).stream()
#define LOG_ERROR stone::Logger(sourcefile(__FILE__), __LINE__, stone::Logger::ERROR).stream()
#define LOG_FATAL stone::Logger(sourcefile(__FILE__), __LINE__, stone::Logger::FATAL).stream()
#define LOG_SYSERR stone::Logger(sourcefile(__FILE__), __LINE__, false).stream()
#define LOG_SYSFATAL stone::Logger(sourcefile(__FILE__), __LINE__, true).stream()

// format style log
#define LOG_FMT_TRACE(...)                                 \
    if (stone::Logger::logLevel() <= stone::Logger::TRACE) \
    stone::Logger(sourcefile(__FILE__), __LINE__, stone::Logger::TRACE, __func__).stream().fmt(__VA_ARGS__)
#define LOG_FMT_DEBUG(...)                                 \
    if (stone::Logger::logLevel() <= stone::Logger::DEBUG) \
    stone::Logger(sourcefile(__FILE__), __LINE__, stone::Logger::DEBUG, __func__).stream().fmt(__VA_ARGS__)
#define LOG_FMT_INFO(...)                                 \
    if (stone::Logger::logLevel() <= stone::Logger::INFO) \
    stone::Logger(sourcefile(__FILE__), __LINE__).stream().fmt(__VA_ARGS__)
#define LOG_FMT_WARN(...) stone::Logger(sourcefile(__FILE__), __LINE__, stone::Logger::WARN).stream().fmt(__VA_ARGS__)
#define LOG_FMT_ERROR(...) stone::Logger(sourcefile(__FILE__), __LINE__, stone::Logger::ERROR).stream().fmt(__VA_ARGS__)
#define LOG_FMT_FATAL(...) stone::Logger(sourcefile(__FILE__), __LINE__, stone::Logger::FATAL).stream().fmt(__VA_ARGS__)
#define LOG_FMT_SYSERR(...) stone::Logger(sourcefile(__FILE__), __LINE__, false).stream().fmt(__VA_ARGS__)
#define LOG_FMT_SYSFATAL(...) stone::Logger(sourcefile(__FILE__), __LINE__, true).stream().fmt(__VA_ARGS__)

} // namespace stone

#endif