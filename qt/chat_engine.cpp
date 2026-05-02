#include "chat_engine.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

ChatClient::ChatClient() : sockfd(-1), running(false), recvThread(nullptr) {}

ChatClient::~ChatClient()
{
    disconnectClient();
}

bool ChatClient::connectToServer(const std::string &ip, int host)
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return false;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(host);

    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

    if (connect(sockfd, (sockaddr*)&addr, sizeof addr) < 0) {
        close(sockfd);
        sockfd = -1;
        return false;
    }
    running = true;
    return true;
}

void ChatClient::login(const std::string &username)
{

    std::string message = username + '\n';
    sendRaw(message);

    // TODO 构造函数传参
    recvThread = new std::thread(&ChatClient::recvLoop, this);
    recvThread->detach();
}

void ChatClient::sendPublicMessage(const std::string &text)
{
    std::string message = text + '\n';
    sendRaw(message);
}

void ChatClient::sendPrivateMessage(const std::string &target, const std::string &text)
{
    std::string message = '@' + target + ' ' + text + '\n';
    sendRaw(message);
}

void ChatClient::disconnectClient()
{
    if (running) {
        running = false;
        if (sockfd != -1) {
            close(sockfd);
            sockfd = -1;
        }

    }
    recvThread = nullptr;

}

void ChatClient::recvLoop()
{
    char buffer[1024];
    while (running) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = read(sockfd, buffer, sizeof(buffer) - 1);
        if (bytes <= 0) {
            break;
        }
        std::string msg(buffer);
        // 去掉末尾的换行回车
        while (!msg.empty() && (msg.back() == '\n' || msg.back() == '\r'))
            msg.pop_back();

        if (msg.empty()) continue;

        // 协议解析
        if (msg.find("系统: ") == 0) {
            // 系统通知，例如 "系统: Alice 进入了聊天室"
            onSystemNotice(msg.substr(sizeof("系统: ") - 1)); // 注意去掉前缀长度
        }
        else if (msg.find("[私聊] ") == 0) {
            // 私聊格式: [私聊] Alice 对你说: hello
            size_t pos1 = msg.find(']') + 1; // 跳过 "[私聊] "
            size_t pos2 = msg.find(" 对你说: ");
            if (pos2 != std::string::npos) {
                std::string sender = msg.substr(pos1 + 1, pos2 - pos1 - 1); // 提取发送者
                std::string text = msg.substr(pos2 + sizeof(" 对你说: ") - 1);
                onPrivateMessage(sender, text);
            }
        }
        else {
            // 公开消息: 用户名: 内容
            size_t pos = msg.find(": ");
            if (pos != std::string::npos) {
                std::string sender = msg.substr(0, pos);
                std::string text = msg.substr(pos + 2);
                onMessageReceived(sender, text);
            }
        }
    }
    // 读失败或 running 为 false 都视为断开
    running = false;
    onDisconnected();
    if (sockfd != -1) {
        close(sockfd);
        sockfd = -1;
    }

}

bool ChatClient::sendRaw(const std::string &data)
{
    if (sockfd == -1) return false;
    int ret = write(sockfd, data.c_str(), data.size());
    return ret != -1;
}
