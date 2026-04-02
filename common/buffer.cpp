#include "buffer.h"

Buffer::Buffer(): readIndex(0) {}

size_t Buffer::readableBytes() const {
    return buffer.size() - readIndex;
}

const char* Buffer::peek() const {
    return buffer.data() + readIndex;
}

void Buffer::retrieve(size_t len) {
    readIndex += len;

    if (readIndex == buffer.size()) {
        buffer.clear();
        readIndex = 0;
    }
}

void Buffer::append(const char* data, size_t len) {
    for (int i = 0; i < len; ++i) {
        buffer.emplace_back(*(data + i));
    }
}

void Buffer::append(const std::string& s) {
    append(s.data(), s.size());
}