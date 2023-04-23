#ifndef STONE_LOGCONFIG_H
#define STONE_LOGCONFIG_H

#include "AsyncLog.h"
#include "Logger.h"
#include <iostream>
#include <memory>
namespace stone
{

    
    // SyncLogConfig--->File[LogFile/TcpFile/UdpFile]--->Appender[LogFileAppender/TcpAppender/UdpAppender]
    template <typename T>
    class SyncLogConfig
    {
    public:
        

    
        SyncLogConfig(std::unique_ptr<T> &&output)
            : output_(std::move(output))
        {
            
        }

        void setOutputAndFlush()
        {
            Logger::setOutput([this](const char *msg, int len)
                              { output_->append(msg, len); });
            Logger::setFlush([this]()
                             { output_->flush(); });
        }

        ~SyncLogConfig() = default;

        DISABLE_COPY(SyncLogConfig)

    private:
        std::unique_ptr<T> output_;
    };


    // AsyncLogConfig--->AsyncLog--->File[LogFile/TcpFile/UdpFile]--->Appender[LogFileAppender/TcpAppender/UdpAppender]
    template <typename T>
    class AsyncLogConfig
    {
    public:
       
        AsyncLogConfig(std::unique_ptr<T> &&output)
            : asyncLog_(std::move(output))
        {
            
        }

        void setOutputAndFlush()
        {
            Logger::setOutput([this](const char *msg, int len)
                              { asyncLog_.append(msg, len); });
            
                            
            asyncLog_.start();
        }

        ~AsyncLogConfig()
        {
            asyncLog_.stop();
        }

        DISABLE_COPY(AsyncLogConfig)

    private:
        AsyncLog<T> asyncLog_;
    };

}

#endif