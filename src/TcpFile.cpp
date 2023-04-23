#include "TcpFile.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>

#define close closesocket

#elif __linux__
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/fcntl.h>
#include <netinet/tcp.h>
#endif

#include <cstdlib>

using namespace stone;

// The TcpFile and TcpAppender is used to write log to a remote server in Intranet (not Internet!!!).

TcpAppender::TcpAppender(std::string_view ip, int port)
    : sockfd_(-1), ip_(ip), port_(port)
{

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        return false;
    }
#endif
    // if connect failed, abort,it should not happen in Intranet
    if (!connect())
    {
        abort();
    }
}

TcpAppender::~TcpAppender()
{
    if (sockfd_ != -1)
    {
        ::close(sockfd_);
    }
}

bool TcpAppender::connect()
{
    if (sockfd_ != -1)
    {
        ::close(sockfd_);
    }

    sockfd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = ::htons(port_);
    addr.sin_addr.s_addr = ::inet_addr(ip_.data());
    int ret = ::connect(sockfd_, (sockaddr *)&addr, sizeof(addr));
    if (ret < 0)
    {
        return false;
    }

    // set no nagle algorithm
    int opt = 1;
    if (::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) == -1)
    {
        return false;
    }
    // set keepalive
    if (::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt)) == -1)
    {
        return false;
    }

    // set send timeout
    struct timeval tv;
    tv.tv_sec = 0;  
    tv.tv_usec = 500;  // 500ms, because use it in Intranet, so 500ms is enough 
    if (::setsockopt(sockfd_, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) == -1)
    {
        return false;
    }

    return true;
}

void TcpAppender::append(const char *line, size_t len)
{
    size_t n = 0;
    size_t remain = len;
    while (remain > 0)
    {
        n = ::send(sockfd_, line, remain, 0);
        if (n <= 0)  // send failed
        {
            if (!connect()) // reconnect
            {
                abort();
            }
            continue;
        }
        remain -= n;
        line += n;
    }
}

void TcpAppender::flush()
{
    // do nothing
}

size_t TcpAppender::write(const char *line, size_t len)
{
    return ::write(sockfd_, line, len);
}

TcpFile::TcpFile(std::string_view ip, int port)
{
    appender_ = std::make_unique<TcpAppender>(ip, port);
}

TcpFile::~TcpFile()
{
}

void TcpFile::append(const char *logline, int len)
{
    appender_->append(logline, len);
}

void TcpFile::flush()
{
    appender_->flush();
}
