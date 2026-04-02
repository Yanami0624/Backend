#pragma once

#include <string>
#include <atomic>
#include <future>
#include <unordered_map>
#include "../common/protocol.h"

class RpcClient {
public:

    RpcClient(
        const std::string& ip,
        int port
    );

    ~RpcClient();

    
    void recvLoop();
    void handleResponse(const RpcMessage& resp);
    
    std::string call(
        const std::string& method,
        const std::string& payload,
        std::chrono::milliseconds timeout
    );
    std::future<std::string> callAsync(
        const std::string& method,
        const std::string& payload
    );
    

private:

    int connectServer();

    int sockfd;
    std::string server_ip;
    int server_port;

    std::atomic<uint64_t> request_id{0};

    std::mutex pending_mutex;

    std::unordered_map<
        uint64_t,
        std::promise<std::string>
    > pending;

    std::thread recv_thread;

    Buffer buffer;

    std::atomic<bool> running{true};
};