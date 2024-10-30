#include "server.hpp"

Server::Server(int portCommand) {
    poller_ = new Poller();
    queueClient_ = new ClientQueueThreadPool(10);
    port_ = portCommand;
    commandServer_ = nullptr;

    rooms_.emplace(rooms_.size() + 1, new Rooms(rooms_.size() + 1));
}

Server::~Server() {
    closeAll();
}

void Server::stop() {
    queueClient_->stop();
    serverStart_ = false;
}

void Server::run() {
    serverStart_ = true;
    commandServer_ = new Command(queueClient_, port_);
    commandServer_->isConnected = [&](const std::string& username) -> std::optional<ClientData> {
        for (const auto& client : clients_) {
            if (client.username == username) {
                return std::optional<ClientData>(client);
            }
        }
        return std::nullopt;
    };
    poller_->add(commandServer_->getServerSocket());

    while (serverStart_ && commandServer_->serverStart_) {
        poller_->wait([this](int fd) {
            if (fd == commandServer_->getServerSocket()) {
                handleNewConnection();
            } else {
                handleClientData(fd);
            }
        }, std::chrono::milliseconds(500));
    }
}

void Server::handleNewConnection() {
    int clientSocket = commandServer_->acceptClient(&rooms_);

    if (clientSocket >= 0) {
        clients_.emplace_back(clientSocket);
        poller_->add(clientSocket);

        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        memset(&clientAddr, 0, clientAddrLen);
        getpeername(clientSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        char ipStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, ipStr, sizeof(ipStr));
    }
}

void Server::handleClientData(int fd) {
    bool found = false;
    for (auto& client : clients_) {
        if (client.socket_fd == fd) {
            found = true;
            if (!commandServer_->handleClient(&client)) {
                handleClientDisconnection(fd);
            }
        break;
        }
    }
}

void Server::handleClientDisconnection(int fd) {
    auto it = std::remove_if(clients_.begin(), clients_.end(), [fd](const ClientData& client) {
        return client.socket_fd == fd;
    });

    if (it != clients_.end()) {
        close(fd);
        clients_.erase(it, clients_.end());
        poller_->remove(fd);
    }
}

void Server::closeAll() {
    for (auto& client : clients_) {
        if (client.socket_fd >= 0) {
            close(client.socket_fd);
        }
    }

    clients_.clear();

    if (poller_) {
        poller_->remove(commandServer_->getServerSocket());
        delete poller_;
        poller_ = nullptr;
    }

    if (queueClient_) {
        queueClient_->stop();
        delete queueClient_;
        queueClient_ = nullptr;
    }
}


