cmake_minimum_required(VERSION 3.14)
project(chatroom VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找线程库
find_package(Threads REQUIRED)

# ---------- 服务端 ----------
add_executable(server server.cpp)
target_link_libraries(server PRIVATE Threads::Threads)

# ---------- 控制台客户端（使用 console/ 下的引擎） ----------
add_executable(console_client
    console/client.cpp
    console/chat_engine.cpp
)
target_include_directories(console_client PRIVATE console)
target_link_libraries(console_client PRIVATE Threads::Threads)

# ---------- Qt 客户端 ----------
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
add_subdirectory(qt)