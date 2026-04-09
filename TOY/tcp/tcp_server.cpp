
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
        
        // int flags = fcntl(connfd, F_GETFL, 0);         // 获取当前标志
        // fcntl(connfd, F_SETFL, flags | O_NONBLOCK);    // 添加非阻塞标志

        if (connfd < 0) {perror("accept"); continue;}

        std::cout << "new connection\n";

        auto fu = threadpool.push([this, connfd]() {
            handleClient(connfd);
        });
    }
}

void TcpServer::handleClient(int connfd) {
    static int handleid = 0;
    ++handleid;
    // printf("handleid %d created\n", handleid); 
    while(true) {
        // printf("handleid %d\n", handleid); 
        char header_buf[HEADER_LEN] = {0};
        ssize_t n = recv(connfd, header_buf, HEADER_LEN, 0);
        if(n == 0) break;
        if(n < 0) {
            this_thread::sleep_for(chrono::milliseconds(1000));
            continue;
        }
        if(n != HEADER_LEN) {
            perror("recv_len != header_len");
        }

        MessageHeader header;
        decodeHeader(header, header_buf);
        // header.print();

        uint32_t body_len = METHOD_LEN + header.len;
        char *buffer = new char[body_len];
        n = read(connfd, buffer, body_len);
        Message msg = decodeBody(buffer, header.len);
        cout << "recv: " << msg.payload << endl;
        delete []buffer;

        auto f = funcs.find(msg.method);
        if(f == funcs.end()) {
            string reply_payload = format("method \"{}\" not found", msg.method);
            auto reply = encodeMsg("", reply_payload, 0, header.request_id);
            write(connfd, reply.c_str(), reply.length());
        } else {
            string reply_payload = funcs[msg.method](msg.payload);
            auto reply = encodeMsg("", reply_payload, 0, header.request_id);
            write(connfd, reply.c_str(), reply.length());
        }
    }
    close(connfd);
    cout << "connection closed\n";
}

void TcpServer::registerFunc(string &method_name, func f) {
    funcs[method_name] = move(f);
}

