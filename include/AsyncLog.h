#ifndef STONE_ASYNCLOG_H
#define STONE_ASYNCLOG_H

#include "LogFile.h"
#include "LogStream.h"
#include "LogBuffer.h"

#include <vector>
#include <memory>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <atomic>
#include <string>
#include <string_view>
#include <cassert>


#define FMT_HEADER_ONLY
#include "fmt/format.h"

namespace stone
{

    // use template to support different type of output
    // output can be file, stdout, socket, etc.
    // output class must have a method named append and flush (flush can be empty,but must be exist)
    template <typename OutPut>
    class AsyncLog
    {

        using Buffer_t = bigBuffer;
        using PtrBuf = std::unique_ptr<Buffer_t>;

    public:
        AsyncLog(std::unique_ptr<OutPut> &&output, int flushInterval = 3)
            : running_(false), flushInterval_(flushInterval),
              currentBuf_(new Buffer_t), nextBuf_(new Buffer_t),
              outPut_(std::move(output)) 
        {
            currentBuf_->clear();
            nextBuf_->clear();
            buffers_.reserve(16);
        }

        ~AsyncLog()
        {
            if (running_)
            {
                stop();
            }
        }

    public:
        // append a line to buffer
        void append(const char *line, size_t len)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (currentBuf_->avail() > len)
            {
                currentBuf_->append(line, len);
            }
            else
            {
                // current buffer is full, move it to buffers_ and use nextBuf_ as current buffer
                buffers_.push_back(std::move(currentBuf_));
                currentBuf_ = nextBuf_ ? std::move(nextBuf_) : std::make_unique<Buffer_t>();
                currentBuf_->append(line, len);
                cond_.notify_one();
            }
        }

        void start()
        {
            running_ = true;

            thread_ = std::thread(
                [this]()
                {
                    PtrBuf newBuf1 = std::make_unique<Buffer_t>();
                    PtrBuf newBuf2 = std::make_unique<Buffer_t>();
                    newBuf1->clear();
                    newBuf2->clear();
                    std::vector<PtrBuf> buffers;
                    buffers.reserve(16);  // 16 is a magic number, just for performance

                    while (running_)
                    {
                        assert(buffers.empty());
                        // if buffers_ is empty, wait for cond_ notify or timeout
                        {
                            std::unique_lock<std::mutex> lock(mutex_);
                            if (buffers_.empty())
                            {

                                cond_.wait_for(lock, std::chrono::seconds(flushInterval_));
                            }

                            // swap buffers_ and buffers
                            buffers_.push_back(std::move(currentBuf_));

                            currentBuf_ = std::move(newBuf1);

                            buffers.swap(buffers_);

                            if (!nextBuf_)
                            {
                                nextBuf_ = std::move(newBuf2);
                            }
                        }
                       

                        assert(!buffers.empty());

                        if (buffers.size() > 25)
                        {

                            char buferror[256] = {0};
                            
                            
                            auto result = fmt::format_to_n(buferror, sizeof(buferror), "Dropped log messages at {}, {} larger buffers\n",
                                            "time ",
                                            buffers.size() - 2);
                            
                            fputs(buferror, stderr);
                    
                            outPut_->append(buferror, result.size);
                            buffers.erase(buffers.begin() + 2, buffers.end());
                        }

                        // write buffers to file,blocks until file write completes
                        // if write failed, the error should be handled by the output class
                        for (auto &buf : buffers)
                        {
                            outPut_->append(buf->begin(), buf->length());
                        }

                        // if buffers size is greater than 2, drop some buffers
                        if (buffers.size() > 2)
                        {
                            buffers.resize(2);  
                        }

                        // if newBuf1 or newBuf2 is empty, use the last buffer in buffers as newBuf1 or newBuf2
                        if (!newBuf1)
                        {
                            newBuf1 = std::move(buffers.back());
                            buffers.pop_back();
                            newBuf1->reset();
                        }

                        if (!newBuf2)
                        {
                            newBuf2 = std::move(buffers.back());
                            buffers.pop_back();
                            newBuf2->reset();
                        }

                        outPut_->flush();
                    }
                    //
                    outPut_->flush();
                });
        }

        void stop()
        {
            running_ = false;
            cond_.notify_all();
            thread_.join();
        }

    private:
        std::vector<PtrBuf> buffers_;
        std::mutex mutex_;
        std::condition_variable cond_;
        std::thread thread_;
        std::atomic_bool running_;
        int flushInterval_;
        off_t rollSize_;
        PtrBuf currentBuf_;
        PtrBuf nextBuf_;
        std::unique_ptr<OutPut> outPut_;
    };
}

#endif