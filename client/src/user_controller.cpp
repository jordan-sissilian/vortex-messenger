#include "user_controller.h"

UserController::UserController(QObject *parent) : QObject(parent) { }

//init
void UserController::setCanal(CanalHandler *canal) {
    this->canal = canal;
}

void UserController::setMessage(MessageHandler *message) {
    this->message = message;
}

// start
void UserController::emitStartToServer() {
    emit connectToServer();
}

// register / login
void UserController::sendRegister(const std::string &username, const std::string &password) {
    canal->handleCrea(username, password);
}

void UserController::sendLogin(const std::string &username, const std::string &password) {
    canal->handleAuth(username, password);
}

// sendMpMsg, sendRoomMsg
void UserController::sendMpMsg(const std::string &username, const std::string &msg) {
    canal->handleMpMsg(username, msg);
}

void UserController::sendRoomMsg(const std::string &msg) {
    canal->handleRoomMsg(msg);
}

// createRoom, joinRoom, joinMp, sendJointMpStatus
void UserController::createRoom(const std::string &nameRoom) {
    canal->handlecreateRoom(nameRoom);
}

void UserController::joinRoom(const std::string &nameRoom) {
    canal->handlejoinRoom(nameRoom);
}

void UserController::joinMp(const std::string &nameUser) {
    canal->handlejoinMp(nameUser);
}

void UserController::sendJointMpStatus(int status, const std::string &username) {
    canal->sendJointMpStatus(status, username);
}

// recvMpMsg, recvRoomMsg
void UserController::recvMpMsg(const std::string &username, int msg) {
    emit recvMpMsgEmit(username, msg);
}

void UserController::recvRoomMsg(const std::string &roomname, const std::string &msg) {
    emit recvRoomMsgEmit(roomname, msg);
}


void UserController::viewMsg(const std::string& nameUser, std::time_t timestampActual) {
    canal->handleViewMsg(nameUser, timestampActual);
}
