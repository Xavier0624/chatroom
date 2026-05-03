#include "mainwindow.h"
#include "qt_chat_client.h"
#include <QCloseEvent>
#include <QDateTime>


MainWindow::MainWindow(QtChatClient* client, const QString& myName, QWidget* parent)
    : QMainWindow(parent), myUsername(myName), chatClient(client)
{
    setWindowTitle("chatclient");
    resize(600, 450);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    // 聊天记录
    chatView = new QTextEdit(this);
    chatView->setReadOnly(true);

    // 在线用户列表
    userList = new QListWidget(this);
    userList->setMaximumWidth(120);

    QLabel *userLabel = new QLabel("在线用户");
    QVBoxLayout *userLayout = new QVBoxLayout;
    userLayout->addWidget(userLabel);
    userLayout->addWidget(userList);

    // 输入
    inputLine = new QLineEdit(this);
    inputLine->setPlaceholderText("输入消息, @用户名 私聊, exit退出");
    inputLine->setFocus();  // 设置自动获取焦点

    sendBtn = new QPushButton("发送", this);

    // 整体布局
    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(chatView);
    topLayout->addLayout(userLayout);



    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(inputLine);
    mainLayout->addWidget(sendBtn);
    
    // 连接客户端信号到窗口槽函数

    connect(chatClient, &QtChatClient::newPublicMessage, this, &MainWindow::appendPublicMessage);
    connect(chatClient, &QtChatClient::newPrivateMessage, this, &MainWindow::appendPrivateMessage);
    connect(chatClient, &QtChatClient::newSystemNotice, this, &MainWindow::appendSystemNotice);
    connect(chatClient, &QtChatClient::connectionLost, this, &MainWindow::onConnectionLost);
    connect(chatClient, &QtChatClient::userJoined, this, [this](const QString& name) {
        for (int i = 0; i < userList->count(); ++ i) {
            if (userList->item(i)->text() == name)
                return;
        }
        auto *item = new QListWidgetItem(name);
        if (myUsername == name) {
            QFont boldFont = item->font();
            boldFont.setBold(true);
            item->setFont(boldFont);
            userList->insertItem(0, item);
        } else {
            userList->addItem(item);
        }
    });
    connect(chatClient, &QtChatClient::userLeft, this, [this](const QString& name) {
        for (int i = 0; i < userList->count(); ++ i) {
            if (userList->item(i)->text() == name) {
                delete userList->takeItem(i);
                break;
            }
        }
    });
    


    // 发送按钮
    connect(sendBtn, &QPushButton::clicked, this, &MainWindow::onSendClicked);
    // 回车
    connect(inputLine, &QLineEdit::returnPressed, this, &MainWindow::onSendClicked);
    // 用户列表
    connect(userList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
        inputLine->setText("@" + item->text() + " ");
    });
}

MainWindow::~MainWindow() {}

void MainWindow::appendPublicMessage(const QString &sender, const QString &message)
{
    QString time = QDateTime::currentDateTime().toString("hh:mm");
    chatView->append(QString("<font color = 'black'>[%1] <b>%2</b>: %3</font>").arg(time).arg(sender).arg(message));
}

void MainWindow::appendPrivateMessage(const QString &sender, const QString &message)
{
    QString time = QDateTime::currentDateTime().toString("hh:mm");
    chatView->append(QString("<font color = 'red'><i>[%1] [私聊] %2: %3</i></font>").arg(time).arg(sender).arg(message));
}

void MainWindow::appendSystemNotice(const QString &text)
{
    QString time = QDateTime::currentDateTime().toString("hh:mm");
    chatView->append(QString("<font color = 'blue'>[%1] %2</font>").arg(time).arg(text));
}

void MainWindow::onConnectionLost()
{
    chatView->append(QString("<font color = 'red'>与服务器断开连接</font>"));
    sendBtn->setEnabled(false);
    inputLine->setEnabled(false);
    userList->clear();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    chatClient->disconnectClient();
    event->accept();
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
    inputLine->setFocus();

}

