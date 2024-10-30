#ifndef PRIVATE_MESSAGEUI_H
#define PRIVATE_MESSAGEUI_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QFontMetrics>
#include <QScrollBar>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QPair>
#include <QList>
#include <QIcon>
#include <QButtonGroup>
#include <QPointer>

#include "message_manager.hpp"

#include "settings_message_ui.h"

struct msgQT {
    QVBoxLayout* messageContainerLayout;
    QLabel* msgLabel;
    QLabel* status;
    QLabel* timestampLabel;
    QLabel* timeDurationLabel;
    time_t timestamp;
    bool isFromMe;

    void deleteMsg() {
        msgLabel->setStyleSheet("background-color: transparent; color: transparent; border: none;");
        status->setStyleSheet("background-color: transparent; color: transparent; border: none;");
        timestampLabel->setStyleSheet("background-color: transparent; color: transparent; border: none;");
    }
};

#ifdef __linux__
#define CONTACT_MP_DIR_PATH "/usr/share/clientcryptomadnessvortex/dir_user/conv/mp/"
#elif __APPLE__
#define CONTACT_MP_DIR_PATH "/Users/Shared/ClientCryptoMadnessVortex/dir_user/conv/mp/"
#endif

class PrivateMessageUi : public QWidget {
    Q_OBJECT

public:
    explicit PrivateMessageUi(std::string nameUser, QWidget *parent = nullptr);
    ~PrivateMessageUi();

    void setNewMp(const std::string& username);
    void processFiles();

private:
    void initaliseTop();
    void initaliseChatScrollArea();
    void initaliseSendMsg();
    void createMessageWidget(QVBoxLayout* scrollLayout, Message msg, Qt::Alignment alignment);

    QVBoxLayout *mainLayout;
    QVBoxLayout *scrollLayout;
    QWidget *bottomWidget;

    std::string nameUser_;
    QLabel *textNameUser;

    QList<QPair<int, msgQT>> labelList;

    QPointer<SettingsMessageUi> settingsMsg;

signals:
    void sendMp(const std::string& nameUser, const std::string& msg);
    void viewMsg(const std::string& nameUser, std::time_t timestampActual);

public slots:
    void setMsg(int msgId);
    void setUserMsg(const std::string& nameUser, int msgId);
    void changeStatus(int msgId, const std::string& status, unsigned int msgTimestamp);
    void deleteMsg(int msgId);

};

#endif // PRIVATE_MESSAGEUI_H
