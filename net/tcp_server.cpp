#include "tcp_server.h"

TcpServer::TcpServer(
    int port,
    int thread_num
)
    :
    port(port),
    threadPool(thread_num)
{
}

TcpServer::~TcpServer() {
    close(listen_fd);
}

void TcpServer::start() {

    listen_fd =
        socket(
            AF_INET,
            SOCK_STREAM,
            0
        );

    if (listen_fd < 0) {
        perror("socket");
        exit(1);
    }

    int opt = 1;

    setsockopt(
        listen_fd,
        SOL_SOCKET,
        SO_REUSEADDR,
        &opt,
        sizeof(opt)
    );

    sockaddr_in addr{};

    addr.sin_family = AF_INET;
    addr.sin_port =
        htons(port);

    addr.sin_addr.s_addr =
        INADDR_ANY;

    if (bind(
            listen_fd,
            (sockaddr*)&addr,
            sizeof(addr)
        ) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(
            listen_fd,
            128
        ) < 0) {
        perror("listen");
        exit(1);
    }

    std::cout
        << "server start\n";

    acceptLoop();
}

void TcpServer::acceptLoop() {

    while (true) {

        sockaddr_in client_addr{};

        socklen_t len =
            sizeof(client_addr);

        int connfd =
            accept(
                listen_fd,
                (sockaddr*)&client_addr,
                &len
            );

        if (connfd < 0) {
            perror("accept");
            continue;
        }

        std::cout
            << "new connection\n";

        auto conn =
            std::make_shared<
                TcpConnection
            >(connfd);

        threadPool.push(
            [conn]() {
                conn->start();
            }
        );
    }
}