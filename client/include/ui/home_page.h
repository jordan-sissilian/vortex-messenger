#ifndef HOME_PAGE_H
#define HOME_PAGE_H

#include "room_ui.h"
#include "private_message_ui.h"
#include "index.h"

#include <QWidget>
#include <QListWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QStackedWidget>
#include <QIcon>
#include <QStyle>
#include <QMessageBox>
#include <QDir>
#include <QButtonGroup>
#include <QFileInfo>
#include <QDateTime>

#include <iostream>
#include <unordered_map>

#include "add_conversation_window.h"


#ifdef __linux__
#define CONTACT_DIR_PATH "/usr/share/clientcryptomadnessvortex/dir_user/contact/"
#define ROOM_DIR_PATH "/usr/share/clientcryptomadnessvortex/dir_user/conv/room/"
#elif __APPLE__
#define CONTACT_DIR_PATH "/Users/Shared/ClientCryptoMadnessVortex/dir_user/contact/"
#define ROOM_DIR_PATH "/Users/Shared/ClientCryptoMadnessVortex/dir_user/conv/room/"
#endif


enum ConvType {
    PM,
    RM
};

enum PageType {
    Index,
    Setting,
    PrivateMessage,
    Room
};

class HomePage : public QWidget {
    Q_OBJECT

public:
    explicit HomePage(QWidget *parent = nullptr);
    ~HomePage();

    PrivateMessageUi actualMp;

private:
    void initTopWidget();
    void initBottomWidget();

    void setWidget(PageType pageType, QString data);
    void setWidget(PageType pageType);

    int getButtonCountNotif(const QString& username);
    void setButtonCountNotif(const QString& username, int newCount);

    QHBoxLayout *horizontalLayout_;
    QScrollArea *scrollArea_;
    QWidget *innerWidgetBtn_;
    QVBoxLayout *conversationsButtons_;
    std::unordered_map<std::string, QPushButton*> userWaitButtonMap_;
    std::unordered_map<std::string, QPushButton*> userButtonMap_;
    QWidget *rightWidget_;
    QStackedWidget *stackedWidget_;

    QWidget *topWidget;
    QWidget *conversationsWidget;

    QWidget *addConvWidget;
    QPushButton *addConvButton;

    std::string viewName;
    RoomUi *actualRoom;

    IndexUi *index;

signals:
    void setNewConvUser(const std::string& name);
    void setNewConvRoom(const std::string& name);
    void newConvUser(const std::string& name);
    void newConvRoom(const std::string& name);
    void newMpStatus(int code, const std::string& name);

public slots:
    void addConv(ConvType convType, QString data);
    void recvRespJointRoom(int code, const std::string &nameroom);
    void newConv(const std::string &username);
    void newConvAccept(const std::string &username);
    void recvRespJoinMp(int code, const std::string &username);
    void recvMpMsg(const std::string &username, int msgId);
    void mpMsg(int msgId);
    void mpChangeStatus(int msgId, const std::string& status, unsigned int msgTimestamp);
    void mpDeleteMsg(int msgId);

    void setNewConvUserSlot(std::string);
    void setNewConvRoomSlots(std::string);
};

#endif  // HOME_PAGE_H
