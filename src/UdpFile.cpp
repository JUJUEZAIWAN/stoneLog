#include "UdpFile.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <cstdlib>

using namespace stone;

UdpAppender::UdpAppender(std::string_view ip, int port) : sockfd_(-1)
{
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = inet_addr(ip.data());
    sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd_ < 0)
    {
        abort();
    }
}

UdpAppender::~UdpAppender()
{
    if (sockfd_ != -1)
    {
        ::close(sockfd_);
    }
}

void UdpAppender::append(const char *line, size_t len)
{
    write(line, len);
}

void UdpAppender::flush()
{
    // do nothing
}

size_t UdpAppender::write(const char *line, size_t len)
{
    size_t n = 0;
    size_t remain = len;
    bool resend = false;
    while (remain > 0)
    {
        n = ::sendto(sockfd_, line, remain, 0, (sockaddr *)&addr_, sizeof(addr_));
        if (n <= 0) // send failed
        {
            if(resend)
            {
                abort();
            }
            else
            {
                resend = true;
                ::close(sockfd_);
                sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
                if (sockfd_ < 0)
                {
                    abort();
                }
                continue;
            }
        }
        remain -= n;
        line += n;
    }
}


UdpFile::UdpFile(std::string_view ip, int port)
{
    appender_ = std::make_unique<UdpAppender>(ip, port);
}

UdpFile::~UdpFile()
{
}

void UdpFile::append(const char *logline, int len)
{
    appender_->append(logline, len);
}

void UdpFile::flush()
{
    appender_->flush();
}
