#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QListWidget>
#include <QSplitter>

class QtChatClient;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QtChatClient *client, const QString& myName, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSendClicked();
    void appendPublicMessage(const QString& sender, const QString& message);
    void appendPrivateMessage(const QString& sender, const QString& message);
    void appendSystemNotice(const QString& text);
    void onConnectionLost();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QtChatClient* chatClient;
    QTextEdit* chatView;
    QLineEdit* inputLine;
    QPushButton* sendBtn;
    QListWidget* userList;
    QString myUsername;
};

#endif