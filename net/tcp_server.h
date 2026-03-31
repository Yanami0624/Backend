#pragma once

#include "tcp_include.h"
#include "../threadpool/thread_pool.h"
#include "tcp_connection.h"

class TcpServer {
public:

    TcpServer(
        int port,
        int thread_num
    );

    ~TcpServer();

    void start();

private:

    void acceptLoop();

private:

    int port;

    int listen_fd;

    ThreadPool threadPool;
};