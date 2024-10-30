#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstring>
#include <unistd.h>
#include <algorithm>

class Rooms {
public:
    Rooms(int id);
    ~Rooms();

    void setMessage(const std::string &username, const std::string& message);
    void sendNewMessageToClients();
    void connectClient(int clientFd);
    void disconnectClient(int clientFd);
    int getId();

private:
    void monitorMessages();

    int roomsId_;
    std::vector<std::pair<std::string, std::string>> messages_;
    std::vector<int> userConnected_;
    std::mutex mtx_;
    std::condition_variable cv_;
    bool newMessageAvailable_ = false;
    bool stopRequested_ = false;
    std::thread monitoringThread_;
};

