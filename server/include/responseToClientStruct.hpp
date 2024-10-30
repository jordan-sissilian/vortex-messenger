#pragma once
#include <cstring> 

struct CnntStruct {
    char code[6];

    CnntStruct(const char* inputCode = 000000) {
        strncpy(code, inputCode, sizeof(code) - 1);
        code[sizeof(code) - 1] = '\0';
    }
};

typedef struct StructAuth {
    unsigned char username[48];
    unsigned char password[48];
} auth_t;

typedef struct StructCrea {
    unsigned char username[48];
    unsigned char password[48];
} crea_t;

typedef struct StructMpMsg {
    unsigned char toClient[48];
    unsigned char msgEncrypted[276];
    unsigned char encryptedKey[64];
} mpMsg_t;

typedef struct StructJoinMp {
    unsigned char toClient[48];
} joinMp_t;

typedef struct StructJoinMpStatus {
    unsigned char toClient[48];
    unsigned char publicKey[187];
    bool status;
} joinMpStatus_t;

typedef struct StructSendPublicKey {
    unsigned char clientName[48];
    unsigned char publicKey[187];
} sendPublicKey_t;

typedef struct StructSendChallengeResponse {
    unsigned char clientName[48];
    unsigned char challengeHash[64];
} sendChallengeResponse_t;

typedef struct StructAck {
    unsigned char toClient[48];
    int msgId;
    int status;
    unsigned int timestamp;
} ack_t;

struct GeneriqueDataReciveStruct {
    int uid;
    unsigned char fromClient[48];
    int sizeContent;
    union content {
        auth_t auth;
        crea_t crea;
        mpMsg_t mpMsg;
        joinMp_t joinMp;
        joinMpStatus_t joinMpStatus;
        sendPublicKey_t sendPublicKey;
        sendChallengeResponse_t challengeResponse;
        ack_t ack;
    } content;

    GeneriqueDataReciveStruct() {
        std::memset(this, 0, sizeof(GeneriqueDataReciveStruct));
    }

    void reset() {
        std::memset(this, 0, sizeof(GeneriqueDataReciveStruct));
    }
};

typedef struct StructRotateKey {
    unsigned char key[64];
} rotateKey_t;

typedef struct StructReciveChallengeResponse {
    unsigned char salt[24];
    unsigned char challenge[32];
} reciveChallengeResponse_t;

struct GeneriqueDataSendStruct {
    int uid;
    unsigned char fromClient[48];

    union content {
        mpMsg_t mpMsg;
        joinMp_t joinMp;
        joinMpStatus_t joinMpStatus;
        rotateKey_t rotateKey;
        reciveChallengeResponse_t challengeResponse;
        ack_t ack;
    } content;

    GeneriqueDataSendStruct() {
        std::memset(this, 0, sizeof(GeneriqueDataSendStruct));
    }
};
