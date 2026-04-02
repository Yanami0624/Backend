#include "../rpc/rpc_server.h"

#include <iostream>
#include <sstream>

std::string add(
    const std::string& payload
) {

    int a, b;

    char comma;

    std::stringstream ss(
        payload
    );

    ss >> a >> comma >> b;

    int result =
        a + b;

    return std::to_string(
        result
    );
}

int main() {

    RpcServer server(
        9000,
        4
    );

    server.registerMethod(
        "add",
        add
    );

    server.start();
}