#pragma once

#include "tcp_client.h"

#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

class ClientPool;
class ClientGuard {
friend class CilentPool;
public:
    ClientGuard(ClientPool* _pool, shared_ptr<TcpClient> _client)
        : pool(_pool), client(_client) {}
    ~ClientGuard();
    TcpClient* operator->() {
        return client.get();
    }
    private:
    ClientPool* pool;
    shared_ptr<TcpClient> client;
};

class ClientPool {
private:
    string ip;
    int port;
    int pool_size = 4;

    queue<shared_ptr<TcpClient>> idle;
    mutex lock_idle;
    condition_variable cv;

public:
    shared_ptr<TcpClient> getClient();
    void releaseClient(shared_ptr<TcpClient> client);

public:
    ClientPool(const string &ip, int _port, int _pool_size);
    ClientGuard acquire();
    string call(
        string &method,
        string& payload,
        chrono::milliseconds timeout);
};