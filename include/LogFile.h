#ifndef STONE_LOGFILE_H
#define STONE_LOGFILE_H

 

#include <string>
#include <string_view>
#include <memory>
#include <mutex>

namespace stone
{
    using std::string;
    using std::string_view;

    class FileAppender
    {
    public:
        FileAppender(string_view filename);
        FileAppender(const char *filename);
        ~FileAppender();

        void append(const char *line, size_t len);

        void flush();

        off_t writtenBytes() const
        {
            return writtenBytes_;
        }

    private:
        size_t write(const char *line, size_t len);

    private:
        off_t writtenBytes_;
        FILE *fp_;
        static constexpr int kAppendBufferSize = 64 * 1024;
        std::array<char, kAppendBufferSize> appendBuffer_;
    };

    class LogFile
    {
    public:
        LogFile(string_view basename, off_t rollSize, int flushInterval = 3, int checkEveryN = 1024);
        ~LogFile();

        void append(const char *logline, int len);
        void flush();
        bool rollFile();
        static string getLogFileName(string_view basename, time_t &now);
        static string_view filePath;


    private:
        string_view basename_;
        const off_t rollSize_;
        std::unique_ptr<FileAppender> appender_;
        time_t lastRoll_;
        time_t lastFlush_;

        const int flushInterval_;
        const int checkEveryN_;
        int count_;
        time_t startOfPeriod_;
        const static int kRollPerSeconds_ = 60 * 60 * 24;
    };
}

#endif