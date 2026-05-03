#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>


class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr);
    QString username() const;

private slots:
    void onLoginClicked();

private:
    QLineEdit *nameEdit;
    QPushButton *loginBtn;
    QLabel *titleLabel;
    QLabel *errorLabel;
};


#endif