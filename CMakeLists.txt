cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(mini_rpc)

include_directories(
    net
    common
    rpc
    threadpool
)

add_executable(
    server
    example/calculator_server.cpp

    rpc/rpc_server.cpp
    net/tcp_server.cpp
    net/tcp_connection.cpp
    common/buffer.cpp
    common/protocol.cpp
)

add_executable(
    client
    example/calculator_client.cpp

    rpc/rpc_client_pool.cpp
    rpc/rpc_client.cpp
    common/buffer.cpp
    common/protocol.cpp
)
