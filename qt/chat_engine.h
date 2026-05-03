#pragma once


#include <string>
#include <atomic>
#include <thread>


class ChatClient {
public:
    ChatClient();
    virtual ~ChatClient();

    // 连接服务器 成功返回true
    bool connectToServer(const std::string& ip, int host);

    // 登录（发送用户名）
    void login(const std::string& username);

    // 发送公开消息
    void sendPublicMessage(const std::string& text);

    // 发送私密消息
    void sendPrivateMessage(const std::string& target, const std::string& text);

    // 断开
    void disconnectClient();

    // 纯虚函数 回调接口
    virtual void onConnected() = 0;     // 连接成功
    virtual void onLoginSuccess() = 0;  // 登录成功
    virtual void onMessageReceived(const std::string& sender, const std::string& message) = 0; // 公开消息
    virtual void onPrivateMessage(const std::string& sender, const std::string& message) = 0;  // 私密消息
    virtual void onSystemNotice(const std::string& text) = 0;    // 系统通知
    virtual void onDisconnected() = 0;    // 连接断开
    virtual void onUserJoined(const std::string& name) {}
    virtual void onUserLeft(const std::string& name) {}

    int getSockfd() const {
        return sockfd;
    }   
    bool getRunning() const {
        return running;
    }
    

protected:
    // 接受线程主函数
    void recvLoop();

    // 发送原始数据
    bool sendRaw(const std::string& data);
    int sockfd;
    std::atomic<bool> running;
    std::thread* recvThread;
};