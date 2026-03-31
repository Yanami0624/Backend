
#include <unordered_map>
#include <functional>
#include <iostream>

#include "../threadpool/thread_pool.cpp"
#include "../common/protocol.h"
#include "../common/buffer.h"

class RpcServer {
public:

    using Handler =
        std::function<std::string(const std::string&)>;

    void registerService(
        const std::string& name,
        Handler handler)
    {
        services[name] = handler;
    }

    std::string handleRequest(
        const std::string& service,
        const std::string& request)
    {
        if (!services.count(service)) {
            return "service not found";
        }

        return services[service](request);
    }

private:

    std::unordered_map<std::string, Handler> services;

};