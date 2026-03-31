cmake_minimum_required(VERSION 3.10)

project(mini_rpc)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    net
    common
    rpc
    threadpool
)

add_executable(
    server
    example/calculator_server.cpp

    net/tcp_server.cpp
    net/tcp_connection.cpp
)

add_executable(
    client
    example/calculator_client.cpp
)
