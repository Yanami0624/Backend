#include "tcp_connection.h"

TcpConnection::TcpConnection(int fd)
    : sockfd(fd),
      running(true)
{
}

TcpConnection::~TcpConnection() {
    close(sockfd);

    std::cout
        << "connection destroyed\n";
}

void TcpConnection::start() {
    handleRead();
}

void TcpConnection::handleRead() {

    char buffer[1024];

    while (running) {

        ssize_t n =
            recv(
                sockfd,
                buffer,
                sizeof(buffer),
                0
            );

        if (n <= 0) {
            running = false;
            break;
        }

        std::string msg(
            buffer,
            n
        );

        std::cout
            << "recv: "
            << msg
            << std::endl;

        send(
            sockfd,
            buffer,
            n,
            0
        );
    }
}