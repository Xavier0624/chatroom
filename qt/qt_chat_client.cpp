#include "qt_chat_client.h"

QtChatClient::QtChatClient(QObject *parent)
    : QObject(parent) {}

void QtChatClient::onConnected() {

}

void QtChatClient::onLoginSuccess()
{
}

void QtChatClient::onMessageReceived(const std::string &sender, const std::string &message)
{
    emit newPublicMessage(QString::fromStdString(sender), QString::fromStdString(message));
}

void QtChatClient::onPrivateMessage(const std::string &sender, const std::string &message)
{
    emit newPrivateMessage(QString::fromStdString(sender), QString::fromStdString(message));
}

void QtChatClient::onSystemNotice(const std::string &text)
{
    emit newSystemNotice(QString::fromStdString(text));
}

void QtChatClient::onDisconnected()
{
    emit connectionLost();
}

void QtChatClient::onUserJoined(const std::string &name) {
    emit userJoined(QString::fromStdString(name));
}

void QtChatClient::onUserLeft(const std::string &name)
{
    emit userLeft(QString::fromStdString(name));
}
