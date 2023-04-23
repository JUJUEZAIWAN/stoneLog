# stoneLog 
[中文](README.MD)

stonelog is a simple and high-performance C++ log library that references the muduo log library and spdlog log, supports stream input and formatted input, local, network and other multi-output.

Refer to the muduo log library and spdlog log implementation, each line of the log is a temporary object, and the log information is written into the buffer through the stream input and formatted input, and then the log information is written to the local file or network file through the callback static function.

The asynchronous implementation refers to the double buffer in muduo, and the log information is written to the local file or network file through a background thread. The thread local buffer is used between the front end and the back end, and the front end writes multiple log information to the thread local buffer. When the thread local buffer is full, the log information in the buffer is written to the back end buffer, which can reduce the competition of locks and improve performance.

## features
·stream input and formatted input  
·synchronous/asynchronous log  
·local/remote log  
·log file rolling  
·output is easy to expand  

## dependency
·C++17  
·fmtlib (included)  


## 使用

### 同步日志

```c++
int main()
{
    string_view basename = "test.log";  // log file name
    int rollSize = 1024 * 1024*100;  // 100M

    SyncLogConfig<LogFile> g_logfile(std::make_unique<LogFile>(basename, rollSize));  // set local log file
    //string_view ip = "";  // test server ip
    //int port = 0;       // test server port
    //SyncLogConfig<TcpFile> g_logfile(std::make_unique<TcpFile>(ip,port));
    //SyncLogConfig<UdpFile> g_logfile(std::make_unique<UdpFile>(ip,port));
    g_logfile.setOutputAndFlush();      // set output and flush
    Logger::setLogLevel(Logger::TRACE); // set log level

}
```

### 异步日志

```c++
int main()
{
    string_view basename = "test.log";  // log file name
    int rollSize = 1024 * 1024*100;  // 100M

    AsyncLogConfig<LogFile> g_logfile(std::make_unique<LogFile>(basename, rollSize));  // set local log file

    //string_view ip = "";  // test server ip
    //int port = 0;       // test server port
    //AsyncLogConfig<TcpFile> g_logfile(std::make_unique<TcpFile>(ip,port));
    //AsyncLogConfig<UdpFile> g_logfile(std::make_unique<UdpFile>(ip,port));  
    g_logfile.setOutputAndFlush();      // set output and flush
    Logger::setLogLevel(Logger::TRACE); // set log level

}
```

### stream input

```c++
#include "LogFile.h"
#include "LogConfig.h"

void test()
{
    string s1("hello world", 100);
    int num = 1000000;
    char c = 'a';
    double d = 1.234;
    bool b = true;
    const char *s = "C++";
    for (int i = 0; i < 100000; i++)
    {
        LOG_INFO<<s1<<num<<c<<d<<b<<s;
        LOG_DEBUG<<s1<<num<<c<<d<<b<<s;
        LOG_TRACE<<s1<<num<<c<<d<<b<<s;
        LOG_WARN<<s1<<num<<c<<d<<b<<s;
        LOG_ERROR<<s1<<num<<c<<d<<b<<s;
    }
}
```

### fomatted input

```c++
#include "LogFile.h"
#include "LogConfig.h"
void testFmt()
{
    string s1("hello world", 100);
    int num = 1000000;
    char c = 'a';
    double d = 1.234;
    bool b = true;
    const char *s = "C++";
    for (int i = 0; i < 100000; i++)
    {
        LOG_FMT_INFO("{}{}{}{}{}{}", s1, num, c, d, b, s);
        LOG_FMT_DEBUG("{}{}{}{}{}{}", s1, num, c, d, b, s);
        LOG_FMT_TRACE("{}{}{}{}{}{}", s1, num, c, d, b, s);
        LOG_FMT_WARN("{}{}{}{}{}{}", s1, num, c, d, b, s);
        LOG_FMT_ERROR("{}{}{}{}{}{}", s1, num, c, d, b, s);
    }

}
```


## TODO

·Cross-platform: Currently only supports linux, and will support windows later  
·Dynamic configuration of logs: Currently only supports compile-time configuration, and will support runtime configuration later  
·Performance test: Currently only a simple test, and will be tested in more detail later  
·Support more outputs: Currently supports local files, TCP, UDP, and will support more outputs later  
