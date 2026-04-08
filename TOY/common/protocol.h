#pragma once

#include "net.h"

#include <iostream>
#include <cstring>
#include <cstdint>
#include <arpa/inet.h>
#include <memory>
#include <string>
#include <format>
using namespace std;

struct MessageHeader {
    uint16_t magic;
    uint16_t type;
    uint32_t len;
    uint32_t request_id;

    void print() {
        auto lines = format("magic-{}\nlen{}\nid{}\n", magic, len, request_id);
        cout << lines;
    }
};

static const uint16_t MAGIC = 0x1145;
static const int HEADER_LEN = sizeof(MessageHeader); // 12 字节
static const int METHOD_LEN = 20;
// 协议格式：| Header(12) | method(20) | payload |

struct Message {
    MessageHeader header;
    char method[METHOD_LEN];
    string payload;
};

// 写入网络字节序（自动处理 16/32 位）
template<class T>
static void write2net(string &s, T data) {
    if constexpr (sizeof(T) == 2) {
        uint16_t net_data = htons(data);
        s.append((char*)&net_data, sizeof(net_data));
    } else if constexpr (sizeof(T) == 4) {
        uint32_t net_data = htonl(data);
        s.append((char*)&net_data, sizeof(net_data));
    }
}

// 从网络字节序读取（自动处理 16/32 位）
template<class T>
static void read4net(const char* c, T& data) {
    memcpy(&data, c, sizeof(T));
    if constexpr (sizeof(T) == 2) {
        data = ntohs(data);
    } else if constexpr (sizeof(T) == 4) {
        data = ntohl(data);
    }
}

// 编码消息
inline string encodeMsg(
    const char *method, 
    const string &payload, 
    int type,
    int seq
) {
    string ret;
    
    // 写入 Header（严格 12 字节）
    write2net(ret, (uint16_t)MAGIC);
    write2net(ret, (uint16_t)type);
    write2net(ret, (uint32_t)payload.length());
    write2net(ret, (uint32_t)seq);

    // 写入 method：固定 20 字节，不足补 0，超出截断
    char method_buf[METHOD_LEN] = {0};
    strncpy(method_buf, method, METHOD_LEN - 1);
    ret.append(method_buf, METHOD_LEN);

    // 写入 payload
    ret.append(payload);
    return ret;
}

// 解码消息
inline Message decodeBody(const char *cs) {
    Message msg{};

    size_t copy_len = min(strlen(cs) - HEADER_LEN, (size_t)METHOD_LEN);
    memcpy(msg.method, cs, copy_len);

    size_t payload_start = HEADER_LEN + METHOD_LEN;
    msg.payload.append(cs + METHOD_LEN);

    return msg;
}

inline void decodeHeader(MessageHeader& header, const char *cs) {
    read4net(cs + 0, header.magic);
    read4net(cs + 2, header.type);
    read4net(cs + 4, header.len);
    read4net(cs + 8, header.request_id);
}

#include <iostream>
#include <vector>
#include <sstream>
#include <regex>
#include <format>
namespace RpcParamUtils {
    const std::string ESCAPE_COMMA = "\\,";
    const std::string REAL_COMMA = ",";

    inline std::string serialize(const std::vector<std::string>& params) {
        std::ostringstream oss;
        for (size_t i = 0; i < params.size(); ++i) {
            std::string param = params[i];
            size_t pos = 0;
            while ((pos = param.find(REAL_COMMA, pos)) != std::string::npos) {
                param.replace(pos, REAL_COMMA.length(), ESCAPE_COMMA);
                pos += ESCAPE_COMMA.length();
            }
            oss << param;
            if (i != params.size() - 1) {
                oss << REAL_COMMA;
            }
        }
        return oss.str();
    }

    inline std::vector<std::string> deserialize(const std::string& param_str) {
        std::vector<std::string> params;
        std::string current_param;

        for (size_t i = 0; i < param_str.size(); ++i) {
            if (i < param_str.size() - 1 && param_str[i] == '\\' && param_str[i+1] == ',') {
                current_param += REAL_COMMA;
                i++;
            }
            else if (param_str[i] == ',') {
                params.push_back(current_param);
                current_param.clear();
            }
            else {
                current_param += param_str[i];
            }
        }
        if (!current_param.empty() || !params.empty()) {
            params.push_back(current_param);
        }

        return params;
    }
}
