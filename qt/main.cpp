#include <QApplication>
#include <QInputDialog>
#include <QMessageBox>
#include "mainwindow.h"
#include "qt_chat_client.h"
#include "logindialog.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // 设置全局字体
    QFont font("Noto Sans CJK SC", 9);
    if (font.exactMatch()) {
        app.setFont(font);
    } else {
        font = QFont("WenQuanYi Micro Hei", 9);
        app.setFont(font);
    }

    app.setStyleSheet(R"(
    QMainWindow {
        background-color: #f0f0f0;
    }
    QTextEdit {
        background-color: white;
        border: 1px solid #ccc;
        border-radius: 4px;
    }
    QLineEdit {
        border: 1px solid #aaa;
        border-radius: 4px;
        padding: 4px;
    }
    QPushButton {
        background-color: #4CAF50;
        color: white;
        border: none;
        border-radius: 4px;
        padding: 6px 16px;
    }
    QPushButton:hover {
        background-color: #45a049;
    }
    QListWidget {
        border: 1px solid #ccc;
        border-radius: 4px;
        background-color: white;
    }
    )");



    QtChatClient client;

    if (!client.connectToServer("127.0.0.1", 8080)) {
        QMessageBox::critical(nullptr, "错误", "无法连接服务器");
        return 1;
    }

    LoginDialog loginDialog;
    if (loginDialog.exec() != QDialog::Accepted) {
        return 0;
    }
    QString username = loginDialog.username();



    MainWindow win(&client, username);
    win.show();

    client.login(username.toStdString());

    return app.exec();
}