#include "logindialog.h"
#include <QMessageBox>
#include <QApplication>
#include <QScreen>

LoginDialog::LoginDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("chatroom");
    setFixedSize(400, 200);

    titleLabel = new QLabel("💬 聊天室登录", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    nameEdit = new QLineEdit(this);
    nameEdit->setPlaceholderText("请输入用户名");
    nameEdit->setMinimumHeight(30);

    errorLabel = new QLabel(this);
    errorLabel->setStyleSheet("color: red");
    errorLabel->setVisible(false);

    loginBtn = new QPushButton("进入聊天室", this);
    loginBtn->setMinimumHeight(35);
    loginBtn->setDefault(true);

    QBoxLayout *layout = new QVBoxLayout(this);
    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addSpacing(15);
    layout->addWidget(nameEdit);
    layout->addWidget(errorLabel);
    layout->addSpacing(10);
    layout->addWidget(loginBtn);
    layout->addStretch();

    connect(loginBtn, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(nameEdit, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);

    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->availableGeometry();
        move((screenGeometry.width() - width()) / 2, (screenGeometry.height() - height()) / 2);
    }
}

QString LoginDialog::username() const
{
    return nameEdit->text().trimmed();
}

void LoginDialog::onLoginClicked() {
    if (username().isEmpty()) {
        errorLabel->setText("用户名不为空!");
        errorLabel->setVisible(true);
        return;
    }
    // TODO 可以加更多的校验

    accept();

}
