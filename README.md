
# 控制台聊天室 (Console Chatroom)

一个基于 C++ 和 Socket 的多线程命令行聊天室，支持群发消息和私聊。

## 功能

- ✅ 多客户端同时在线聊天  
- ✅ 用户进入 / 离开时自动通知全体在线成员  
- ✅ 群发公开消息  
- ✅ 私聊：使用 `@用户名 消息内容` 格式发送悄悄话  
- ✅ 命令 `exit` 安全退出  
- ✅ 在线人数实时显示  
- ✅ 输入校验（空用户名拒绝）

## 技术栈

- **语言**：C++11（核心使用 socket、多线程、标准库）
- **网络**：POSIX Socket API (TCP)
- **并发模型**：每个客户端一个处理线程 (`std::thread`) + 互斥锁 (`std::mutex`) 保护共享在线列表
- **构建**：`g++` 命令行编译，`-pthread` 链接线程库

## 目录结构

```
chatroom/
├── server.cpp          # 聊天服务端
├── client.cpp          # 聊天客户端
└── README.md
```

## 编译与运行

### 环境要求
- Linux / macOS / WSL (Windows Subsystem for Linux)
- g++ 支持 C++11

### 编译

```bash
# 编译服务端
g++ -std=c++11 -pthread -o server server.cpp

# 编译客户端
g++ -std=c++11 -pthread -o client client.cpp
```

### 运行

1. 启动服务端（默认监听 8080 端口）：
   ```bash
   ./server
   ```

2. 启动客户端（可开启多个终端）：
   ```bash
   ./client
   ```

3. 根据提示输入用户名后即可开始聊天。

## 使用示例

```
# 服务端日志
$ ./server
服务器监听端口 8080...
Alice 上线 (fd=5), 当前在线: 1
Bob 上线 (fd=6), 当前在线: 2
...
```

```
# 客户端 A (Alice)
请输入用户名: Alice
你: 大家好
你: @Bob 悄悄说
你: exit
```

```
# 客户端 B (Bob)
请输入用户名: Bob
系统: Alice 进入了聊天室
Alice: 大家好
[私聊] Alice 对你说: 悄悄说
你: 系统: Alice 离开了聊天室
```

## 技术亮点

- 使用 `std::vector` + `std::mutex` + `std::lock_guard` 实现线程安全的客户端管理
- 简单的文本协议解析，支持群聊与私聊命令
- `SIGPIPE` 信号处理防止服务器异常崩溃
- 清晰的代码注释与模块划分

## 后续计划

- [ ] 基于 `select` / `epoll` 的 I/O 多路复用高并发服务端
- [ ] 客户端终端 UI 优化（非规范输入 + ANSI 控制）
- [ ] 图形用户界面 (GUI) 版本（Qt 或 Dear ImGui）
- [ ] 聊天记录日志、心跳检测等

## 许可证

MIT License
```
