#pragma once

#include <string>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <iostream>
#include <fstream>
#include <vector>


#ifdef __linux__
#define PUBLIC_KEY_FILE "/usr/share/servercryptomadnessvortex/server/users_key/"
#elif __APPLE__
#define PUBLIC_KEY_FILE "/Users/Shared/ServerCryptoMadnessVortex/server/users_key/"
#endif
#define PUBLIC_KEY_FILE_USER(username) (std::string(PUBLIC_KEY_FILE) + (username) + ".key")

class RSAEncryption {
public:
    RSAEncryption();
    ~RSAEncryption();

    size_t encrypt(unsigned char *dest, const unsigned char *message, const std::string& publicKeyFile);

private:
    void printLastError();
    EVP_PKEY* loadPublicKey(const std::string& publicKeyFile);

    EVP_PKEY* pkey;
    std::string publicKeyFile;
};
