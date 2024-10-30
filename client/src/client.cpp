#include "client.hpp"
#include "network.hpp"
#include "message_handler.hpp"
#include "canal_handler.hpp"

#include <thread>
#include <iostream>

Client::Client(UserController *userController, MainWindow *mainWindow)
    : userController(userController), mainWindow(mainWindow) 
{
    isRunning_ = true;
    stopThread = false;
    workerThread = std::thread(&Client::processQueue, this);
}

Client::~Client() {
    {
        std::lock_guard<std::mutex> lock(queueMutexEphemeralMsg);
        stopThread = true;
    }
    cv.notify_all();
    workerThread.join();
}

void Client::addEphemeralMessage(int id, int duration) {
    std::lock_guard<std::mutex> lock(queueMutexEphemeralMsg);
    EphemeralMessage msg = { id, std::chrono::steady_clock::now(), duration };
    msgQueue.push(msg);
    cv.notify_all();
}

void Client::processQueue() { //todo regler sa sa bloque les ack
    while (true) {
        std::unique_lock<std::mutex> lock(queueMutex);
        cv.wait(lock, [this]() { 
            return !msgQueue.empty() || stopThread; 
        });

        if (stopThread) {
            break;
        }

        while (!msgQueue.empty()) {
            EphemeralMessage msg = msgQueue.top();
            auto now = std::chrono::steady_clock::now();
            auto msgEndTime = msg.timestamp + std::chrono::seconds(msg.duration);

            if (now >= msgEndTime) {
               for (const auto& entry : std::filesystem::directory_iterator(CONTACT_MP_DIR_PATH)) {
                    if (entry.is_directory()) {
                        std::string username = entry.path().filename().string();
                        MessageManager msgManager(CONTACT_MP_DIR_PATH + username + "/messages.bin");

                        msgManager.deleteMessage(msg.id);
                        emit userController->mpDeleteMsgEmit(msg.id);
                        msgQueue.pop();
                    }
                }
            } else {
                cv.wait_until(lock, msgEndTime);
            }
        }
    }
}

void Client::run() {
    while (!connectToServer(SERVER_ADDRESS, 9999, username));

    msgHandler_ = new MessageHandler;

    std::thread inputThread(&Client::handleCommand, this);
    std::thread receiveThread(&Client::receiveMessages, this);

    inputThread.join();
    receiveThread.join();
}

bool Client::connectToServer(const std::string& server_ip, int port, const std::string& username) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    Network net;
    int sockfd = net.connectToServer(server_ip, port);
    if (sockfd < 0) {
        return (false);
    } else {
        this->server_socket = sockfd;
        return (true);
    }
    return (false);
}

void Client::handleCommand() {
    CanalHandler canalHandler(msgHandler_, server_socket, [&](uint32_t id, CommandCallback callback) {
        addCommandToQueue(id, callback);
    }, userController);
    userController->setCanal(&canalHandler);

    while (true) { }
}

void Client::receiveMessages() {
    userController->setMessage(msgHandler_);

    while (true) {
       fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);

        int max_sd = server_socket;
        int activity = select(max_sd + 1, &readfds, nullptr, nullptr, nullptr);

        if (activity < 0 && errno != EINTR) {
            continue;
        }

        if (FD_ISSET(server_socket, &readfds)) {
            StructToClient message = msgHandler_->receiveMessage(server_socket);
            if (message.opcode != NONE_CODE) {
                processMessage(message);
            } else if (message.id > 0) {
                std::thread([this, message]() {
                    processCommandQueue(message);
                }).detach();
            }
        }
    }
}

void Client::processMessage(const StructToClient& message) {
    switch (message.opcode) {
        case CNNT_CODE:
            std::cout << "Connected to server !" << std::endl;
            handleConnection(); // Gérer la connexion
            break;
        case RKEY_CODE:
            handleKeyExchange(message); // Gérer l'échange de clé
            break;
        case RVUM_CODE:
            handleUserMessage(message); // Gérer le message utilisateur
            break;
        case RACK_CODE:
            handleAck(message); // Gérer l'accusé de réception
            break;
        case NJMP_CODE:
            handleUserJoin(message); // Gérer l'arrivée d'un nouvel utilisateur
            break;
        case SJMP_CODE:
            handleJoinStatus(message); // Gérer le statut de joint
            break;
        default:
            std::cerr << "Opcode inconnu : " << message.opcode << std::endl;
            break;
    }
}

void Client::handleConnection() {
    std::string path = std::string(USER_CONFIG_DIR_PATH) + "user";
    struct stat buffer;
    if (stat(path.c_str(), &buffer) == 0) {
        std::ifstream file(path);
        if (file) {
            std::string line;
            if (std::getline(file, line)) {
                std::istringstream iss(line);
                std::string username;
                std::getline(iss, username, ':');
                userController->sendLogin(username, "");
            }
            file.close();
        }
    } else {
        emit userController->emitStartToServer();
    }
}

void Client::handleKeyExchange(const StructToClient& message) {
    RSAEncryption rsaEncryption;
    rsaEncryption.initialize();

    unsigned char *decryptMessageKey = rsaEncryption.decrypt(message.data.content.rotateKey.key, 64);
    std::ofstream keyFile(KEY_CLIENT_FILE, std::ios::binary);
    keyFile.write(reinterpret_cast<const char *>(decryptMessageKey), 16);
    keyFile.close();

    msgHandler_->communicator_.loadKey();
}

