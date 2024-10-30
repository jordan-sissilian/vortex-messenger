#include "canal_handler.hpp"
#include <iostream>
#include <fstream>
#include <random>

CanalHandler::CanalHandler(MessageHandler *msgHandler_, int socket_fd, std::function<void(uint32_t, CommandCallback)> addCommandFunc, UserController *userController)
    : msgHandler_(msgHandler_), socket_fd(socket_fd), addCommandFunc_(addCommandFunc), userController_(userController) {}

static uint32_t randomId() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis;

    return dis(gen);
}

void CanalHandler::callbackAuth(int code, const std::string& salt, const std::string& challenge) {
    if (code == 250) {
        handleChallengeReponse(salt, challenge);
    } else {
        std::cout << "Error: recive challenge" << std::endl;
        std::string filePath(std::string(USER_CONFIG_DIR_PATH) + "user");
        std::remove(filePath.c_str());
        std::cout << "delete file with credential" << std::endl;
    }
}

void CanalHandler::handleAuth(const std::string& username, const std::string& password) {
    msg_.reset();
    msg_.id = randomId();
    msg_.opcode = AUTH_CODE;

    if (password != "" && (username.length() > 48 || password.length() > 48)) {
        return;
    }

    if (password != "") {
        std::ofstream userFile;
        try {
            userFile.open(std::string(USER_CONFIG_DIR_PATH) + "user", std::ios::app);
            if (!userFile) {
                return;
            }
            userFile << username << ":" << password << std::endl; 
            userFile.close();
        } catch (const std::exception& e) {
            std::cerr << "Error writing to file: " << e.what() << std::endl;
        }
    }

    username_ = username;
    std::copy(username.begin(), username.end(), msg_.data.content.auth.username);
    msg_.data.content.auth.username[std::min(username.size(), sizeof(msg_.data.content.auth.username) - 1)] = '\0';
    
    CommandCallback callback = [&, this](int code, const std::string& salt, const std::string& challenge) {
        this->callbackAuth(code, salt, challenge);
    };
    addCommandFunc_(msg_.id, std::move(callback));

    queueManager_.setQueue(msg_);
    msgHandler_->sendMessage(socket_fd, msg_);
}

void CanalHandler::callbackChallengeReponse(int code) {
    rsaEncryption_.initialize();

    if (code == 250) {
        StuctToServ msg;
        msg.id = randomId();
        msg.opcode = SDPK_CODE;
        msg.data.uid = rsaEncryption_.getUserIdFromPrivateKey();

        std::copy(username_.begin(), username_.end(), msg.data.content.sendPublicKey.clientName);
        msg.data.content.sendPublicKey.clientName[std::min(username_.size(), sizeof(msg.data.content.sendPublicKey.clientName) - 1)] = '\0';

        std::ifstream file(PUBLIC_KEY_FILE, std::ios::binary);
        file.read(reinterpret_cast<char*>(msg.data.content.sendPublicKey.publicKey), sizeof(msg.data.content.sendPublicKey.publicKey) - 1);
    
        MessageHandler msgHandler;
        msgHandler.sendMessage(socket_fd, msg);
    } else {
        std::string filePath(std::string(USER_CONFIG_DIR_PATH) + "user");
        std::remove(filePath.c_str());
    }

    emit userController_->sendLoginCode(code);
}

static std::string hashWithSHA256(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

static std::string hashPassword(const std::string& password, const std::string& salt) {
    CryptoPP::SHA3_256 sha3;
    std::string sha3Digest;

    CryptoPP::StringSource ss1(password, true, new CryptoPP::HashFilter(sha3, new CryptoPP::StringSink(sha3Digest)));

    //same with server
    const size_t N = 16384;  // Cost factor (iterations)
    const size_t r = 8;      // Block size
    const size_t p = 1;      // Parallelism factor (threads)
    const size_t dkLen = 32; // Desired derived key length (32 bytes)

    CryptoPP::SecByteBlock derivedKey(dkLen);
    CryptoPP::Scrypt scrypt;
    scrypt.DeriveKey(derivedKey, derivedKey.size(),
                      (const CryptoPP::byte*)sha3Digest.data(), sha3Digest.size(),
                      (const CryptoPP::byte*)salt.data(), salt.size(),
                      N, r, p);

    std::string encodedDigest;
    CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(encodedDigest));
    encoder.Put(derivedKey, derivedKey.size());
    encoder.MessageEnd();

    return encodedDigest;
}

