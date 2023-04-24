# stoneLog 
[English](README_EN.MD)

stonelog 是一个简易的高性能跨平台的C++日志库。
参考muduo日志库和spdlog日志，支持流式输入和格式化输入，本地，网络等多种输出。

每一行日志都是一个临时对象，支持流式输入和格式化输入，将日志信息写入到缓冲区，然后通过回调静态函数，将日志信息同步或者异步的写入到本地文件或者网络文件中。  

异步实现参考muduo中的双缓冲区，通过一个后台线程将日志信息写入到本地文件或者网络文件中。在前端和后端之间使用了thread local 的缓冲区，前端将多条日志信息写入到thread local缓冲区，定时刷新或者thread local缓冲区满时，将缓冲区的日志信息写入到后端的缓冲区，可以减少锁的竞争，提高性能。  

输出易于扩展，封装了AsyncLogConfig和SyncLogConfig两个类，分别对应异步和同步日志，通过模板参数指定日志输出的类型，目前支持本地文件，TCP，UDP，后续会支持更多输出。通过setOutputAndFlush函数设置日志输出和刷新，通过setLogLevel函数设置日志级别。  
可以自定义输出类型，只要实现append函数和flush函数即可。




## 特性

- [ ]流式输入和格式化输入  
- [ ]同步/异步日志  
- [ ]本地/远程日志  
- [ ]日志文件滚动  
- [ ]输出易于扩展  
- [ ]跨平台


## 依赖

·C++17   
·fmtlib (已包含)

## 使用

### 同步日志

```c++
int main()
{
    string_view basename = "test.log";  // 日志文件名
    int rollSize = 1024 * 1024*100;  // 100M

    SyncLogConfig<LogFile> g_logfile(std::make_unique<LogFile>(basename, rollSize));  // 设置本地日志文件
    //string_view ip = "";  // test server ip
    //int port = 0;       // test server port
    //SyncLogConfig<TcpFile> g_logfile(std::make_unique<TcpFile>(ip,port));
    //SyncLogConfig<UdpFile> g_logfile(std::make_unique<UdpFile>(ip,port));
    g_logfile.setOutputAndFlush();      // 设置输出和刷新
    Logger::setLogLevel(Logger::TRACE); // 设置日志级别

}
```

### 异步日志

```c++
int main()
{
    string_view basename = "test.log";  // 日志文件名
    int rollSize = 1024 * 1024*100;  // 100M

    AsyncLogConfig<LogFile> g_logfile(std::make_unique<LogFile>(basename, rollSize));  // 设置本地日志文件

    //string_view ip = "";  // test server ip
    //int port = 0;       // test server port
    //AsyncLogConfig<TcpFile> g_logfile(std::make_unique<TcpFile>(ip,port));
    //AsyncLogConfig<UdpFile> g_logfile(std::make_unique<UdpFile>(ip,port));
    g_logfile.setOutputAndFlush();      // 设置输出和刷新
    Logger::setLogLevel(Logger::TRACE); // 设置日志级别

}
```

### 流式输入

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

### 格式化输入 （依赖 fmtlib，代码已包含）

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

- [ ]日志动态配置：目前只支持编译时配置，后续会支持运行时配置  
- [ ]性能测试：目前只是简单的测试，后续会进行更加详细的测试  
- [ ]支持更多输出：目前支持本地文件，TCP，UDP，后续会支持更多输出  
