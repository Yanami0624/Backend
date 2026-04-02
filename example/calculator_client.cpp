#include "../rpc/rpc_client.h"
#include "../rpc/rpc_client_pool.h"
#include <iostream>
#include <format>
using namespace std;

int main() {

    RpcClientPool pool(
        "127.0.0.1",
        9000,
        4
    );

{
    for(int i = 0; i < 10; ++i) {
    auto client = pool.acquire();

    auto resp =
        client->call(
            "add",
            "114000,514",
            std::chrono::milliseconds(3000)
        );
        cout << resp << endl;
    }
    
}
}