#include "crypted_msg.hpp"

CryptedMsg::CryptedMsg() {
    generateKey(key_);
}

CryptedMsg::~CryptedMsg() {}

unsigned char *CryptedMsg::getKey() {
    return (key_);
}

void CryptedMsg::generateKey(unsigned char* key) {
    if (!RAND_bytes(key, KEY_SIZE)) {
        return;
    }
}

size_t CryptedMsg::encrypt(const unsigned char* plaintext, size_t size, unsigned char* buffer) {
    if (size > MAX_MESSAGE_SIZE) {
        return (-1);
    }

    unsigned char iv[IV_SIZE];
    RAND_bytes(iv, sizeof(iv));

    int padding = 16 - (size % 16);
    int padded_size = size + padding;

    unsigned char padded_plaintext[padded_size];
    memcpy(padded_plaintext, plaintext, size);
    for (size_t i = size; i < padded_size; ++i) {
        padded_plaintext[i] = static_cast<unsigned char>(padding);
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return (-1);
    }

    if (EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, key_, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return (-1);
    }

    int len;
    unsigned char ciphertext[padded_size];

    if (EVP_EncryptUpdate(ctx, ciphertext, &len, padded_plaintext, padded_size) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return (-1);
    }

    if (EVP_EncryptFinal_ex(ctx, ciphertext + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return (-1);
    }

    EVP_CIPHER_CTX_free(ctx);

    memcpy(buffer, &padded_size, sizeof(int));
    memcpy(buffer + sizeof(int), iv, IV_SIZE);
    memcpy(buffer + sizeof(int) + IV_SIZE, ciphertext, padded_size);

    return sizeof(int) + IV_SIZE + padded_size;
}

bool CryptedMsg::decrypt(const unsigned char* buffer, unsigned char* plaintext, const unsigned char* key) {
    int ciphertext_len;
    memcpy(&ciphertext_len, buffer, sizeof(ciphertext_len));

    unsigned char iv[IV_SIZE];
    memcpy(iv, buffer + sizeof(ciphertext_len), sizeof(iv));
    const unsigned char* ciphertext = buffer + sizeof(ciphertext_len) + sizeof(iv);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return (false);
    }

    int len;
    if (EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return (false);
    }

    int plaintext_len = len;
    if (EVP_DecryptFinal_ex(ctx, plaintext + plaintext_len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return (false);
    }
    EVP_CIPHER_CTX_free(ctx);
    return (true);
}