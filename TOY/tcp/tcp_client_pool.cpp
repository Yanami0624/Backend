#include "tcp_client_pool.h"

ClientGuard::~ClientGuard() {
    pool->releaseClient(client);
}

ClientGuard ClientPool::acquire() {
    shared_ptr<TcpClient> client;
    {
        auto lock = unique_lock<mutex>(lock_idle);
        cv.wait(lock, [&]() {
            return !idle.empty();
        });
        client = idle.front();
        idle.pop();
    }
    return ClientGuard(this, client);
}

ClientPool::ClientPool(const string &_ip, int _port, int _pool_size)
: ip(_ip), port(_port), pool_size(_pool_size) {
    lock_guard<mutex> lg(lock_idle);
    for(int i = 0; i < pool_size; ++i) {
        auto client = make_shared<TcpClient>(ip, port);
        idle.push(client);
    }
}

shared_ptr<TcpClient> ClientPool::getClient() {
    {
        auto lock = unique_lock<mutex>(lock_idle);
        cv.wait(lock, [&]() {
            return !idle.empty();
        });
        auto client = idle.front();
        idle.pop();
        // printf("getClient");
        // printf(": %x, idle size %d\n", client.get(), idle.size());
        return client;
    }
}

void ClientPool::releaseClient(shared_ptr<TcpClient> client) {
    {
        lock_guard<mutex> lock(lock_idle);
        // printf("releaseClient");
        // printf(": %x, idle size %d\n", client.get(), idle.size());
        // client->connectServer();
        idle.push(client);
    }
    cv.notify_one();
}

string ClientPool::call(
        string &method,
        string& payload,
        chrono::milliseconds timeout) 
{
    auto client = getClient();
    auto ret = client->call(method, payload, timeout);
    releaseClient(client);
    return ret;
}

