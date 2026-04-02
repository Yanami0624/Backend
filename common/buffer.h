#pragma once

#include <vector>
#include <string>
#include <cstring>

class Buffer {
public:

    Buffer();

    size_t readableBytes() const;

    const char* peek() const;

    void retrieve(size_t len);

    void append(
        const char* data,
        size_t len
    );

    void append(
        const std::string& s
    );

private:

    std::vector<char> buffer;

    size_t readIndex;
};