
// tcp_server.cpp
#include "tcp_server.h"
#include <format>
using namespace std;
TcpServer::TcpServer(int _port, int _nthrds): 
    port(_port), 
    threadpool(ThreadPool(_nthrds)) {}

void TcpServer::start() {
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(listen_fd, (sockaddr*)&addr, sizeof(addr));

    listen(listen_fd, 128);
    std::cout << "server start\n";

    acceptLoop();
}

void TcpServer::acceptLoop() {
    while (true) {
        sockaddr_in client_addr{};
        socklen_t len = sizeof(client_addr);
        int connfd = accept(listen_fd, (sockaddr*)&client_addr, &len);

        if (connfd < 0) {perror("accept"); continue;}

        std::cout << "new connection\n";

        auto fu = threadpool.push([this, connfd]() {
            handleClient(connfd);
        });
    }
}

// ssize_t readFull(int fd, void* buf, size_t len) {
//     size_t total = 0;
//     char* p = (char*)buf;
//     while (total < len) {
//         ssize_t n = read(fd, p + total, len - total);
//         if (n <= 0) {
//             return n;
//         }
//         total += n;
//     }
//     return total;
// }

void TcpServer::handleClient(int connfd) {
    char header_buf[HEADER_LEN] = {0};
    ssize_t n = read(connfd, header_buf, HEADER_LEN);
    if(n != HEADER_LEN) {
        perror("header length error.");
    }

    MessageHeader header;
    decodeHeader(header, header_buf);
    // header.print();

    uint32_t body_len = METHOD_LEN + header.len;
    char *buffer = new char[body_len];
    n = read(connfd, buffer, body_len);
    Message msg = decodeBody(buffer);
    delete []buffer;

    auto f = funcs.find(msg.method);
    if(f == funcs.end()) {
        string reply = format("method \"{}\" not found", msg.method);
        write(connfd, reply.c_str(), reply.length());
    } else {
        string reply_payload = funcs[msg.method](msg.payload);
        auto reply = encodeMsg("", reply_payload, 0, msg.header.request_id);
        write(connfd, reply.c_str(), reply.length());
    }

    close(connfd);
}

void TcpServer::registerFunc(string &method_name, func f) {
    funcs[method_name] = move(f);
}

