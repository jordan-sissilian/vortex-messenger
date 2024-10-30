#pragma once

#include <chrono>
#include <cstdint>
#include <cstring>

#include "responseToClientStruct.hpp"

enum CodeResponseStatus {
    Ok,
    Ongoing,
    End,
    Error
};

struct StuctToServ {
    uint32_t id;
    uint32_t opcode;
    std::chrono::system_clock::time_point timestamp;
    GeneriqueDataReciveStruct data;
};

struct StructToClient {
    uint32_t id;
    uint32_t opcode;
    std::chrono::system_clock::time_point timestamp;
    GeneriqueDataSendStruct data;
    int status[2][1];

    void setStatus(CodeResponseStatus newStatus, int code) {
        status[0][0] = newStatus;
        status[1][0] = code;
    }

    StructToClient() 
        : id(0),
          opcode(0X00)
    {
        memset(status, 0, sizeof(status));
        timestamp = std::chrono::system_clock::now();
    }
};
