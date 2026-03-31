// tcp_client.cpp

#include "tcp_include.h"

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    connect(fd, (sockaddr*)&addr, sizeof(addr));

    auto msg = "fuck you";
    send(fd, msg, sizeof(msg), 0);

    char buffer[1024];
    int n = recv(fd, buffer, sizeof(buffer), 0);

    std::cout << std::string(buffer, n) << std::endl;

    close(fd);
}