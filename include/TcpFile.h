#ifndef STONE_TCPFILE_H
#define STONE_TCPFILE_H

#include <string_view>
#include <memory>

namespace stone
{
    class TcpAppender
    {
    public:
        TcpAppender(std::string_view ip, int port);
        ~TcpAppender();

        void append(const char *line, size_t len);

        void flush();

    private:
        size_t write(const char *line, size_t len);
        bool connect();

    private:
        int sockfd_;
        std::string_view ip_;
        int port_;
        static constexpr int kAppendBufferSize = 64 * 1024;
        std::array<char, kAppendBufferSize> appendBuffer_;
    };

    class TcpFile
    {
    public:
        TcpFile(std::string_view ip, int port);
        ~TcpFile();

        void append(const char *logline, int len);
        void flush();

    private:
    private:
        std::unique_ptr<TcpAppender> appender_;
    };
}

#endif
