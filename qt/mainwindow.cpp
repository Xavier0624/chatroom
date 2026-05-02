#include "mainwindow.h"
#include "qt_chat_client.h"
#include <QDateTime>


MainWindow::MainWindow(QtChatClient* client, QWidget* parent)
    : QMainWindow(parent), chatClient(client)
{
    setWindowTitle("chatclient");
    resize(500, 400);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    chatView = new QTextEdit(this);
    chatView->setReadOnly(true);

    inputLine = new QLineEdit(this);
    inputLine->setPlaceholderText("输入消息, @用户名 私聊, exit退出");

    sendBtn = new QPushButton("发送", this);

    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->addWidget(new QLabel("聊天记录:"));
    layout->addWidget(chatView);
    layout->addWidget(inputLine);
    layout->addWidget(sendBtn);
    
    // 连接客户端信号到窗口槽函数

    connect(chatClient, &QtChatClient::newPublicMessage, this, &MainWindow::appendPublicMessage);
    connect(chatClient, &QtChatClient::newPrivateMessage, this, &MainWindow::appendPrivateMessage);
    connect(chatClient, &QtChatClient::newSystemNotice, this, &MainWindow::appendSystemNotice);
    connect(chatClient, &QtChatClient::connectionLost, this, &MainWindow::onConnectionLost);

    // 发送按钮
    connect(sendBtn, &QPushButton::clicked, this, &MainWindow::onSendClicked);
    // 回车
    connect(inputLine, &QLineEdit::returnPressed, this, &MainWindow::onSendClicked);
}

MainWindow::~MainWindow() {}

void MainWindow::appendPublicMessage(const QString &sender, const QString &message)
{
    chatView->append(QString("<b>%1</b>: %2").arg(sender).arg(message));
}

void MainWindow::appendPrivateMessage(const QString &sender, const QString &message)
{
    chatView->append(QString("<i>[私聊] %1</i>: %2").arg(sender).arg(message));

}

void MainWindow::appendSystemNotice(const QString &text)
{
    chatView->append(QString("<font color = 'blue'>%1</font>").arg(text));

}

void MainWindow::onConnectionLost()
{
    chatView->append(QString("<font color = 'red'>与服务器断开连接</font>"));
    sendBtn->setEnabled(false);
    inputLine->setEnabled(false);
}

void MainWindow::onSendClicked() {
    QString text = inputLine->text().trimmed();
    if (text.isEmpty()) return;

    if (text == "exit") {
        chatClient->disconnectClient();
        close();
        return;
    }

    if (text.startsWith('@')) {
        int spacePos = text.indexOf(' ');
        if (spacePos != -1) {
            QString target = text.mid(1, spacePos - 1);
            QString msg = text.mid(spacePos + 1);
            chatClient->sendPrivateMessage(target.toStdString(), msg.toStdString());

        }
    } else {
            chatClient->sendPublicMessage(text.toStdString());
    }
    inputLine->clear();

}

