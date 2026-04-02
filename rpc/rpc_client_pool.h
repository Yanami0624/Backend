

#pragma once

#include "rpc_client.h"

#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

class RpcClientPool;

class ClientGuard {
public:
    ClientGuard(
        RpcClientPool* pool,
        std::shared_ptr<RpcClient> client)
        : pool_(pool),
          client_(client) {}

    ~ClientGuard();

    RpcClient* operator->() {
        return client_.get();
    }

private:

    RpcClientPool* pool_;
    std::shared_ptr<RpcClient> client_;
};

class RpcClientPool {
public:

    RpcClientPool(
        const std::string& ip,
        int port,
        size_t pool_size);

    ClientGuard acquire();

    std::string call(
        const std::string& method,
        const std::string& payload,
        std::chrono::milliseconds timeout);

private:

    friend class ClientGuard;

    std::shared_ptr<RpcClient> getClient();

    void releaseClient(
        std::shared_ptr<RpcClient> client);

private:

    std::string server_ip;

    int server_port;

    std::queue<std::shared_ptr<RpcClient>> idle;

    std::mutex mutex;

    std::condition_variable cv;

    uint64_t max_connections;
};

inline ClientGuard::~ClientGuard() {
    pool_->releaseClient(client_);
}