#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

int main() {
    int sock = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9000);

    inet_pton(
        AF_INET,
        "127.0.0.1",
        &addr.sin_addr
    );

    connect(
        sock,
        (sockaddr*)&addr,
        sizeof(addr)
    );

    std::string msg = "hello RPC";

    send(
        sock,
        msg.data(),
        msg.size(),
        0
    );

    char buf[1024];

    int n = recv(
        sock,
        buf,
        sizeof(buf),
        0
    );

    std::cout
        << std::string(buf, n)
        << std::endl;
}