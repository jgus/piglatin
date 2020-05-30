/*
 Notes on thread-sensitive logic:
 inbox_ contains waiting work items - an empty inbox means no more work to do (yet)
 done_ is set when the worker thread should shut down, for any reason - ie, it's a command signal
 canceled_ is set when the converter has been explicitly canceled

 The mutex should never be locked for any significant time - no processing or waiting occurs while the mutex is locked (except a CV wait, which internally releases the lock while waiting.) Therefore waiting to obtain a lock on the mutex should always be a relatively fast operation.

 The worker thread will remain idle and blocked until either it gets the done signal, or there is something in the inbox it can pop and process.

 Enqueueing an item for conversion is a simple matter of locking the mutex and pushing to the queue.

 On a polite shutdown (ie, destruction without cancellation), the destructor will wait for the queue to empty. Invocation of the destructor in a well-formed program implies no outstanding references to the object exist, which in turn means it's not possible for a caller to add any more work to the inbox. Therefore we can infer that no additional work will be added to the inbox, so the destructor signals done (without canceling) and then waits for the thread to exit. In the meantime, the worker thread finishes any last item it may be working on, invokes the callback (since it hasn't been canceled) and then returns, at that time unblocking the destructor. At that point it is then safe for the destructor to return, allow the mutex to be destroyed, etc.

 In the event of cancellation, there is no need for the canceling thread to wait on anything (except any small wait required to obtain the lock) and we simply need to signal done, set the canceled flag, and tell the PigLatinConverter to cancel anything it's currently working on. The worker thread will have its call to PigLatinConverter::convert() return, it will see the cancellation flag, drop any invalid result it obtained, and return. The destructor won't wait for the inbox to empty, but will still wait for the worker thread to finish in order to be able to clean up safely.
*/

#include "async_converter.h"

#include <cassert>

async_converter::async_converter(std::function<void(std::string const& original,
                                                    std::string const& converted)> callback) : thread_([this, callback =
std::move(callback)]() {
    while (true) {
        std::string original;
        {
            std::unique_lock lock{mutex_};
            cv_.wait(lock, [this] { return done_ || !inbox_.empty(); });
            if (done_) return;
            assert(!inbox_.empty());
            original = std::move(inbox_.front());
            inbox_.pop_front();
            cv_.notify_all();
        }
        auto converted = converter_.convert(original);
        {
            // PigLatinConverter::convert() appears to return immediately when canceled, but still returns a string, with no indication to the caller here whether that string is valid or garbage as a result of cancellation. Check the canceled flag before telling _our_ caller that we have a valid result.
            std::lock_guard lock{mutex_};
            if (!canceled_)
                callback(original, converted);
        }
    }
}) {}

async_converter::~async_converter() {
    {
        std::unique_lock lock{mutex_};
        cv_.wait(lock, [this] { return canceled_ || inbox_.empty(); });
        done_ = true;
        cv_.notify_all();
    }
    thread_.join();
}

void async_converter::convert(std::string original) {
    std::lock_guard lock{mutex_};
    if (done_) return;
    inbox_.push_back(std::move(original));
    cv_.notify_all();
}

void async_converter::cancel() {
    std::lock_guard lock{mutex_};
    canceled_ = true;
    done_ = true;
    converter_.cancel();
    cv_.notify_all();
}
