// pending_manager.h
#pragma once

#include <unordered_map>
#include <mutex>
#include <future>


class PendingManager {
public:

    std::future<std::string> add(uint64_t id) {
        std::lock_guard<std::mutex> lock(mutex_);

        std::promise<std::string> promise;

        auto future = promise.get_future();

        pending_[id] = std::move(promise);

        return future;
    }

    void set_response(uint64_t id,
                      const std::string& response) {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = pending_.find(id);

        if (it != pending_.end()) {
            it->second.set_value(response);

            pending_.erase(it);
        }
    }

    void timeout(uint64_t id) {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = pending_.find(id);

        if (it != pending_.end()) {
            it->second.set_exception(
                std::make_exception_ptr(
                    std::runtime_error("timeout")
                )
            );

            pending_.erase(it);
        }
    }

private:

    std::mutex mutex_;

    std::unordered_map<
        uint64_t,
        std::promise<std::string>
    > pending_;
};
