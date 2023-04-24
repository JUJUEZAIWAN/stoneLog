#ifndef STONE_UDPFILE_H
#define STONE_UDPFILE_H

 
#include <string_view>
#include <array>
#include <memory>

#ifdef  _WIN32
#include <winsock2.h>

#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

namespace stone
{
    class UdpAppender
    {
    public:
        UdpAppender(std::string_view ip, int port);
        ~UdpAppender();

        void append(const char *line, size_t len);

        void flush();

    private:
        size_t write(const char *line, size_t len);

    private:
        struct sockaddr_in addr_;
        int sockfd_;
        static constexpr int kAppendBufferSize = 64 * 1024;
        std::array<char, kAppendBufferSize> appendBuffer_;
    };

    class UdpFile
    {
    public:
        UdpFile(std::string_view ip, int port);
        ~UdpFile();

        void append(const char *logline, int len);
        void flush();

    private:
        std::unique_ptr<UdpAppender> appender_;
    };

}

#endif