void CanalHandler::handleChallengeReponse(const std::string& salt, const std::string& challenge) {
    msg_.reset();
    msg_.id = randomId();
    msg_.opcode = CHRP_CODE;

    std::string username, password;
    std::ifstream userFileRead;
    try {
        userFileRead.open(std::string(USER_CONFIG_DIR_PATH) + "user");
        if (!userFileRead) {
            return;
        }

        std::string line;
        if (std::getline(userFileRead, line)) {
            std::stringstream ss(line);
            std::getline(ss, username, ':');
            std::getline(ss, password);

            if (username.empty() || password.empty()) {
                return;
            }
        } else {
            return;
        }
        userFileRead.close();
    } catch (const std::exception& e) {
        std::cerr << "Error reading file: " << e.what() << std::endl;
    }

    std::string passwordHashed = password.size() == 64 ? password : hashPassword(password, salt);
    if (password.size() != 64) {
        std::ofstream userFileWrite;
        try {
            userFileWrite.open(std::string(USER_CONFIG_DIR_PATH) + "user", std::ios::trunc);
            if (!userFileWrite) {
                return;
            }
            userFileWrite << username << ":" << passwordHashed << std::endl; 
            userFileWrite.close();
        } catch (const std::exception& e) {
            std::cerr << "Error writing to file: " << e.what() << std::endl;
        }
    }

    username_ = username;
    std::copy(username.begin(), username.end(), msg_.data.content.auth.username);
    msg_.data.content.auth.username[std::min(username.size(), sizeof(msg_.data.content.auth.username) - 1)] = '\0';
    
    std::string hashedPassword = hashWithSHA256(passwordHashed + salt);
    std::string combined = hashedPassword + challenge;
    std::string challengeHash = hashWithSHA256(combined);

    std::copy(challengeHash.begin(), challengeHash.end(), msg_.data.content.challengeResponse.challengeHash);

    CommandCallback callback = [&, this](int code) {
        this->callbackChallengeReponse(code);
    };
    addCommandFunc_(msg_.id, std::move(callback));

    queueManager_.setQueue(msg_);
    msgHandler_->sendMessage(socket_fd, msg_);
}

void CanalHandler::callbackCrea(int code) {
    emit userController_->sendRegisterCode(code);
}

void CanalHandler::handleCrea(const std::string& username, const std::string& password) {
    msg_.reset();
    msg_.id = randomId();
    msg_.opcode = CREA_CODE;

    if (username.length() > 48 || password.length() > 48) {
        return;
    }

    std::copy(username.begin(), username.end(), msg_.data.content.crea.username);
    msg_.data.content.crea.username[std::min(username.size(), sizeof(msg_.data.content.crea.username) - 1)] = '\0';

    std::copy(password.begin(), password.end(), msg_.data.content.crea.password);
    msg_.data.content.crea.password[std::min(password.size(), sizeof(msg_.data.content.crea.password) - 1)] = '\0';

    CommandCallback callback = [&, this](int code) {
        this->callbackCrea(code);
    };
    addCommandFunc_(msg_.id, std::move(callback));

    queueManager_.setQueue(msg_);
    msgHandler_->sendMessage(socket_fd, msg_);
}

void CanalHandler::handleQuit() {
    msg_.reset();
    msg_.id = randomId();
    msg_.opcode = DECO_CODE;

    msgHandler_->sendMessage(socket_fd, msg_);
}

void CanalHandler::callbackMpms(int code) {
    // emit userController_->sendMpMsgCode(code);
}

