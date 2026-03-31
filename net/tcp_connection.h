#pragma once

#include "tcp_include.h"

class TcpConnection :
    public std::enable_shared_from_this<
        TcpConnection
    >
{
public:

    explicit TcpConnection(int fd);

    ~TcpConnection();

    void start();

private:

    void handleRead();

private:

    int sockfd;

    std::atomic<bool> running;
};