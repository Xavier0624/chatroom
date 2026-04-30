#include "chat_engine.h"
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <arpa/inet.h>
#include <unistd.h>
// 控制台版本的客户端实现
class ConsoleChatClient : public ChatClient {
public:
    void onConnected() override {
        // 连接成功，控制台已经由 connect 打印了？
    }

    void onLoginSuccess() override {
        // 暂不需要
    }

    void onMessageReceived(const std::string& sender, const std::string& message) override {
        std::cout << "\n" << sender << ": " << message << std::endl;
        std::cout << "你: " << std::flush;
    }

    void onPrivateMessage(const std::string& sender, const std::string& message) override {
        std::cout << "\n[私聊] " << sender << " 对你说: " << message << std::endl;
        std::cout << "你: " << std::flush;
    }

    void onSystemNotice(const std::string& text) override {
        std::cout << "\n" << text << std::endl;
        std::cout << "你: " << std::flush;
    }

    void onDisconnected() override {
        std::cout << "\n与服务器断开连接\n";
        running = false; 
    }
};

int main() {
    ConsoleChatClient client;

    if (!client.connectToServer("127.0.0.1", 8080)) {
        std::cerr << "连接服务器失败\n";
        return 1;
    }


    char buf[64];
    int len = read(client.getSockfd(), buf, sizeof(buf)-1); 
    if (len > 0) {
        buf[len] = '\0';
        std::cout << buf;
    }

    std::string username;
    while (true) {
        std::getline(std::cin, username);
        if (!username.empty() && username.find_first_not_of(" \t") != std::string::npos)
            break;
        std::cout << "用户名不能为空，请重新输入: ";
    }
    // 发送用户名并启动接收线程
    client.login(username);

    // 主线程负责读取用户输入并发送
    std::string input;
    while (client.getRunning()) {
        std::cout << "你: " << std::flush;
        std::getline(std::cin, input);
        if (!client.getRunning()) break;
        if (input == "exit") {
            break;
        }
        if (input.empty()) continue;

        if (input[0] == '@') {
            size_t space_pos = input.find(' ');
            if (space_pos != std::string::npos) {
                std::string target = input.substr(1, space_pos - 1);
                std::string msg = input.substr(space_pos + 1);
                client.sendPrivateMessage(target, msg);
                continue;
            }
        }
        client.sendPublicMessage(input);
    }

    client.disconnect();
    return 0;
}