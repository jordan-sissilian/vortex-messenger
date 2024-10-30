#pragma once

#include "client_data.hpp"
#include "rooms.hpp"
#include "command.hpp"
#include "poller.hpp"
#include "client_queue_thread_pool.hpp"

#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cerrno>
#include <cstring>
#include <future>
#include <sys/stat.h>
#include <errno.h>
#include <chrono>
#include <arpa/inet.h>

class Command;
struct ClientData;

class Server {
public:
    Server(int portCommand = 9999);
    ~Server();

    void run();
    void stop();

    std::unordered_map<int, Rooms*> rooms_;

private:
    void closeAll();
    void handleNewConnection();
    void handleClientData(int fd);
    void handleClientDisconnection(int fd);
    void connectToRoom(int fd);

    bool serverStart_;
    std::vector<ClientData> clients_;
    Poller *poller_;
    ClientQueueThreadPool *queueClient_;

    int port_;
    int clientConnected_;
    
    Command *commandServer_;
};
