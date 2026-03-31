#include "../net/tcp_server.h"

int main() {
    TcpServer server(9000, 4);

    server.start();
}