void CanalHandler::handleMpMsg(const std::string &username, const std::string &message) {
    msg_.reset();
    msg_.id = randomId();
    msg_.opcode = MPMS_CODE;

    if (username.length() > 48 || message.length() > 256) {
        return;
    }

    std::string fromClient(username_);
    std::string toClient(username);

    std::copy(fromClient.begin(), fromClient.end(), msg_.data.fromClient);
    msg_.data.fromClient[std::min(fromClient.size(), sizeof(msg_.data.fromClient) - 1)] = '\0';

    std::copy(toClient.begin(), toClient.end(), msg_.data.content.mpMsg.toClient);
    msg_.data.content.mpMsg.toClient[std::min(toClient.size(), sizeof(msg_.data.content.mpMsg.toClient) - 1)] = '\0';

    unsigned char chiffredMsg[CryptedMsg::MAX_MESSAGE_SIZE + CryptedMsg::IV_SIZE + sizeof(int)]{0};

    CryptedMsg cryptedMsg;
    RSAEncryption rsaEncryption;
    rsaEncryption.initialize();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 1000000);
    int randomId = dis(gen);

    MessageManager msgManager(std::string(CONTACT_MP_DIR_PATH) + toClient + "/messages.bin");
    Message msgStruct = {
        randomId,
        true,
        "",
        std::time(nullptr),
        MessageStatus::Sent,
        -1 //todo time ephemer -1 false, time true
    };
    std::strncpy(msgStruct.content, message.c_str(), sizeof(msgStruct.content) - 1);
    msgStruct.content[sizeof(msgStruct.content) - 1] = '\0';

    size_t encryptedSize = cryptedMsg.encrypt(reinterpret_cast<const unsigned char*>(&msgStruct), sizeof(msgStruct), chiffredMsg);
    std::memset(msg_.data.content.mpMsg.msgEncrypted, 0, sizeof(msg_.data.content.mpMsg.msgEncrypted));
    if (encryptedSize <= sizeof(msg_.data.content.mpMsg.msgEncrypted)) {
        std::memcpy(msg_.data.content.mpMsg.msgEncrypted, chiffredMsg, encryptedSize);
    } else {
        std::cerr << "Error: encryptedSize > size of msg_.data.content.mpMsg.msgEncrypted" << std::endl;
    }

    unsigned char *key = cryptedMsg.getKey();
    unsigned char cryptMessage[64]{0};
    size_t cryptMessageLen = rsaEncryption.encrypt(cryptMessage, key, PUBLIC_KEY_FILE_USER(toClient));
    std::memset(msg_.data.content.mpMsg.encryptedKey, 0, sizeof(msg_.data.content.mpMsg.encryptedKey));
    if (cryptMessageLen <= sizeof(msg_.data.content.mpMsg.encryptedKey)) {
        std::memcpy(msg_.data.content.mpMsg.encryptedKey, cryptMessage, cryptMessageLen);
    } else {
        std::cerr << "Error: cryptMessageLen > size of msg_.data.content.mpMsg.encryptedKey" << std::endl;
    }

    msgManager.addMessage(msgStruct);
    emit userController_->mpMsgEmit(msgStruct.id);

    queueManager_.setQueue(msg_);
    msgHandler_->sendMessage(socket_fd, msg_);
}

void CanalHandler::callbackRmsg(int code) {
    // TODO: action auth
    emit userController_->sendRoomMsgCode(code);
}

void CanalHandler::handleRoomMsg(const std::string &message) {
    msg_.reset();
    msg_.id = randomId();
    msg_.opcode = RMSG_CODE;

    if (message.length() > 256) {
        return;
    }

    std::copy(message.begin(), message.end(), msg_.data.content.mpMsg.msgEncrypted); // Utilisation d'un message approprié
    msg_.data.content.mpMsg.msgEncrypted[std::min(message.size(), sizeof(msg_.data.content.mpMsg.msgEncrypted) - 1)] = '\0';

    queueManager_.setQueue(msg_);
    msgHandler_->sendMessage(socket_fd, msg_);
}

void CanalHandler::callbackCrms(int code) {
    emit userController_->createRoomCode(code);
}

