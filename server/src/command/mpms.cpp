#include "command.hpp"
#include <ctime>
#include <fstream>
#include <random>

void Command::handleMpmsCommand(ClientData* client, const StuctToServ& message) {

    const std::string username(reinterpret_cast<const char*>(message.data.content.mpMsg.toClient));

    StructToClient response_client_sender;
    response_client_sender.id = message.id;
    response_client_sender.opcode = NONE_CODE;

    StructToClient response_client_receiver;
    response_client_receiver.opcode = RVUM_CODE;
    bool userExist = isUserExists(username);
    if (!userExist) {
        response_client_sender.setStatus(CodeResponseStatus::Error, 500);
        sendToClient(client, response_client_sender);
        return;
    }
    auto optClient = isConnected(username);
    if (!optClient) {
        std::time_t currentTime = std::time(nullptr);
        std::string filePath = std::string(QUEU_PATH) + username + "_" + std::to_string(currentTime);

        std::ofstream outputFile(filePath, std::ios::binary);
        if (outputFile.is_open()) {
            outputFile.write(reinterpret_cast<const char*>(&message), sizeof(StuctToServ));
            outputFile.close();
        }

        response_client_sender.setStatus(CodeResponseStatus::Ongoing, 490);
        sendToClient(client, response_client_sender);
        return;
    }
    response_client_sender.setStatus(CodeResponseStatus::Ok, 250);
    sendToClient(client, response_client_sender);

    const std::string fromClient(reinterpret_cast<const char*>(message.data.fromClient));
    ClientData user_receiver = *optClient;
    size_t username_len = fromClient.length();
    std::memcpy(response_client_receiver.data.content.mpMsg.toClient, fromClient.c_str(), username_len);
    response_client_receiver.data.content.mpMsg.toClient[username_len] = '\0';

    std::memcpy(response_client_receiver.data.content.mpMsg.msgEncrypted, message.data.content.mpMsg.msgEncrypted, 276);
    std::memcpy(response_client_receiver.data.content.mpMsg.encryptedKey, message.data.content.mpMsg.encryptedKey, 64);

    sendToClient(&user_receiver, response_client_receiver);

    if (client->msg_send >= 5) {
        StructToClient newKeyResponse;
        newKeyResponse.opcode = RKEY_CODE;
        client->communicator_.username_ = client->username;
        unsigned char *newKey = client->communicator_.createNewKey();

        RSAEncryption clientEncryptKey;
        clientEncryptKey.encrypt(newKeyResponse.data.content.rotateKey.key, newKey, PUBLIC_KEY_FILE_USER(client->username));

        sendToClient(client, newKeyResponse);
        client->communicator_.rotateKey(newKey);
        client->msg_send = 0;
    } else {
        client->msg_send += 1;
    }
}
