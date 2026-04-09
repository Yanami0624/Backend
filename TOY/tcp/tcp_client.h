#pragma once

#include "net.h"
#include "../thread_pool/thread_pool.h"
#include "../common/protocol.h"

#include <cstring>
#include <iostream>
#include <functional>
#include <thread>
#include <vector>
#include <memory>
#include <atomic>
#include <mutex>

using namespace std;

class TcpClient {
private:
    int sockfd;
    string server_ip;
    int port;
    atomic<uint64_t> request_id{0};

    mutex pending_mutex;
    unordered_map<uint64_t, promise<string>> pending;

    thread recv_thread;
    atomic<bool> stop{false};

public:
    TcpClient(string _ip, int _port);
    ~TcpClient();
    
    void connectServer();
    void disconnect();
    void recvLoop();
    void handleResponse(int id, string s);

    string call(
        string &method,
        string &payload,
        chrono::milliseconds timeout
    );
    future<string> callAsync(
        string &method,
        string &payload
    );
};