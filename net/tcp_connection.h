#pragma once

#include "tcp_include.h"
#include "../common/buffer.h"
#include "../common/protocol.h"

class TcpConnection :
    public std::enable_shared_from_this<
        TcpConnection
    >
{
public:
    using MessageCallback =
    std::function<
        std::string(
            const std::string&,
            const std::string&
        )
    >;

    void setMessageCallback(
        MessageCallback cb
    );

    explicit TcpConnection(int fd);

    ~TcpConnection();

    void start();

private:

    void handleRead();

private:

    int sockfd;

    std::atomic<bool> running;

    MessageCallback messageCallback;

    Buffer buffer;
};