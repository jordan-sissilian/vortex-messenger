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
#define PRIVATE_KEY_FILE "/usr/share/clientcryptomadnessvortex/dir_user/key/private.pem" 
#define PUBLIC_KEY_FILE "/usr/share/clientcryptomadnessvortex/dir_user/key/public.pem"
#define PUBLIC_KEY_FILE_USER(username) (std::string("/usr/share/clientcryptomadnessvortex/dir_user/contact/") + (username))
#elif __APPLE__
#define PRIVATE_KEY_FILE "/Users/Shared/ClientCryptoMadnessVortex/dir_user/key/private.pem" 
#define PUBLIC_KEY_FILE "/Users/Shared/ClientCryptoMadnessVortex/dir_user/key/public.pem"
#define PUBLIC_KEY_FILE_USER(username) (std::string("/Users/Shared/ClientCryptoMadnessVortex/dir_user/contact/") + (username))
#endif


class RSAEncryption {
public:
    RSAEncryption();
    ~RSAEncryption();

    void initialize();
    size_t encrypt(unsigned char *dest, const unsigned char *message, const std::string& publicKeyFile);
    unsigned char* decrypt(const unsigned char* encryptedData, size_t encryptedSize);
    int getUserIdFromPrivateKey();

private:
    void generateRSAKeyPair();
    void saveKeys(const std::string& privateKeyFile, const std::string& publicKeyFile);
    void loadKeys(const std::string& privateKeyFile, const std::string& publicKeyFile);
    bool fileExists(const std::string& filename);
    void printLastError();
    int hashPrivateKeyToId();
    EVP_PKEY* loadPublicKey(const std::string& publicKeyFile);

    EVP_PKEY* pkey;
    std::string privateKeyFile;
    std::string publicKeyFile;
};
