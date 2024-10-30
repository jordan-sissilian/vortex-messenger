#pragma once

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdexcept>
#include <cstring>
#include <iostream>

class CryptedMsg {
public:
    static const size_t KEY_SIZE = 16; // AES 128 bits => 16 bytes
    static const size_t IV_SIZE = 16; // Size of the IV for AES
    static const size_t MAX_MESSAGE_SIZE = 256; // Max size of plaintext message

    CryptedMsg();
    ~CryptedMsg();
    
    void generateKey(unsigned char* key);
    size_t encrypt(const unsigned char* plaintext, size_t size, unsigned char* buffer);
    bool decrypt(const unsigned char* buffer, unsigned char* plaintext, const unsigned char *key);
    unsigned char *getKey();

private:
    unsigned char key_[KEY_SIZE]; // AES key
};
