#pragma once

#include <string>

#include "secure_server_communicator.hpp"

class SecureServerCommunicator;

class MessageHandler {
    public:
        MessageHandler();
        void sendMessage(int socket, const StuctToServ& response);
        StructToClient receiveMessage(int socket);
    
        SecureServerCommunicator communicator_;
};
