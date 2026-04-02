#pragma once

#include <unordered_map>
#include <functional>
#include <string>
#include <memory>

#include "../net/tcp_server.h"

class RpcServer {
public:

    using RpcHandler =
        std::function<
            std::string(
                const std::string&
            )
        >;

    RpcServer(
        int port,
        int threads
    );

    void start();

    void registerMethod(
        const std::string& method,
        RpcHandler handler
    );

private:

    std::string dispatch(
        const std::string& method,
        const std::string& payload
    );

private:

    TcpServer server;

    std::unordered_map<
        std::string,
        RpcHandler
    > handlers;
};