#pragma once

#include <string>
#include <cstdint>
#include <arpa/inet.h>

enum MessageType {

    REQUEST = 1,
    RESPONSE = 2

};

struct RpcHeader {

    uint32_t total_len;

    uint32_t request_id;

    uint16_t type;

    uint16_t method_len;

};

struct RpcMessage {

    RpcHeader header;

    std::string method;

    std::string payload;

};

#include "buffer.h"
// encode -------------------------------------------

static void writeUint32(
    std::string& out,
    uint32_t value
) {
    value = htonl(value);

    out.append(
        (char*)&value,
        sizeof(value)
    );
}

static void writeUint16(
    std::string& out,
    uint16_t value
) {
    value = htons(value);

    out.append(
        (char*)&value,
        sizeof(value)
    );
}

inline std::string encodeMessage(
    const RpcMessage& msg
) {

    std::string out;

    uint32_t total_len =
        sizeof(RpcHeader)
        + msg.method.size()
        + msg.payload.size();

    writeUint32(out, total_len);

    writeUint32(
        out,
        msg.header.request_id
    );

    writeUint16(
        out,
        msg.header.type
    );

    writeUint16(
        out,
        msg.method.size()
    );

    out.append(msg.method);

    out.append(msg.payload);

    return out;
}

// decode -------------------------------------------

inline bool decodeMessage(
    Buffer& buffer,
    RpcMessage& msg
) {

    if (
        buffer.readableBytes()
        < sizeof(RpcHeader)
    )
        return false;

    const char* data =
        buffer.peek();

    uint32_t total_len =
        ntohl(
            *(uint32_t*)data
        );

    if (
        buffer.readableBytes()
        < total_len
    )
        return false;

    msg.header.total_len =
        total_len;

    msg.header.request_id =
        ntohl(
            *(uint32_t*)(data + 4)
        );

    msg.header.type =
        ntohs(
            *(uint16_t*)(data + 8)
        );

    msg.header.method_len =
        ntohs(
            *(uint16_t*)(data + 10)
        );

    size_t pos = 12;

    msg.method.assign(
        data + pos,
        msg.header.method_len
    );

    pos += msg.header.method_len;

    msg.payload.assign(
        data + pos,
        total_len
        - sizeof(RpcHeader)
        - msg.header.method_len
    );

    buffer.retrieve(total_len);

    return true;
}