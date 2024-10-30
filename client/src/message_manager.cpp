#include "message_manager.hpp"
#include <chrono>

MessageManager::MessageManager(const std::string& filePath) : filePath_(filePath) {}

void MessageManager::addMessage(const Message& message) {
    std::ofstream file(filePath_, std::ios::binary | std::ios::app);
    if (!file) {
        std::cerr << "Erreur lors de l'ouverture du fichier pour écriture." << std::endl;
        return;
    }
    file.write(reinterpret_cast<const char*>(&message), sizeof(Message));
    file.close();
}

Message MessageManager::getMessage(int msgId, bool from) {
    std::ifstream file(filePath_, std::ios::binary);
    Message message;

    if (!file.is_open()) {
        return Message{};
    }

    while (file.read(reinterpret_cast<char*>(&message), sizeof(Message))) {
        if (message.id == msgId && message.isFromMe == from) {
            file.close();
            return message;
        }
    }

    file.close();
    return Message{};
}

void MessageManager::messageChangeStatus(int msgId, int status) {
    std::vector<Message> messages = readMessages();

    std::ofstream file(filePath_, std::ios::binary);
    if (!file) {
        std::cerr << "Erreur lors de l'ouverture du fichier pour suppression." << std::endl;
        return;
    }

    for (auto& message : messages) {
        if (message.id != msgId) {
            file.write(reinterpret_cast<const char*>(&message), sizeof(Message));
        } else {
            message.status = (MessageStatus)status;
            file.write(reinterpret_cast<const char*>(&message), sizeof(Message));
        }
    }
    file.close();
}

std::vector<Message> MessageManager::readMessages() {
    std::vector<Message> messages;
    std::ifstream file(filePath_, std::ios::binary);
    if (!file) {
        std::cerr << "Erreur lors de l'ouverture du fichier pour lecture." << std::endl;
        return messages;
    }

    Message message;
    while (file.read(reinterpret_cast<char*>(&message), sizeof(Message))) {
        messages.push_back(message);
    }
    file.close();
    return messages;
}

void MessageManager::deleteMessage(int messageId) {
    std::vector<Message> messages = readMessages();

    std::ofstream file(filePath_, std::ios::binary);
    if (!file) {
        std::cerr << "Erreur lors de l'ouverture du fichier pour suppression." << std::endl;
        return;
    }

    for (const auto& message : messages) {
        if (message.id != messageId) {
            file.write(reinterpret_cast<const char*>(&message), sizeof(Message));
        }
    }
    file.close();
}

void MessageManager::printMessages() {
    auto messages = readMessages();
    for (const auto& msg : messages) {
        std::cout << "\nID         : " << msg.id 
                << "\nMessage    : " << msg.content 
                << "\nUser       : " << (msg.isFromMe ? "Me" : "User Distant") 
                << "\nTimestamp  : " << std::ctime(&msg.timestamp) 
                << "Status     : " << statusToString(msg.status) 
                << "\nEphemeral  : " << (msg.ephemeralDuration == -1 ? "No" : std::to_string(msg.ephemeralDuration) + "s") 
                << std::endl;
    }
}

std::string MessageManager::statusToString(MessageStatus status) {
    switch (status) {
        case MessageStatus::Sent:
            return "Envoyer.";
        case MessageStatus::Received:
            return "Recu.";
        case MessageStatus::Read:
            return "Lu.";
        default:
            return "Unknown";
    }
}
