#ifndef LOGIN_PAGE_H
#define LOGIN_PAGE_H

#include "home_page.h"

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPixmap>
#include <QHBoxLayout>
#include <QFrame>
#include <QCursor>
#include <QMessageBox>

class AuthPage : public QFrame {
    Q_OBJECT

public:
    explicit AuthPage(QWidget *parent = nullptr);
    ~AuthPage();

private:
    QVBoxLayout *mainLayout;
    QPushButton *switchButton;
    bool switchButtonStatus;

    QLineEdit *usernameInput;
    QLineEdit *passwordInput;

    QPushButton *sendButton;

    void handleLogin();
    void handleRegister();

signals:
    void newLogin(const std::string& username, const std::string& password);
    void newRegister(const std::string& username, const std::string& password);
    void LoginOk();

public slots:
    void responseLogin(int code);
    void responseRegister(int code);

};

#endif  // LOGIN_PAGE_H

