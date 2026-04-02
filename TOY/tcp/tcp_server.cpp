
// tcp_server.cpp
#include "tcp_server.h"

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

void TcpServer::handleClient(int connfd) {
    char buffer[1024] = {0};

    while(true) {
        ssize_t n = read(connfd, buffer, sizeof(buffer));
        if(n <= 0) {
            std::cout << "Client disconnected\n";
            break;
        }
        std::cout << "Recv from client: " << buffer << std::endl;
        write(connfd, buffer, n);
        memset(buffer, 0, n);
    }

    close(connfd);
}