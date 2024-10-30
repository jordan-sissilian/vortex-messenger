#ifndef USER_CONTROLLER_H
#define USER_CONTROLLER_H

#include "message_handler.hpp"
#include "canal_handler.hpp"

#include <QWidget>
#include <QListWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

class MessageHandler;
class CanalHandler;

class UserController : public QObject {
    Q_OBJECT

public:
    explicit UserController(QObject *parent = nullptr);

    void setCanal(CanalHandler *canal);
    void setMessage(MessageHandler *message);

    void emitStartToServer();
    void recvMpMsg(const std::string &username, int msg);
    void recvRoomMsg(const std::string &roomname, const std::string &msg);

private:
    CanalHandler *canal;
    MessageHandler *message;

signals:
    void connectToServer();
    int sendLoginCode(int code);
    int sendRegisterCode(int code);
    int sendMpMsgCode(int code);
    int sendRoomMsgCode(int code);
    int createRoomCode(int code);
    int joinRoomrCode(int code);
    int joinMpCode(int code, const std::string &username);
    int recvMpMsgEmit(const std::string &username, int msg);
    int mpMsgEmit(int msg);
    int recvRoomMsgEmit(const std::string &roomname, const std::string &msg);
    void setNewConv(const std::string &username);
    void setNewUserJoin(const std::string &username);
    void setUserJoinStatus(int code, const std::string &username);
    void mpChangeStatusEmit(int msgId, const std::string &status, unsigned int msgTimestamp);
    void mpDeleteMsgEmit(int msgId);

public slots:
    void sendRegister(const std::string &username, const std::string &password);
    void sendLogin(const std::string &username, const std::string &password);
    void sendMpMsg(const std::string &username, const std::string &msg);
    void viewMsg(const std::string& nameUser, std::time_t timestampActual);
    void sendRoomMsg(const std::string &msg);
    void createRoom(const std::string &nameRoom);
    void joinRoom(const std::string &nameRoom);
    void joinMp(const std::string &nameUser);
    void sendJointMpStatus(int status, const std::string &uerName);

};

#endif  // USER_CONTROLLER_H
