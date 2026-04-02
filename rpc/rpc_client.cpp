#include "rpc_client.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

#include "../common/protocol.h"
#include "../common/buffer.h"

RpcClient::RpcClient(const std::string& ip, int port)
    : server_ip(ip),
      server_port(port),
      request_id(1),
      running(true)
{
    connectServer();

    recv_thread = std::thread(
        &RpcClient::recvLoop,
        this
    );
}

RpcClient::~RpcClient() {
    running = false;

    shutdown(sockfd, SHUT_RDWR);

    if (recv_thread.joinable()) {
        recv_thread.join();
    }

    close(sockfd);
}

int RpcClient::connectServer() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip.c_str(), &addr.sin_addr);

    if (connect(sockfd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        exit(1);
    }

    return 0;
}

void RpcClient::recvLoop() {
    char temp[1024];

    while (running) {
        ssize_t n = recv(sockfd, temp, sizeof(temp), 0);

        if (n <= 0) {
            running = false;
            break;
        }

        buffer.append(temp, n);

        RpcMessage resp;

        while (decodeMessage(buffer, resp)) {
            handleResponse(resp);
        }
    }
}

void RpcClient::handleResponse(const RpcMessage& resp) {
    std::lock_guard<std::mutex> lock(pending_mutex);

    auto it = pending.find(resp.header.request_id);
    if (it != pending.end()) {
        it->second.set_value(resp.payload);
        pending.erase(it);
    }
}

std::string RpcClient::call(
    const std::string& method,
    const std::string& payload,
    std::chrono::milliseconds timeout)
{
    uint64_t id = request_id++;

    RpcMessage msg;
    msg.header.request_id = id;
    msg.header.type = REQUEST;
    msg.method = method;
    msg.payload = payload;

    std::string data = encodeMessage(msg);

    std::promise<std::string> promise;
    auto future = promise.get_future();
    {
        std::lock_guard<std::mutex> lock(pending_mutex);
        pending.emplace(id, std::move(promise));
    }

    send(sockfd, data.data(), data.size(), 0);
    if (future.wait_for(timeout) != std::future_status::ready) {
        std::lock_guard<std::mutex> lock(pending_mutex);
        pending.erase(id);
        throw std::runtime_error("rpc timeout");
    }

    return future.get();
}

std::future<std::string> RpcClient::callAsync(
    const std::string& method,
    const std::string& payload)
{
    uint64_t id = request_id++;

    RpcMessage msg;
    msg.header.request_id = id;
    msg.header.type = REQUEST;
    msg.method = method;
    msg.payload = payload;

    std::string data = encodeMessage(msg);

    std::promise<std::string> promise;
    auto future = promise.get_future();
    {
        std::lock_guard<std::mutex> lock(pending_mutex);
        pending.emplace(id, std::move(promise));
    }

    send(sockfd, data.data(), data.size(), 0);

    return future;
}