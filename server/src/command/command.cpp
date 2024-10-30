#include "command.hpp"
#include <cstdio>

#ifdef INCLUDE_OPCODE
    extern "C" int callOpCodeFunc(Command *cmd, int opcode, void* clientData, std::vector<std::string>& vec);
    extern "C" Command::CommandHandler Command::function_table_op_code[4] = {
        &Command::handleAuthCommand,
        &Command::handleCreaCommand,
        &Command::handleMessCommand,
        &Command::handleDecoCommand
    };
#else
    extern "C" int callOpCodeFunc(Command *cmd, int opcode, void* clientData, std::vector<std::string>& vec) {}
#endif

Command::Command(ClientQueueThreadPool* queueClient, int port)
    : queueClient_(queueClient), client_(nullptr) {
    setupServer(port);

#ifndef INCLUDE_OPCODE
    commandHandlers_[AUTH_CODE] = &Command::handleAuthCommand;
    commandHandlers_[CHRP_CODE] = &Command::handleChrpCommand;
    commandHandlers_[CREA_CODE] = &Command::handleCreaCommand;
    commandHandlers_[DECO_CODE] = &Command::handleDecoCommand; //todo

    commandHandlers_[JNMP_CODE] = &Command::handleJnmpCommand;
    commandHandlers_[SDPK_CODE] = &Command::handleSdpkCommand;
    commandHandlers_[JMPS_CODE] = &Command::handleJmpsCommand;

    commandHandlers_[MPMS_CODE] = &Command::handleMpmsCommand;

    commandHandlers_[SACK_CODE] = &Command::handleSackCommand;
#endif
}

void Command::setupServer(int port) {
    serverSocketCommand_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketCommand_ < 0) {
        serverStart_ = false;
        std::cerr << "Erreur de création du socket" << std::endl;
        return;
    }

    serverAddrCommand_.sin_family = AF_INET;
    serverAddrCommand_.sin_addr.s_addr = INADDR_ANY;
    serverAddrCommand_.sin_port = htons(port);

    if (bind(serverSocketCommand_, (struct sockaddr *)&serverAddrCommand_, sizeof(serverAddrCommand_)) < 0) {
        close(serverSocketCommand_);
        std::cerr << "Erreur lors du bind, port : " << port << " - " << std::strerror(errno) << std::endl;
        serverStart_ = false;
        return;
    }

    if (listen(serverSocketCommand_, 5) < 0) {
        close(serverSocketCommand_);
        std::cerr << "Erreur lors de l'écoute, port : " << port << " - " << std::strerror(errno) << std::endl;
        serverStart_ = false;
        return;
    }

    serverStart_ = true;
    std::cout << "Serveur démarré, Port : " << port << std::endl;
}

int Command::getServerSocket() const {
    return serverSocketCommand_;
}

void Command::sendToClient(ClientData* client, const StructToClient& response) {
    unsigned char buffer[4 + 16 + 1024];
    client->communicator_.prepareMessage(response, buffer);
    if (write(client->socket_fd, buffer, sizeof(buffer)) < 0) {
        std::cerr << "Erreur d'écriture sur le socket " << client->socket_fd << ": " << std::strerror(errno) << std::endl;
    }
}

bool Command::handleClient(ClientData* client) {
    unsigned char buffer[4 + 16 + 1024];
    ssize_t bytesRead = read(client->socket_fd, buffer, sizeof(buffer));
    if (bytesRead > 0) {

        int message_size;
        memcpy(&message_size, buffer, sizeof(message_size));

        unsigned char decryptedtext[1024];
        memset(decryptedtext, 0, 1024);
        bool succesDecrypt = client->communicator_.decrypt(buffer, message_size, decryptedtext);

        StuctToServ message;
        if (succesDecrypt) {
            std::memcpy(&message, decryptedtext, sizeof(StuctToServ));
            processCommand(client, message);
        } else {
            message.opcode = -1; // todo reglé ca, au changement de clef
            message.id = 0;
        }

        return (true);
    } else if (bytesRead == 0) {
        std::cout << "Client déconnecté" << std::endl;
        return (false);
    } else {
        std::cerr << "Erreur de lecture du socket " << client->socket_fd << ": " << std::strerror(errno) << std::endl;
        return (false);
    }
}

int Command::acceptClient(std::unordered_map<int, Rooms*>* rooms) {
    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    int clientSocket = accept(serverSocketCommand_, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket < 0) {
        std::cerr << "Erreur d'acceptation de la connexion: " << std::strerror(errno) << std::endl;
        return -1;
    }

    std::string clientIP = inet_ntoa(clientAddr.sin_addr);
    std::cout << "Client connecté (IP: " << clientIP << ")" << std::endl;

    StructToClient response;
    response.id = 0;
    response.opcode = CNNT_CODE;
    response.timestamp = std::chrono::system_clock::now();
    response.setStatus(CodeResponseStatus::Ok, 200);

    SecureServerCommunicator communicator;
    unsigned char buffer[4 + 16 + 1024];
    std::memset(buffer, 0, sizeof(buffer));
    communicator.prepareMessage(response, buffer);

    if (write(clientSocket, buffer, 1044) < 0) {
        return -1;
    }

    //rooms_ = rooms;
    return clientSocket;
}

void Command::processCommand(ClientData* client, const StuctToServ& message) {
    client_ = client;

    std::cout << "Op Code     : " << message.opcode << std::endl;
    std::cout << "id command     : " << message.id << std::endl;
    std::cout << std::endl;

#ifdef INCLUDE_OPCODE
    callOpCodeFunc(this, 0x01, client, commands);
#else

    StructToClient response;
    response.id = message.id;
    response.opcode = NONE_CODE;

    auto itCommand = commandHandlers_.find(message.opcode);
    if (itCommand != commandHandlers_.end()) {
        queueClient_->enqueueClientTask(client->socket_fd, [this, client, message, &response, itCommand]() {

            if (!client->authenticated && (message.opcode == AUTH_CODE || message.opcode == CREA_CODE || message.opcode == CHRP_CODE) || client->authenticated) {
                (this->*(itCommand->second))(client, message);
            } else {
                response.setStatus(CodeResponseStatus::Error, 550);
                sendToClient(client, response);
            }
        });
    } else {
        response.setStatus(CodeResponseStatus::Error, 500);
        sendToClient(client, response);
    }

#endif
}

bool Command::isUserExists(const std::string& username) {
    std::string filePath = std::string(STORAGE_PATH) + username + ".key";
    struct stat buffer;
    return (stat(filePath.c_str(), &buffer) == 0);
}