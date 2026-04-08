
// client.cpp
#include "../tcp/net.h"
#include "../common/protocol.h"
#include "protocol.h"
#include "../tcp/tcp_client.h"

#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
using namespace std;
using namespace RpcParamUtils;

int main() {
    TcpClient client("127.0.0.1", 9000);
    auto method = string("echo");
    auto payload = string("1,2,3");
    auto res = client.call(method, payload, chrono::milliseconds(3000));
    cout << res << endl;
}