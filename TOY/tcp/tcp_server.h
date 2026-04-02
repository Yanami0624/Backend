#pragma once

#include "net.h"
#include "../thread_pool/thread_pool.h"

#include <cstring>
#include <iostream>
#include <functional>
#include <thread>
#include <vector>
#include <memory>
#include <atomic>

using namespace std;

class TcpServer {
private:
    int port;
    int listen_fd;
    ThreadPool threadpool;
    
    void acceptLoop();
    void handleClient(int connfd);

public:
    TcpServer(int _port, int _nthrds);

    void start();
};