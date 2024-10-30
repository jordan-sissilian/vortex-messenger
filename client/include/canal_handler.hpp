#ifndef CANAL_HANDLER_HPP
#define CANAL_HANDLER_HPP

#include <string>
#include <random>
#include <cstdio>

#include <osrng.h>
#include <hex.h>
#include <filters.h>
#include <scrypt.h>
#include <sha3.h>

#include "message_handler.hpp"
#include "queu_manager.hpp"
#include "message_manager.hpp"

#include "user_controller.h"
class UserController;

#include "crypted_msg.hpp"
#include "secure_server_communicator.hpp"
#include "responseToServerStruct.hpp"

#define NONE_CODE 0x00

#include "universal_structs_client.hpp"
//server OpCode
#define AUTH_CODE 0x01 // auth                      //ok
#define CREA_CODE 0x02 // create account            //ok
#define DECO_CODE 0x03 // deco                      //todo
#define MPMS_CODE 0x04 // private message send      //ok
#define RMSG_CODE 0x05 // room    message send      //todo
#define CRMS_CODE 0x06 // create room               //todo
#define JRMS_CODE 0x07 // join   room               //todo
#define JNMP_CODE 0x08 // join   mp                 //ok
#define SDPK_CODE 0x09 // send public key           //ok
#define JMPS_CODE 0xa // send joint mp status       //ok
#define CHRP_CODE 0xb // send challenge reponse     //ok
#define SACK_CODE 0xc // ack code                   //ok

#include "responseToServerStruct.hpp"
//client OpCode
#define CNNT_CODE 0x01 // connected to server       //ok
#define RVUM_CODE 0x02 // recive user message       //ok
#define RVRM_CODE 0x03 // recive room message       //todo
#define NJMP_CODE 0x04 // new    join mp            //ok
#define SJMP_CODE 0x05 // status join mp            //ok
#define RKEY_CODE 0x06 // rotate key                //ok
#define RACK_CODE 0x07 // ack code                  //ok

#include "client.hpp"
class Client;

using CommandCallback = std::variant<
    std::function<void(int)>,
    std::function<void(int, const std::string&)>,
    std::function<void(int, const std::string&, const std::string&)>,
    std::monostate
>;

class CanalHandler {
    public:
        CanalHandler(MessageHandler *msgHandler, int socket_fd, std::function<void(uint32_t, CommandCallback)> addCommandFunc, UserController *userController);

        void handleAuth(const std::string& username, const std::string& password);
        void handleChallengeReponse(const std::string& salt, const std::string& challenge);
        void handleCrea(const std::string& username, const std::string& password);
        void handleQuit();
        void handleMpMsg(const std::string &username, const std::string &message);
        void handleRoomMsg(const std::string &message);
        void handlecreateRoom(const std::string &nameRoom);
        void handlejoinRoom(const std::string &nameRoom);
        void handlejoinMp(const std::string &nameUser);
        void sendJointMpStatus(int status, const std::string &nameUser);
        void handleViewMsg(const std::string& nameUser, std::time_t timestampActual);


        void callbackAuth(int code, const std::string& salt, const std::string &challenge);
        void callbackChallengeReponse(int code);
        void callbackCrea(int code);
        void callbackMpms(int code);
        void callbackRmsg(int code);
        void callbackCrms(int code);
        void callbackJrms(int code);
        void callbackJnmp(int code, const std::string &username);

    private:
        int socket_fd;
        std::string username_;
        std::function<void(uint32_t, CommandCallback)> addCommandFunc_;
        UserController *userController_;

        MessageHandler *msgHandler_;
        StuctToServ msg_;
        RSAEncryption rsaEncryption_;
        QueueManager queueManager_;
};

#endif 
