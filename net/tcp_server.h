#pragma once

#include "tcp_include.h"
#include "../threadpool/thread_pool.h"
#include "tcp_connection.h"

class TcpServer {
public:
    using MessageCallback =
        TcpConnection::MessageCallback;

    void setMessageCallback(
        MessageCallback cb
    );

    
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
    
    MessageCallback messageCallback;
    
    ThreadPool threadPool;
};