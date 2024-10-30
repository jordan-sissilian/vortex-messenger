#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <string>

class Network {
public:
    int connectToServer(const std::string& server_ip, int port);
    void closeConnection(int socket);
};

#endif
 