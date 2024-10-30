#include "queu_manager.hpp"

void QueueManager::setQueue(const StuctToServ& stuct) {
    std::string filename = PATH_TO_QUEUE + std::to_string(stuct.id);
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(stuct.timestamp.time_since_epoch()).count();
    ofs.write(reinterpret_cast<const char*>(&stuct.id), sizeof(stuct.id));
    ofs.write(reinterpret_cast<const char*>(&stuct.opcode), sizeof(stuct.opcode));
    ofs.write(reinterpret_cast<const char*>(&timestamp), sizeof(timestamp));
    ofs.write(reinterpret_cast<const char*>(&stuct.data), sizeof(GeneriqueDataSendStruct));
    ofs.close();
}

void QueueManager::getQueue(uint32_t id, StuctToServ& stuct) {
    std::string filename = PATH_TO_QUEUE + std::to_string(id);
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) {
        std::cerr << "Error opening file for reading: " << filename << std::endl;
        return;
    }

    ifs.read(reinterpret_cast<char*>(&stuct.id), sizeof(stuct.id));
    ifs.read(reinterpret_cast<char*>(&stuct.opcode), sizeof(stuct.opcode));
    uint64_t timestamp;
    ifs.read(reinterpret_cast<char*>(&timestamp), sizeof(timestamp));
    stuct.timestamp = std::chrono::system_clock::time_point(std::chrono::milliseconds(timestamp));
    ifs.read(reinterpret_cast<char*>(&stuct.data), sizeof(GeneriqueDataSendStruct));

    ifs.close();
}

void QueueManager::removeQueue(uint32_t id) {
    std::string filename = PATH_TO_QUEUE + std::to_string(id);
    if (std::remove(filename.c_str()) != 0) {
        std::cerr << "Error removing the file: " << filename << std::endl;
    }
}

void QueueManager::getInfo(uint32_t id) {
    StuctToServ stuct;
    getQueue(id, stuct);

    std::cout << "ID: " << stuct.id << std::endl;
    std::cout << "Opcode: " << stuct.opcode << std::endl;

    auto time_t_timestamp = std::chrono::system_clock::to_time_t(stuct.timestamp);
    std::cout << "Timestamp: " << std::ctime(&time_t_timestamp);

    std::cout << "GeneriqueDataSendStruct:" << std::endl;
    std::cout << "  UID: " << stuct.data.uid << std::endl;

    switch (stuct.opcode) {
        case AUTH_CODE: {
            std::cout << "Auth Data:" << std::endl;
            std::cout << "  Username: " << stuct.data.content.auth.username << std::endl;
            break;
        }
        case CREA_CODE: {
            std::cout << "Create Account Data:" << std::endl;
            std::cout << "  Username: " << stuct.data.content.crea.username << std::endl;
            std::cout << "  Password: " << stuct.data.content.crea.password << std::endl;
            break;
        }
        case MPMS_CODE: {
            std::cout << "Private Message Data:" << std::endl;
            std::cout << "  To Client: " << stuct.data.content.mpMsg.toClient << std::endl;
            std::cout << "  Message: " << stuct.data.content.mpMsg.msgEncrypted << std::endl;
            std::cout << "  Encrypted Key: " << stuct.data.content.mpMsg.encryptedKey << std::endl;
            break;
        }
        case RMSG_CODE: {
            std::cout << "Room Message Data:" << std::endl;
            break;
        }
        case CRMS_CODE: {
            std::cout << "Create Room Data:" << std::endl;
            break;
        }
        case JRMS_CODE: {
            std::cout << "Join Room Data:" << std::endl;
            break;
        }
        case JNMP_CODE: {
            std::cout << "Join MP Data:" << std::endl;
            std::cout << "  Username: " << stuct.data.content.joinMpStatus.toClient << std::endl; // Assurez-vous que `msg` contient le nom d'utilisateur
            break;
        }
        case SDPK_CODE: {
            std::cout << "Send Public Key Data:" << std::endl;
            break;
        }
        case JMPS_CODE: {
            std::cout << "Join MP Status Data:" << std::endl;
            break;
        }
        case CHRP_CODE: {
            std::cout << "Join MP Status Data:" << std::endl;
            std::cout << "  clientName: " << stuct.data.content.challengeResponse.clientName << std::endl; // Assurez-vous que `msg` contient le nom d'utilisateur
            std::cout << "  challengeHash: " << stuct.data.content.challengeResponse.challengeHash << std::endl; // Assurez-vous que `msg` contient le nom d'utilisateur
            break;
        }
        default: {
            std::cout << "Unknown Opcode" << std::endl;
            break;
        }
    }
}
