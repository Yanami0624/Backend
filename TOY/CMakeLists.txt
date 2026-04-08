cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-w)

project(toy)

include_directories(
    common
    example
    rpc
    tcp
    thread_pool
)

add_executable(
    server
    example/server.cpp

    tcp/tcp_server.cpp
)

add_executable(
    client
    example/client.cpp

    tcp/tcp_client.cpp
)