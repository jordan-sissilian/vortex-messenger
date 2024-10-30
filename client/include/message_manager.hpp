#pragma once

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <ctime>
#include <thread>
#include <mutex>
#include <condition_variable>

enum class MessageStatus {
    Sent,
    Received,
    Read
};

struct Message {
    int id;                 // Identifiant unique du message
    bool isFromMe;          // Identifier si c'est l'user ou l'user distant
    char content[200];      // Contenu du message
    time_t timestamp;       // Timestamp du message
    MessageStatus status;   // Statut du message
    int ephemeralDuration;  // Durée en secondes pour les messages éphémères, -1 si non éphémère
};

class MessageManager {
public:
    MessageManager(const std::string& filePath);
    void addMessage(const Message& message);
    std::vector<Message> readMessages();
    Message getMessage(int msgId, bool from);
    void deleteMessage(int messageId);
    void messageChangeStatus(int msgId, int status);
    void printMessages();
    std::string statusToString(MessageStatus status);

private:
    std::string filePath_;
    std::thread ephemeralThread_;

};

