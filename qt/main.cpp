#include <QApplication>
#include <QInputDialog>
#include <QMessageBox>
#include "mainwindow.h"
#include "qt_chat_client.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QtChatClient client;

    if (!client.connectToServer("127.0.0.1", 8080)) {
        QMessageBox::critical(nullptr, "错误", "无法连接服务器");
        return 1;
    }

    bool st;
    QString username = QInputDialog::getText(nullptr, "登录", "请输入用户名:", QLineEdit::Normal, "", &st);

    if (!st || username.trimmed().isEmpty()) {
        return 0;
    }

    client.login(username.toStdString());

    MainWindow win(&client);
    win.show();

    return app.exec();
}