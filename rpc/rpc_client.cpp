
#include <iostream>
#include <atomic>

class RpcClient {
public:

    std::string call(
        const std::string& service,
        const std::string& request)
    {
        uint32_t id = nextId();

        std::cout
            << "send request id="
            << id
            << std::endl;

        return "mock response";
    }

private:

    uint32_t nextId() {
        return ++request_id;
    }

private:

    std::atomic<uint32_t> request_id{0};

};