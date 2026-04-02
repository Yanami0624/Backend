
// tcp_connection.cpp
#include "tcp_connection.h"
#include "../common/buffer.h"
#include "../common/protocol.h"

TcpConnection::TcpConnection(int fd): sockfd(fd), running(true) {}

TcpConnection::~TcpConnection() {
    close(sockfd);
    std::cout << "connection destroyed\n";
}

void TcpConnection::start() {
    handleRead();
}

void TcpConnection::setMessageCallback(MessageCallback cb) {
    messageCallback = std::move(cb);
}

void TcpConnection::handleRead() {
    Buffer buffer;
    char temp[1024];
    while (running) {
        ssize_t n = recv(sockfd, temp, sizeof(temp), 0);

        if (n <= 0)
            break;

        buffer.append(temp, n);

        RpcMessage msg;
        while (decodeMessage(buffer, msg)) {
            std::string result;
            if (messageCallback) {
                result = messageCallback(msg.method, msg.payload);
            } else {
                result = "no handler";
            }

            RpcMessage resp;
            resp.header.request_id = msg.header.request_id;
            resp.header.type = RESPONSE;
            resp.method = msg.method;
            resp.payload = result;
            
            std::string out = encodeMessage(resp);

            send(sockfd, out.data(), out.size(), 0);
        }
    }

    close(sockfd);
}