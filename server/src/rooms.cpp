#include "rooms.hpp"
#include <iostream>

Rooms::Rooms(int id) : roomsId_(id) {
    monitoringThread_ = std::thread(&Rooms::monitorMessages, this);
}

Rooms::~Rooms() {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        stopRequested_ = true;
    }
    cv_.notify_all();
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
}

void Rooms::connectClient(int clientFd) {
    userConnected_.push_back(clientFd);
}

void Rooms::disconnectClient(int clientFd) {
    /*auto it = std::find(userConnected_.begin(), userConnected_.end(), clientFd);
    if (it != userConnected_.end()) {
        userConnected_.erase(it);
    }*/
}

void Rooms::setMessage(const std::string &username, const std::string& message) {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        messages_.emplace_back(username, message);
        newMessageAvailable_ = true;
    }
    cv_.notify_all();
}

int Rooms::getId() {
    return roomsId_;
}

void Rooms::sendNewMessageToClients() {
    if (!messages_.empty()) {
        std::string lastMessage = "new_msg_room " + std::to_string(roomsId_) + " " + messages_.back().first + " " + messages_.back().second;
        for (auto client : userConnected_) {
            write(client, lastMessage.c_str(), lastMessage.size());
        }
    }
}

void Rooms::monitorMessages() {
    std::unique_lock<std::mutex> lock(mtx_);
    while (!stopRequested_) {
        cv_.wait(lock, [this] { return newMessageAvailable_ || stopRequested_; });
        if (newMessageAvailable_) {
            sendNewMessageToClients();
            newMessageAvailable_ = false;
        }
    }
}
