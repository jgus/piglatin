#include "async_converter.h"

#include <cassert>

async_converter::async_converter(std::function<void(std::string const& original,
                                                    std::string const& converted)> callback) : thread_([this, callback =
std::move(callback)]() {
    while (true) {
        std::string original;
        {
            std::unique_lock lock{mutex_};
            cv_.wait(lock, [this] { return done || !inbox_.empty(); });
            if (done) return;
            assert(!inbox_.empty());
            original = std::move(inbox_.front());
            inbox_.pop_front();
            cv_.notify_all();
        }
        auto converted = converter_.convert(original);
        callback(original, converted);
    }
}) {}

async_converter::~async_converter() {
    {
        std::unique_lock lock{mutex_};
        cv_.wait(lock, [this] { return done || inbox_.empty(); });
        done = true;
        cv_.notify_all();
    }
    thread_.join();
}

void async_converter::convert(std::string original) {
    std::lock_guard lock{mutex_};
    if (done) return;
    inbox_.push_back(std::move(original));
    cv_.notify_all();
}

void async_converter::cancel() {
    std::lock_guard lock{mutex_};
    done = true;
    converter_.cancel();
    cv_.notify_all();
}