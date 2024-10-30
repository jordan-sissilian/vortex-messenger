#include "rsa_encryption.hpp"

RSAEncryption::RSAEncryption() 
    : pkey(nullptr), 
      privateKeyFile(PRIVATE_KEY_FILE), 
      publicKeyFile(PUBLIC_KEY_FILE) {}

RSAEncryption::~RSAEncryption() {
    if (pkey) {
        EVP_PKEY_free(pkey);
    }
}

void RSAEncryption::initialize() {
    if (!fileExists(privateKeyFile) || !fileExists(publicKeyFile)) {
        generateRSAKeyPair();
    } else {
        loadKeys(privateKeyFile, publicKeyFile);
    }
}

void RSAEncryption::generateRSAKeyPair() {
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if (!ctx) {
        printLastError();
        return;
    }

    if (EVP_PKEY_keygen_init(ctx) <= 0 || EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 512) <= 0) {
        printLastError();
        EVP_PKEY_CTX_free(ctx);
        return;
    }

    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        printLastError();
        EVP_PKEY_CTX_free(ctx);
        return;
    }

    EVP_PKEY_CTX_free(ctx);
    saveKeys(privateKeyFile, publicKeyFile);
}

void RSAEncryption::saveKeys(const std::string& privateKeyFile, const std::string& publicKeyFile) {
    BIO* privateBio = BIO_new_file(privateKeyFile.c_str(), "w");
    if (privateBio) {
        if (PEM_write_bio_PrivateKey(privateBio, pkey, nullptr, nullptr, 0, nullptr, nullptr) != 1) {
            printLastError();
        }
        BIO_free(privateBio);
    } else {
        std::cerr << "Erreur lors de l'ouverture du fichier pour la clé privée." << std::endl;
    }

    BIO* publicBio = BIO_new_file(publicKeyFile.c_str(), "w");
    if (publicBio) {
        if (PEM_write_bio_PUBKEY(publicBio, pkey) != 1) {
            printLastError();
        }
        BIO_free(publicBio);
    } else {
        std::cerr << "Erreur lors de l'ouverture du fichier pour la clé publique." << std::endl;
    }
}

void RSAEncryption::loadKeys(const std::string& privateKeyFile, const std::string& publicKeyFile) {
    BIO* privateBio = BIO_new_file(privateKeyFile.c_str(), "r");
    if (privateBio) {
        pkey = PEM_read_bio_PrivateKey(privateBio, nullptr, nullptr, nullptr);
        if (!pkey) {
            printLastError();
        }
        BIO_free(privateBio);
    } else {
        std::cerr << "Erreur lors de l'ouverture du fichier pour la clé privée." << std::endl;
    }
}

bool RSAEncryption::fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
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
        std::cerr << "Erreur lors de l'ouverture du fichier pour la clé publique." << std::endl;
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

unsigned char* RSAEncryption::decrypt(const unsigned char* encryptedData, size_t encryptedSize) {
    if (encryptedData == nullptr) {
        throw std::invalid_argument("Encrypted data must not be null");
    }

    unsigned char* out = new unsigned char[16];

    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (!ctx) {
        printLastError();
        delete[] out;
        return nullptr;
    }

    if (EVP_PKEY_decrypt_init(ctx) <= 0) {
        printLastError();
        EVP_PKEY_CTX_free(ctx);
        delete[] out;
        return nullptr;
    }

    size_t decryptedSize;
    if (EVP_PKEY_decrypt(ctx, nullptr, &decryptedSize, encryptedData, encryptedSize) <= 0) {
        printLastError();
        EVP_PKEY_CTX_free(ctx);
        delete[] out;
        return nullptr;
    }

    if (EVP_PKEY_decrypt(ctx, out, &decryptedSize, encryptedData, encryptedSize) <= 0) {
        printLastError();
        EVP_PKEY_CTX_free(ctx);
        delete[] out;
        return nullptr;
    }

    EVP_PKEY_CTX_free(ctx);
    return out;
}

int RSAEncryption::getUserIdFromPrivateKey() {
    return hashPrivateKeyToId();
}

int RSAEncryption::hashPrivateKeyToId() {
   BIO* bio = BIO_new(BIO_s_mem());
    if (PEM_write_bio_PrivateKey(bio, pkey, nullptr, nullptr, 0, nullptr, nullptr) == 0) {
        EVP_PKEY_free(pkey);
        BIO_free(bio);
        printLastError();
        return -1;
    }

    BUF_MEM* buffer;
    BIO_get_mem_ptr(bio, &buffer);
    BIO_flush(bio);
    std::string privateKeyPEM(buffer->data, buffer->length);
    BIO_free(bio);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(privateKeyPEM.c_str()), privateKeyPEM.size(), hash);

    int id = 0;
    for (int i = 0; i < 4; ++i) { 
        id = (id << 8) | hash[i];
    }
    return id;
}