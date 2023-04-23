#include <cstdio>
#include "TcpFile.h"
#include "LogConfig.h"
#include <string_view>
#include <chrono>
#include <thread>
#include <string>
#include <iostream>
#include <mutex>
using namespace stone;
using namespace std;

string s1("hello world", 100);
int num = 1000000;
char c = 'a';
double d = 1.234;
bool b = true;
const char *s = "C++";

std::mutex g_mutex;


void test()
{
    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < 100000; i++)
    {
        LOG_INFO<<s1<<num<<c<<d<<b<<s;
        LOG_DEBUG<<s1<<num<<c<<d<<b<<s;
        LOG_TRACE<<s1<<num<<c<<d<<b<<s;
        LOG_WARN<<s1<<num<<c<<d<<b<<s;
        LOG_ERROR<<s1<<num<<c<<d<<b<<s;
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end - start;
    lock_guard<mutex> lock(g_mutex);
    std::cout << this_thread::get_id()<< "  time: " << diff.count() << "s" << std::endl;
}

void testFmt()
{
    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < 100000; i++)
    {
        LOG_FMT_INFO("{}{}{}{}{}{}", s1, num, c, d, b, s);
        LOG_FMT_DEBUG("{}{}{}{}{}{}", s1, num, c, d, b, s);
        LOG_FMT_TRACE("{}{}{}{}{}{}", s1, num, c, d, b, s);
        LOG_FMT_WARN("{}{}{}{}{}{}", s1, num, c, d, b, s);
        LOG_FMT_ERROR("{}{}{}{}{}{}", s1, num, c, d, b, s);
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end - start;
    lock_guard<mutex> lock(g_mutex);
    std::cout << this_thread::get_id()<< "  time: " << diff.count() << "s" << std::endl;
}


int main()
{
    string_view ip = "";  // test server ip
    int port = 0;

    SyncLogConfig<TcpFile> g_logfile(std::make_unique<TcpFile>(ip,port));
    g_logfile.setOutputAndFlush();
    Logger::setLogLevel(Logger::TRACE);
    vector<thread> threads;
    for(int i = 0; i < 10; i++)
    {
        threads.emplace_back(test);
        
    }
    for(int i = 0; i < 10; i++)
    {
        threads[i].join();
    }

    for(int i = 0; i < 10; i++)
    {
        threads[i] = thread(testFmt);
    }

    for(int i = 0; i < 10; i++)
    {
        threads[i].join();
    }





}