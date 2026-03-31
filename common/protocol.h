// protocol.h

#pragma once

#include <cstdint>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

struct RpcHeader {

    uint32_t magic;

    enum MsgType {
        REQUEST,
        RESPONSE,
    };
    MsgType msg_type;

    uint32_t request_id;

    uint32_t body_len;

    static const uint32_t MAGIC = 0x12345678;

};


void send_message(
    int fd,
    const std::string& body
) {
    RpcHeader header;

    header.magic = RpcHeader::MAGIC;
    header.request_id = 1;
    header.msg_type = RpcHeader::MsgType::REQUEST;
    header.body_len = body.size();

    send(fd, &header, sizeof(header), 0);

    send(fd, body.data(), body.size(), 0);
}