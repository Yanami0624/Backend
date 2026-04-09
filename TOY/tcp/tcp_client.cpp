

// tcp_client.cpp
#include "tcp_client.h"

TcpClient::TcpClient(string _ip, int _port): server_ip(_ip), port(_port) {
    connectServer();
    recv_thread = std::thread(&TcpClient::recvLoop, this);
}

TcpClient::~TcpClient() {
    stop.store(true);
    shutdown(sockfd, SHUT_RDWR);
    if (recv_thread.joinable()) {
        recv_thread.join();
    }
    close(sockfd);
}

void TcpClient::connectServer() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, server_ip.c_str(), &addr.sin_addr);
    connect(fd, (sockaddr*)&addr, sizeof(addr));

    sockfd = fd;
}

void TcpClient::recvLoop() {
    while(!stop) {
        char header_buf[HEADER_LEN] = {0};
        ssize_t n = recv(sockfd, header_buf, HEADER_LEN, 0);
        if(n <= 0) {
            stop.store(true);
            break;
        }
        MessageHeader header;
        decodeHeader(header, header_buf);

        uint32_t body_len = METHOD_LEN + header.len;
        char *buffer = new char[body_len];
        n = read(sockfd, buffer, body_len);
        
        Message msg = decodeBody(buffer, header.len);
        delete []buffer;

        handleResponse(header.request_id, move(msg.payload));
    }
}

void TcpClient::handleResponse(int id, string s) {
    lock_guard<mutex> lg(pending_mutex);
    auto it = pending.find(id);
    if(it != pending.end()) {
        it->second.set_value(move(s));
        pending.erase(it);
    } else {
        auto err = format("client: response_id {} not found", id);
        perror(err.c_str());
    }
}

string TcpClient::call(
    string& method,
    string& payload,
    chrono::milliseconds timeout
) {
    int id = request_id.fetch_add(1);
    auto msg = encodeMsg(method.c_str(), payload, 0, id);
    auto p = promise<string>();
    auto f = p.get_future();
    {
        lock_guard<mutex> lg(pending_mutex);
        pending.emplace(id, move(p));
    }

    send(sockfd, msg.c_str(), msg.length(), 0);
    if(f.wait_for(timeout) != future_status::ready) {
        lock_guard<mutex> lg(pending_mutex);
        pending.erase(id);
        throw runtime_error("client: Timeout when waiting for response.");
    }

    return f.get();
}

future<string> TcpClient::callAsync(
        string &method,
        string &payload
) {
    // int id = request_id++;
    // auto msg = encodeMsg(method.c_str(), payload, 0, id);
    // auto p = promise<string>();
    // auto f = p.get_future();
    // {
    //     lock_guard<mutex> lg(pending_mutex);
    //     pending.emplace(id, move(p));
    // }

    // send(sockfd, msg.c_str(), msg.length(), 0);

    // return f;
};