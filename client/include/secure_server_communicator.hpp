#pragma once

#include "universal_structs_client.hpp"

#include <iostream>
#include <cstring>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <string>
#include <fstream>


#ifdef __linux__
#define KEY_CLIENT_FILE "/usr/share/clientcryptomadnessvortex/dir_user/config/key"
#elif __APPLE__
#define KEY_CLIENT_FILE "/Users/Shared/ClientCryptoMadnessVortex/dir_user/config/key"
#endif

class SecureServerCommunicator {
public:
    SecureServerCommunicator();
    size_t encrypt(const unsigned char* plaintext, size_t size, unsigned char* ciphertext, unsigned char* iv);
    bool decrypt(const unsigned char* buffer, size_t size, unsigned char* plaintext);
    void prepareMessage(const StuctToServ& msg, unsigned char* buffer);
    void loadKey();
    unsigned char *getKey() {
        return (key_);
    }

private:
    static const int KEY_SIZE_ = 16; 
    unsigned char key_[KEY_SIZE_];
};