void Client::handleUserMessage(const StructToClient& message) {
    std::string username(reinterpret_cast<const char*>(message.data.content.mpMsg.toClient));
    CryptedMsg cryptedMsg;
    RSAEncryption rsaEncryption;
    rsaEncryption.initialize();

    MessageManager msgManager(std::string(CONTACT_MP_DIR_PATH) + username + "/messages.bin");

    unsigned char *decryptMessageKey = rsaEncryption.decrypt(message.data.content.mpMsg.encryptedKey, 64);
    if (decryptMessageKey == nullptr) {
        //todo emit Error?
        return;
    }
    unsigned char decrypted[CryptedMsg::MAX_MESSAGE_SIZE]{0};
    if (!cryptedMsg.decrypt(message.data.content.mpMsg.msgEncrypted, decrypted, decryptMessageKey)) {
        //todo emit Error?
        return;
    }

    Message decryptedMessage;
    std::memcpy(&decryptedMessage, decrypted, sizeof(Message));

    decryptedMessage.isFromMe = !decryptedMessage.isFromMe;
    msgManager.addMessage(decryptedMessage);

    StuctToServ msgAck;
    msgAck.opcode = SACK_CODE;
    std::copy(username.begin(), username.end(), msgAck.data.content.ack.toClient);
    msgAck.data.content.ack.toClient[std::min(username.size(), sizeof(msgAck.data.content.ack.toClient) - 1)] = '\0';
    msgAck.data.content.ack.msgId = decryptedMessage.id;
    msgAck.data.content.ack.status = (int)MessageStatus::Received;
    msgHandler_->sendMessage(server_socket, msgAck);

    if (decryptedMessage.ephemeralDuration != -1) {
        addEphemeralMessage(decryptedMessage.id, decryptedMessage.ephemeralDuration);
    }

    emit userController->recvMpMsg(username, decryptedMessage.id);
}

void Client::handleAck(const StructToClient& message) {
    std::string username(reinterpret_cast<const char*>(message.data.content.ack.toClient));
    int msgId = message.data.content.ack.msgId;
    int msgStatus = message.data.content.ack.status;

    MessageManager msgManager(std::string(CONTACT_MP_DIR_PATH) + username + "/messages.bin");
    msgManager.messageChangeStatus(msgId, msgStatus);
    std::string status = msgManager.statusToString((MessageStatus)msgStatus);

    emit userController->mpChangeStatusEmit(msgId, status, 0);
}

void Client::handleUserJoin(const StructToClient& message) {
    std::string toClient(reinterpret_cast<const char*>(message.data.content.joinMp.toClient));
    std::string fromClient(reinterpret_cast<const char*>(message.data.fromClient));
    emit userController->setNewUserJoin(fromClient);
}

void Client::handleJoinStatus(const StructToClient& message) {
    std::string username(reinterpret_cast<const char*>(message.data.content.joinMpStatus.toClient));
    int status = message.data.content.joinMpStatus.status ? 1 : 0;

    if (status) {
        std::string key(reinterpret_cast<const char*>(message.data.content.joinMpStatus.publicKey));
        if (!key.length()) {
            status = 1;
        }
        std::string filename = std::string(CONTACT_DIR_PATH) + username;
        std::ofstream outFile(filename);
        if (outFile) {
            outFile << key;
            outFile.close();
        }
        std::string folderPath = std::string(CONTACT_MP_DIR_PATH) + username;
        if (!std::filesystem::exists(folderPath)) {
            std::filesystem::create_directory(folderPath);
        }
    }
    emit userController->setUserJoinStatus(status, username);
}

void Client::stop() {
    {
        std::lock_guard<std::mutex> lock(runningMutex);
        isRunning_ = false;
    }
    runningCV.notify_all();
    queueCv.notify_all();
}

void Client::addCommandToQueue(uint32_t id, CommandCallback callback) {
    std::lock_guard<std::mutex> lock(queueMutex);
    commandQueue[id] = callback;
    queueCv.notify_one();
}

void Client::processCommandQueue(const StructToClient& msg) {
    while (isRunning_) {
        std::unique_lock<std::mutex> lock(queueMutex);
        queueCv.wait(lock, [this] { return !commandQueue.empty(); });

        auto it = commandQueue.find(msg.id);
        if (it != commandQueue.end()) {

            QueueManager queueManager;
            StuctToServ tmpQueuInfo;
            queueManager.getQueue(msg.id, tmpQueuInfo);
            int opcode = tmpQueuInfo.opcode;

            auto& callbackFunc = it->second;
            int status = msg.status[0][0];
            int code = msg.status[1][0];
            if (status != CodeResponseStatus::Ongoing) {
                lock.unlock();
                if (opcode == AUTH_CODE) {
                    std::string salt = std::string(reinterpret_cast<const char*>(msg.data.content.challengeResponse.salt), 24);
                    std::string challenge = std::string(reinterpret_cast<const char*>(msg.data.content.challengeResponse.challenge), 32);
                    auto& callback = std::get<std::function<void(int, const std::string&, const std::string&)>>(callbackFunc);
                    callback(code, salt, challenge);
                } else if (opcode == JNMP_CODE) {
                    std::string username = std::string(reinterpret_cast<char*>(tmpQueuInfo.data.content.joinMpStatus.toClient));
                    auto& callback = std::get<std::function<void(int, const std::string&)>>(callbackFunc);
                    callback(code, username);
                } else {
                    auto& callback = std::get<std::function<void(int)>>(callbackFunc);
                    callback(code);
                }
                lock.lock();

                queueManager.getInfo(msg.id);
                queueManager.removeQueue(msg.id);

                commandQueue.erase(it);
                lock.unlock();
            }
            return;
        }
    }
}