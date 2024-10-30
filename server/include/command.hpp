#pragma once

#include "server.hpp"
#include "client_data.hpp"
#include "thread_pool.hpp"
#include "client_queue_thread_pool.hpp"
#include "define.hpp"
#include "secure_server_communicator.hpp"
#include "rsa_encryption.hpp"

#include <map>
#include <vector>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <sys/utsname.h>
#include <sstream>
#include <iomanip>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h> 
#include <regex>

struct ClientData;

#define NONE_CODE 0x00

#include "universal_structs_server.hpp"
//server OpCode
#define AUTH_CODE 0x01
#define CREA_CODE 0x02
#define DECO_CODE 0x03
#define MPMS_CODE 0x04
#define JNMP_CODE 0x08 // join   mp                 //ok
#define SDPK_CODE 0x09 // send public key           //ok
#define JMPS_CODE 0xa  // send joint mp status      //ok
#define CHRP_CODE 0xb // send challenge reponse     //ok
#define SACK_CODE 0xc // ack code                   //ok

#include "responseToClientStruct.hpp"
//client OpCode
#define CNNT_CODE 0x01
#define RVUM_CODE 0x02
#define RVRM_CODE 0x03
#define NJMP_CODE 0x04 // new    join mp            //ok
#define SJMP_CODE 0x05 // status join mp            //ok
#define RKEY_CODE 0x06 // rotate key                //ok
#define RACK_CODE 0x07 // ack code                  //ok

#ifdef __linux__
#define STORAGE_PATH "/usr/share/servercryptomadnessvortex/server/users_key/"
#define QUEU_PATH "/usr/share/servercryptomadnessvortex/server/queue/"
#elif __APPLE__
#define STORAGE_PATH "/Users/Shared/ServerCryptoMadnessVortex/server/users_key/"
#define QUEU_PATH "/Users/Shared/ServerCryptoMadnessVortex/server/queue/"
#endif

class Command {
public:
    Command(ClientQueueThreadPool* threadPool = nullptr, int port = 9999);
    ~Command() {};

    int acceptClient(std::unordered_map<int, Rooms*> *rooms);
    int getServerSocket() const;
    bool handleClient(ClientData* client);
    void sendToClient(ClientData *client, const StructToClient& message);
    bool isUserExists(const std::string& username);

    void handleAuthCommand(ClientData* client, const StuctToServ& message);
    void handleChrpCommand(ClientData* client, const StuctToServ& message);
    void handleCreaCommand(ClientData* client, const StuctToServ& message);
    void handleMessCommand(ClientData* client, const StuctToServ& message) {};
    void handleDecoCommand(ClientData* client, const StuctToServ& message);
    void handleJnmpCommand(ClientData* client, const StuctToServ& message);
    void handleSdpkCommand(ClientData* client, const StuctToServ& message);
    void handleJmpsCommand(ClientData* client, const StuctToServ& message);
    void handleMpmsCommand(ClientData* client, const StuctToServ& message);
    void handleSackCommand(ClientData* client, const StuctToServ& message);

    using CommandHandler = void (Command::*)(ClientData*, const StuctToServ& message);
    #ifndef INCLUDE_OPCODE
        std::map<int, CommandHandler> commandHandlers_;
    #else
        static CommandHandler function_table_op_code[4];
    #endif

    bool serverStart_;
    std::function<std::optional<ClientData>(const std::string&)> isConnected;

private:

    void setupServer(int port);
    void processCommand(ClientData* client, const StuctToServ& message);

    int serverSocketCommand_;
    struct sockaddr_in serverAddrCommand_;

    ClientQueueThreadPool* queueClient_;
    ClientData* client_;

    std::vector<int> portUse_;

    std::unordered_map<int, Rooms*> *rooms_;
};
