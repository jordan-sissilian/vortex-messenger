#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "auth_page.h"
#include "home_page.h"
#include "user_controller.h"
#include "title_bar.h"
#include "message_manager.hpp"

#include <QMainWindow>
#include <QScreen>
#include <QMouseEvent>
#include <QEvent>
#include <QApplication>
#include <QButtonGroup>

class UserController;
struct Message;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(UserController *userController, QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void handleNewConnection(); 

protected:
    void changeEvent(QEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    TitleBar *titleBar;
    AuthPage *authPage;
    HomePage *homePage;
    QWidget *bottomBorderWidget;

    UserController *userController;

    QWidget *leftFrame;
    QWidget *rightFrame;

signals:
    void windowClicked();
};

#endif // MAIN_WINDOW_H
