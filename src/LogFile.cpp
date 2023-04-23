
#include "LogFile.h"
#include <cassert>

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <unistd.h>
#endif

using namespace stone;
using std::chrono::system_clock;

FileAppender::FileAppender(string_view filename)
    : fp_(fopen(filename.data(), "ae")), writtenBytes_(0)
{
    assert(fp_);
    ::setbuffer(fp_, appendBuffer_.data(), appendBuffer_.size());
}
FileAppender::FileAppender(const char *filename)
    : fp_(fopen(filename, "ae")), writtenBytes_(0)
{
    assert(fp_);
    ::setbuffer(fp_, appendBuffer_.data(), appendBuffer_.size());
}
FileAppender::~FileAppender()
{
    flush();
    ::fclose(fp_);
}

size_t FileAppender::write(const char *line, size_t len)
{
    return ::fwrite_unlocked(line, 1, len, fp_);
}

void FileAppender::append(const char *line, size_t len)
{
    size_t written = 0;
    while (written != len)
    {
        size_t remain = len - written;
        size_t write_n = write(line + written, remain);
        if (write_n != remain)
        {
            // TODO 处理error
        }
        written += write_n;
    }
    writtenBytes_ += written;
}

void FileAppender::flush()
{
    ::fflush(fp_);
}

LogFile::LogFile(string_view basename, off_t rollSize,  int flushInterval, int checkEveryN)
    : basename_(basename), rollSize_(rollSize), flushInterval_(flushInterval),
      checkEveryN_(checkEveryN), startOfPeriod_(0), lastRoll_(0), lastFlush_(0)

{
    assert(basename.find('/') == string::npos);
    rollFile();
}

LogFile::~LogFile()
{
}

void LogFile::append(const char *logline, int len)
{

    appender_->append(logline, len);
    // if write bytes > rollSize_ , roll file
    if (appender_->writtenBytes() > rollSize_)
    {
        rollFile();
    }
    else
    {
        ++count_;
        if (count_ >= checkEveryN_)
        {
            count_ = 0;
            time_t now = system_clock::to_time_t(system_clock::now());
            time_t thisPeriod_ = now / kRollPerSeconds_ * kRollPerSeconds_;

            if (thisPeriod_ != startOfPeriod_)
            {
                rollFile();
            }
            else if (now - lastFlush_ > flushInterval_)
            {
                lastFlush_ = now;
                appender_->flush();
            }
        }
    }
}

void LogFile::flush()
{

    appender_->flush();
}

// rool file
bool LogFile::rollFile()
{
    time_t now = 0;
    string filename = filePath.data() + getLogFileName(basename_, now);
    time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;

    if (now > lastRoll_)
    {
        lastRoll_ = now;
        lastFlush_ = now;
        startOfPeriod_ = start;
        appender_ = std::make_unique<FileAppender>(filename);

        return true;
    }
    return false;
}

string LogFile::getLogFileName(std::string_view basename, time_t &now)
{
    string filename;
    filename.reserve(basename.size() + 64);
    filename = basename;
    now = system_clock::to_time_t(system_clock::now());
    char t_time[32] = {0};
    char pidbuf[32] = {0};
    auto &[second, minute, hour, day, month, year, _1, _2, _3, _4, _5] = *std::localtime(&now);

    auto _getpid = []()
    {
#ifdef _WIN32
        return ::GetCurrentProcessId();
#elif __linux__
        return ::getpid();
#endif
    };

    fmt::format_to(std::begin(t_time), "{:4d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}", year + 1900, month + 1, day, hour, minute, second);
    fmt::format_to(std::begin(pidbuf), ".{}", _getpid());

    filename += t_time;

    filename += []() -> string
    {
        char buf[256];
#ifdef _WIN32
        if (::GetComputerNameA(buf, sizeof(buf)))
        {
            buf[sizeof(buf) - 1] = '\0';
            return buf;
        }
#elif __linux__
        if (::gethostname(buf, sizeof(buf)) == 0)
        {
            buf[sizeof(buf) - 1] = '\0';
            return buf;
        }
#endif
        else
        {
            return "unknownhost";
        }
    }();

    filename += pidbuf;
    filename += ".log";

    return filename;
}
