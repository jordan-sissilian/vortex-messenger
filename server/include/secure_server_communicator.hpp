#pragma once

#include "universal_structs_server.hpp"

#include <iostream>
#include <cstring>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <string>
#include <fstream>


#ifdef __linux__
#define KEY_CLIENT_FILE "/usr/share/servercryptomadnessvortex/server/users_key_serv/"
#elif __APPLE__
#define KEY_CLIENT_FILE "/Users/Shared/ServerCryptoMadnessVortex/server/users_key_serv/"
#endif

class SecureServerCommunicator {
public:
    SecureServerCommunicator();
    size_t encrypt(const unsigned char* plaintext, size_t size, unsigned char* ciphertext, unsigned char* iv);
    bool decrypt(const unsigned char* buffer, size_t size, unsigned char* plaintext);
    void prepareMessage(const StructToClient& msg, unsigned char* buffer);
    unsigned char * createNewKey();
    void rotateKey(unsigned char *newKey);
    unsigned char *getKey();

    std::string username_;
private:
    static const int KEY_SIZE_ = 16; 
    unsigned char key_[KEY_SIZE_];
};