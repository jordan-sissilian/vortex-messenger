#include "rsa_encryption.hpp"

RSAEncryption::RSAEncryption() 
    : pkey(nullptr), 
      publicKeyFile(PUBLIC_KEY_FILE) {}

RSAEncryption::~RSAEncryption() {
    if (pkey) {
        EVP_PKEY_free(pkey);
    }
}

void RSAEncryption::printLastError() {
    unsigned long errCode = ERR_get_error();
    char errMsg[120];
    ERR_error_string_n(errCode, errMsg, sizeof(errMsg));
    std::cerr << "Erreur : " << errMsg << std::endl;
}

EVP_PKEY* RSAEncryption::loadPublicKey(const std::string& publicKeyFile) {
    BIO* publicBio = BIO_new_file(publicKeyFile.c_str(), "r");
    if (publicBio) {
        EVP_PKEY* pubKey = PEM_read_bio_PUBKEY(publicBio, nullptr, nullptr, nullptr);
        BIO_free(publicBio);
        if (!pubKey) {
            printLastError();
        }
        return pubKey;
    } else {
        std::cerr << "Erreur load public key." << std::endl;
        return nullptr;
    }
}

size_t RSAEncryption::encrypt(unsigned char *dest, const unsigned char *message, const std::string& publicKeyFile) {
    if (message == nullptr) {
        throw std::invalid_argument("Message must not be null");
    }

    EVP_PKEY* publicKey = loadPublicKey(publicKeyFile);
    if (!publicKey) {
        throw std::runtime_error("Failed to load public key");
    }

    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(publicKey, nullptr);
    if (!ctx) {
        printLastError();
        EVP_PKEY_free(publicKey);
        throw std::runtime_error("Failed to create context");
    }

    if (EVP_PKEY_encrypt_init(ctx) <= 0) {
        printLastError();
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(publicKey);
        throw std::runtime_error("Failed to initialize encryption");
    }

    size_t outlen;
    if (EVP_PKEY_encrypt(ctx, nullptr, &outlen, message, 16) <= 0) {
        printLastError();
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(publicKey);
        throw std::runtime_error("Failed to determine output length");
    }

    if (dest == nullptr) {
        dest = new unsigned char[outlen];
    } else {
        if (outlen > 64) {
            EVP_PKEY_CTX_free(ctx);
            EVP_PKEY_free(publicKey);
            throw std::runtime_error("Destination buffer is too small");
        }
    }

    if (EVP_PKEY_encrypt(ctx, dest, &outlen, message, 16) <= 0) {
        printLastError();
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(publicKey);
        if (dest != nullptr) {
            delete[] dest;
        }
        throw std::runtime_error("Encryption failed");
    }

    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(publicKey);
    return outlen;
}