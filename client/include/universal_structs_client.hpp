#pragma once

#include <chrono>
#include <cstdint>
#include <cstring>

#include "responseToServerStruct.hpp"

enum CodeResponseStatus {
    Ok,
    Ongoing,
    Error
};

struct StuctToServ {
    uint32_t id;
    uint32_t opcode;
    std::chrono::system_clock::time_point timestamp;
    GeneriqueDataSendStruct data;

    StuctToServ()
        : id(0),
          opcode(0x00)
    {
        timestamp = std::chrono::system_clock::now();
    }

    void reset() {
        id = 0;
        opcode = 0x00;
        timestamp = std::chrono::system_clock::now();
        data.reset();
    }
};

struct StructToClient {
    uint32_t id;
    uint32_t opcode;
    std::chrono::system_clock::time_point timestamp;
    GeneriqueDataReciveStruct data;
    int status[2][1];
};