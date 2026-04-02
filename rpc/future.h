// future.h
#pragma once

#include <future>
#include <chrono>

template<typename T>
class RpcFuture {
public:
    RpcFuture(std::future<T>&& fut)
        : future_(std::move(fut)) {}

    bool wait_for(std::chrono::milliseconds timeout) {
        return future_.wait_for(timeout)
            == std::future_status::ready;
    }

    T get() {
        return future_.get();
    }

private:
    std::future<T> future_;
};