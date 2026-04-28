#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <atomic>

std::atomic<bool> running{true};

void recv_handler(int sock) {

    char buf[1024];
    while (running) {
        memset(buf, 0, sizeof buf);
        int bytes = read(sock, buf, sizeof buf - 1);
        if (bytes <= 0) {
            std::cout << "\n 与服务器断开连接 \n";
            running = false;
            break;
        }
        std::cout << std::endl << buf << std::endl;
        std::cout << "你: " << std::flush;        

    }
}


std::string trim(const std::string& username) {
    size_t start = username.find_first_not_of(" \n\t\r\f\v");
    if (start == std::string::npos) {
        return "";
    }
    size_t end = username.find_last_not_of(" \n\t\r\f\v");
    return username.substr(start, end - start + 1);
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket failed");
        return 1;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("连接失败");
        return 1;
    }

    // 欢迎
    char welcome[64];
    read(sock, welcome, sizeof welcome - 1);
    std::cout << welcome << std::endl;

    std::string name;
    while (true) {
        getline(std::cin, name);
        // 去除首尾空格
        name = trim(name);
        if (name.empty()) {
            std::cout << "用户名不可为空!" << std::endl;
            continue;
        }
        break;
    }
    write(sock, name.c_str(), name.size());

    std::thread(recv_handler, sock).detach();

    std::string input;
    while (running) {
        std::cout << "你: " << std::flush;
        getline(std::cin, input);
        if (!running) {     // 在等待输入的时候可能接受线程已经结束连接
            break;
        }
        if (input == "exit") {
            running = false;
            break;
        }
        input += '\n';
        write(sock, input.c_str(), input.size());
    }


    close(sock);
    return 0;
}