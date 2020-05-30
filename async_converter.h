#pragma once

#include <thread>
#include <condition_variable>
#include <deque>
#include <functional>

#include "PigLatinConverter.h"

class async_converter {
 public:
    explicit async_converter(std::function<void(std::string const& original, std::string const& converted)> callback);
    ~async_converter();
    void convert(std::string original);
    void cancel();
 private:
    std::mutex mutex_;
    std::condition_variable cv_;
    std::thread thread_;
    bool done = false;
    std::deque<std::string> inbox_;
    PigLatinConverter converter_;
};
