#pragma once

#include <string>
#include <memory>

#include "secure_server_communicator.hpp"

struct ClientData {
    int socket_fd;

    std::string client_adress_ip;
    std::string username;

    bool authenticated;

    void* room_join;

    SecureServerCommunicator communicator_;
    int msg_send;

    std::string challengeTmp;

    void reset() {
        authenticated = false;
        client_adress_ip = "";
        username = "";
    }

    ClientData(int fd = -1) :
        socket_fd(fd),
        client_adress_ip(""),
        username(""),
        authenticated(false),
        communicator_(),
        msg_send(0)
    {}
};

