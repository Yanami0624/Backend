#include "rpc_client_pool.h"

RpcClientPool::RpcClientPool(const std::string& ip, int port, size_t pool_size)
    : server_ip(ip),
      server_port(port),
      max_connections(pool_size)
{
    for (size_t i = 0; i < pool_size; ++i) {
        auto client = std::make_shared<RpcClient>(server_ip, server_port);
        idle.push(client);
    }
}

std::shared_ptr<RpcClient> RpcClientPool::getClient() {
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [&] {
        return !idle.empty();
    });

    auto client = idle.front();
    idle.pop();
    return client;
}

void RpcClientPool::releaseClient(std::shared_ptr<RpcClient> client) {
    std::lock_guard<std::mutex> lock(mutex);
    idle.push(client);
    cv.notify_one();
}

ClientGuard RpcClientPool::acquire() {
    auto client = getClient();
    return ClientGuard(this, client);
}

std::string RpcClientPool::call(const std::string& method, const std::string& payload, std::chrono::milliseconds timeout) {
    auto client = acquire();
    return client->call(method, payload, timeout);
}