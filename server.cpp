#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <mutex>
#include <vector>
#include <thread>
#include <algorithm>
#include <csignal>

struct ClientInfo {
    std::string name;
    int fd;
    ClientInfo(std::string name, int fd) {
        this->fd = fd, this->name = name;
    }
};

std::vector<ClientInfo> clients;
std::mutex clients_mutex;

void broadcast(const std::string& message, int sender_fd = -1) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (const auto& client : clients) {
        if (client.fd != sender_fd) {
            write(client.fd, message.c_str(), message.size());
        }
    }
}


void handle_client(int client_fd) {

    std::string welcome = "请输入用户名：";
    write(client_fd, welcome.c_str(), welcome.size());

    char name_buf[64] = {0};
    int len = read(client_fd, name_buf, sizeof(name_buf) - 1);
    if (len <= 0) {
        // TODO: 更好的错误处理
        close(client_fd);
        return;
    }
    std::string username(name_buf);
    if (username.back() == '\n') username.pop_back();


    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.push_back({username, client_fd});
        std::cout << "用户: " << username << " ,fd: " << client_fd << " 加入聊天室！当前人数: " << clients.size() << std::endl;
    }
    std::string msg = "系统: " + username + " 进入了聊天室！\n";
    broadcast(msg, client_fd);

    std::string userListMsg = "USER_LIST";
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        for (const auto& c : clients) {
            userListMsg += " " + c.name;
        }
    }
    userListMsg += '\n';
    write(client_fd, userListMsg.c_str(), userListMsg.size());

    // TODO: 处理消息获取循环

    // TODO: 更好的缓存管理
    char buf[1024];
    while (true) {
        memset(buf, 0, sizeof buf);

        int bytes = read(client_fd, buf, sizeof buf - 1);
        if (bytes <= 0) {
            break;
        }

        std::string msg(buf);
        while (!msg.empty() && (msg.back() == '\n' || msg.back() == '\r')) msg.pop_back();

        // 私信
        if (msg[0] == '@') {
            size_t space_pos = msg.find(' ');
            if (space_pos != std::string::npos) {
                std::string target = msg.substr(1, space_pos - 1);
                std::string content = msg.substr(space_pos + 1);

                std::lock_guard<std::mutex> lock(clients_mutex);
                auto it = std::find_if(clients.begin(), clients.end(), [&target](const ClientInfo& c) {
                    return c.name == target;
                });

                if (it != clients.end()) {
                    std::string formatted = "[私聊] " + username + " 对你说: " + content + '\n';
                    write(it->fd, formatted.c_str(), formatted.size());
                } else {
                    std::string warning = "系统: 用户 " + username + " 不存在！\n";
                    write(client_fd, warning.c_str(), warning.size());
                }
                continue;
            } else {
                std::string error = "系统: 格式错误！";
                write(client_fd, error.c_str(), error.size()); 
            }
            continue;
        }

        // 群发
        std::string public_msg = username + ": " + msg + '\n';
        broadcast(public_msg, client_fd);
    }

    // TODO: 离开处理
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        // C++ 经典的 erase-remove idiom
        clients.erase(std::remove_if(clients.begin(), clients.end(), [&client_fd](const ClientInfo& c) {
            return c.fd == client_fd;
        }), clients.end());
        std::cout << "用户：" << username << " ,fd: " << client_fd << " 离开聊天室！当前人数: " << clients.size() << std::endl;
    }
    
    std::string leaveing_msg = "系统: " + username + " 离开了聊天室！";
    broadcast(leaveing_msg);
    close(client_fd);
}

int main() {

    // 忽略SIGPIPE信号, 防止因为某个客户端的异常离开导致服务器崩溃
    signal(SIGPIPE, SIG_IGN);


    // 创建socket AF_INET: 使用IPv4地址簇 SOCK_STREAM: TCP字节流 0: 协议号
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket failed");
        return 1;
    }


    // 绑定服务器地址 这是一个ipv4地址 
    sockaddr_in address{};
    address.sin_family = AF_INET;           // 表示ipv4类型
    address.sin_addr.s_addr = INADDR_ANY;   // 表示绑定到本机的所有网卡地址
    address.sin_port = htons(8080);         // host to network short 把端口号转换为大端法，网络字节序要求必须是大端法

    // 服务器socket和本机地址绑定 注意第二个参数必须强制类型转换 因为还有sockaddr_in6 ipv6等其他类型的地址
    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        return 1;
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen failed");
        return 1;
    }

    std::cout << "服务器监听端口 8080..." << std::endl;

    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) {
            perror("accept failed");
        }
        std::thread(handle_client, client_fd).detach();
    }

    close(server_fd);

    return 0;
}