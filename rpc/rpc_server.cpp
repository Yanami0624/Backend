#include "rpc_server.h"
#include "../common/protocol.h"
#include <iostream>

RpcServer::RpcServer(int port, int threads): server(port, threads) {}

void RpcServer::registerMethod(const std::string& method, RpcHandler handler) {
    handlers[method] = handler;
}

std::string RpcServer::dispatch(const std::string& method, const std::string& payload) {
    auto it = handlers.find(method);
    if (it == handlers.end()) {
        return "method not found";
    }
    return it->second(payload);
}

void RpcServer::start() {
    server.setMessageCallback([this](const std::string& method, const std::string& payload) {
        return dispatch(method, payload);
    });
    server.start();
}