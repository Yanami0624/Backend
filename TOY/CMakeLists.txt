cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_PARALLEL_LEVEL 8)
add_definitions(-w)

project(toy)

include_directories(
    common
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
    tcp/tcp_client_pool.cpp

)