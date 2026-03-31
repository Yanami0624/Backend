#pragma once

#include <vector>
#include <string>
#include <cstring>

class Buffer {
public:

    void append(const void* data, size_t len) {
        const char* d = static_cast<const char*>(data);
        buf.insert(buf.end(), d, d + len);
    }

    void append(const std::string& s) {
        append(s.data(), s.size());
    }

    const char* data() const {
        return buf.data();
    }

    size_t size() const {
        return buf.size();
    }

    void clear() {
        buf.clear();
    }

private:

    std::vector<char> buf;

};