void CanalHandler::handlecreateRoom(const std::string &nameRoom) {
    msg_.reset();
    msg_.id = randomId();
    msg_.opcode = CRMS_CODE;

    if (nameRoom.length() > 48) {
        return;
    }

    std::copy(nameRoom.begin(), nameRoom.end(), msg_.data.content.mpMsg.msgEncrypted); // Utilisation d'un message approprié
    msg_.data.content.mpMsg.msgEncrypted[std::min(nameRoom.size(), sizeof(msg_.data.content.mpMsg.msgEncrypted) - 1)] = '\0';

    queueManager_.setQueue(msg_);
    msgHandler_->sendMessage(socket_fd, msg_);
}

void CanalHandler::callbackJrms(int code) {
    emit userController_->joinRoomrCode(code);
}

void CanalHandler::handlejoinRoom(const std::string &nameRoom) {
    msg_.reset();
    msg_.id = randomId();
    msg_.opcode = JRMS_CODE;

    if (nameRoom.length() > 48) {
        return;
    }

    std::copy(nameRoom.begin(), nameRoom.end(), msg_.data.content.mpMsg.msgEncrypted); // Utilisation d'un message approprié
    msg_.data.content.mpMsg.msgEncrypted[std::min(nameRoom.size(), sizeof(msg_.data.content.mpMsg.msgEncrypted) - 1)] = '\0';

    queueManager_.setQueue(msg_);
    msgHandler_->sendMessage(socket_fd, msg_);
}

void CanalHandler::callbackJnmp(int code, const std::string &username) {
    emit userController_->joinMpCode(code, username); //todo
}

void CanalHandler::handlejoinMp(const std::string &username) {
    msg_.reset();
    msg_.id = randomId();
    msg_.opcode = JNMP_CODE;
    if (username.length() > 48) {
        return;
    }

    std::string toClient(username);
    std::copy(toClient.begin(), toClient.end(), msg_.data.content.joinMp.toClient);
    msg_.data.content.joinMp.toClient[toClient.length()] = '\0';

    std::string fromClient(username_);
    std::copy(fromClient.begin(), fromClient.end(), msg_.data.fromClient);
    msg_.data.fromClient[std::min(fromClient.size(), sizeof(msg_.data.fromClient) - 1)] = '\0';

    CommandCallback callback = [&, this](int code, const std::string& username) {
        this->callbackJnmp(code, username);
    };
    addCommandFunc_(msg_.id, std::move(callback));

    queueManager_.setQueue(msg_);
    msgHandler_->sendMessage(socket_fd, msg_);
}

void CanalHandler::sendJointMpStatus(int status, const std::string &nameUser) {
    msg_.reset();
    msg_.id = randomId();
    msg_.opcode = JMPS_CODE;

    std::string fromClient(username_);
    std::copy(fromClient.begin(), fromClient.end(), msg_.data.fromClient);
    msg_.data.fromClient[std::min(fromClient.size(), sizeof(msg_.data.fromClient) - 1)] = '\0';

    std::string toClient(nameUser);
    std::copy(toClient.begin(), toClient.end(), msg_.data.content.joinMp.toClient);
    msg_.data.content.joinMp.toClient[std::min(toClient.size(), sizeof(msg_.data.content.joinMp.toClient) - 1)] = '\0';

    msg_.data.content.joinMpStatus.status = status ? true : false;

    msgHandler_->sendMessage(socket_fd, msg_);
}

void CanalHandler::handleViewMsg(const std::string& nameUser, std::time_t timestampActual) { // todo corriger ca bug
    msg_.reset();
    msg_.id = randomId();
    msg_.opcode = SACK_CODE;

    std::copy(nameUser.begin(), nameUser.end(), msg_.data.content.ack.toClient);
    msg_.data.content.ack.toClient[std::min(nameUser.size(), sizeof(msg_.data.content.ack.toClient) - 1)] = '\0';
    msg_.data.content.ack.msgId = 0;
    msg_.data.content.ack.status = (int)MessageStatus::Read;
    msg_.data.content.ack.timestamp = (unsigned int)timestampActual;

    msgHandler_->sendMessage(socket_fd, msg_);
}