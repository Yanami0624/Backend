#include "../tcp/tcp_server.h"

using namespace std;
using namespace RpcParamUtils;

string echo(string s) {
    return s;
}
string add(string s) {
    auto args = deserialize(s);
    int ret = 0;
    for(auto i: args) ret += stoi(i);
    return format("{}", ret);
}

int main() {
    TcpServer server(
        9000,
        10
    );

    string method;
    method = "echo";
    server.registerFunc(method, echo);
    method = "add";
    server.registerFunc(method, add);

    server.start();
}