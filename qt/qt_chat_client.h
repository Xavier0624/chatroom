#ifndef QT_CHAT_CLIENT_H
#define QT_CHAT_CLIENT_H

#include <QObject>
#include "chat_engine.h"

class QtChatClient : public QObject, public ChatClient {
    Q_OBJECT
public:
    explicit QtChatClient(QObject *parent = nullptr);

    // 实现纯虚函数
    void onConnected() override;
    void onLoginSuccess() override;
    void onMessageReceived(const std::string& sender, const std::string& message) override;
    void onPrivateMessage(const std::string& sender, const std::string& message) override;
    void onSystemNotice(const std::string& text) override;
    void onDisconnected() override;
    void onUserJoined(const std::string& name) override;
    void onUserLeft(const std::string& name) override;
    

signals:
    // 通知 GUI 更新信号
    void newPublicMessage(QString sender, QString message);
    void newPrivateMessage(QString sender, QString message);
    void newSystemNotice(QString text);
    void connectionLost();
    void userJoined(QString name);
    void userLeft(QString name);
};


#endif