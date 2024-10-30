#include "network.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

int Network::connectToServer(const std::string& server_ip, int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error: Could not create socket." << std::endl;
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << "Error: Invalid address." << std::endl;
        return -1;
    }

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error: Could not connect to server." << std::endl;
        return -1;
    }

    return sockfd;
}

void Network::closeConnection(int socket) {
    close(socket);
}
