
// client.cpp
#include "../tcp/net.h"
#include "../common/protocol.h"
#include "protocol.h"
#include "../tcp/tcp_client.h"
#include "../tcp/tcp_client_pool.h"

#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
using namespace std;
using namespace RpcParamUtils;

int main() {
    // TcpClient client("127.0.0.1", 9000);
    // auto method = string("add");
    // auto payload = string("1,2,3");
    // auto res = client.call(method, payload, chrono::milliseconds(3000));
    // cout << res << endl;

    auto pool = ClientPool("127.0.0.1", 9000, 4);
    for(int i = 0; i < 10; ++i) {
        auto method = string("add");
        auto payload = string("0,") + to_string(i * i);
        // cout << payload << endl;
        auto res = pool.call(method, payload, chrono::milliseconds(3000));
        cout << res << endl;
    }
}