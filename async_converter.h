#pragma once

#include <thread>
#include <condition_variable>
#include <deque>
#include <functional>

#include "PigLatinConverter.h"

// Worker class encapsulating asynchronous conversion of strings to pig latin
class async_converter {
 public:
    // Create a converter, and launch its worker thread. The given callback will be invoked on completion of each successful conversion
    explicit async_converter(std::function<void(std::string const& original, std::string const& converted)> callback);

    // The destructor will block until the worker thread completes all currently-queued conversions, without canceling them. (That is, destruction performs a clean, non-aborted shutdown.)
    ~async_converter();

    // Enqueue a given string for conversion. If cancel() has been called previously, this does nothing.
    void convert(std::string original);

    // Cancel any in-flight conversions, and ignore any future calls to convert()
    void cancel();

 private:
    std::mutex mutex_;
    std::condition_variable cv_;
    // The following members are thread sensitive. They must only be accessed while holding a lock on mutex_, and cv_ must be notified when their values change
    bool canceled_ = false;
    bool done_ = false;
    std::deque<std::string> inbox_;
    // End of thread-sensitive state

    std::thread thread_;
    PigLatinConverter converter_;
};
