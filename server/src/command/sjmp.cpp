#include "command.hpp"
#include <ctime>
#include <fstream>

/**
 * @brief Handles the JMPS command (Join Message Status).
 * 
 * This function processes the JMPS command sent by a client, extracts the username
 * from the received message, and checks if the user is connected. If the user is not
 * connected, the message is saved to a file with a timestamp. If the user is connected,
 * it retrieves the user's public key and sends a response to the client.
 * 
 * @param client  A pointer to the ClientData structure representing the client's data 
 *                sending the command.
 * @param message A reference to a StuctToServ structure containing the command data,
 *                including the username.
 * 
 * @return void
 * 
 * @details
 * 
 * - If the user is not connected, the message is saved to a file named <username>_<timestamp> in the defined storage path.
 * - If the user is connected, the public key is retrieved from a file and sent back to the client.
 * - The response includes the status of the operation and the username of the connected user.
 */
void Command::handleJmpsCommand(ClientData* client, const StuctToServ& message) {
    const std::string username(reinterpret_cast<const char*>(message.data.content.joinMpStatus.toClient));

    StructToClient response_client_receiver;
    response_client_receiver.opcode = SJMP_CODE;
    response_client_receiver.data.content.joinMpStatus.status = message.data.content.joinMpStatus.status;

    std::string toClient(reinterpret_cast<const char*>(message.data.content.joinMp.toClient));
    std::string fromClient(reinterpret_cast<const char*>(message.data.fromClient));

    auto optClient = isConnected(toClient);
    if (!optClient) {
        std::time_t currentTime = std::time(nullptr);
        std::string filePath = std::string(STORAGE_PATH) + username + "_" + std::to_string(currentTime);

        std::ofstream outputFile(filePath, std::ios::binary);
        if (outputFile.is_open()) {
            outputFile.write(reinterpret_cast<const char*>(&message), sizeof(StuctToServ));
            outputFile.close();
        }
        return;
    }


    std::ifstream keyFile_toClient(std::string(STORAGE_PATH) + fromClient + ".key");
    std::stringstream keyStream;

    if (keyFile_toClient) {
        for (std::string line; std::getline(keyFile_toClient, line);) {
            keyStream << line << "\n";
        }
        keyFile_toClient.close();
    } else {
        std::cerr << "Erreur : open file key user'" << std::endl;
    }

    size_t fromClient_len = fromClient.length();
    std::memset(response_client_receiver.data.content.joinMpStatus.toClient, 0, sizeof(response_client_receiver.data.content.joinMpStatus.toClient));
    std::memcpy(response_client_receiver.data.content.joinMpStatus.toClient, fromClient.c_str(), fromClient_len);
    response_client_receiver.data.content.joinMpStatus.toClient[fromClient_len] = '\0';

    std::string key = keyStream.str();
    size_t key_len = key.length();
    std::memset(response_client_receiver.data.content.joinMpStatus.publicKey, 0, sizeof(response_client_receiver.data.content.joinMpStatus.publicKey));
    std::memcpy(response_client_receiver.data.content.joinMpStatus.publicKey, key.c_str(), key_len);
    response_client_receiver.data.content.joinMpStatus.publicKey[key_len] = '\0';

    ClientData user_receiver = *optClient;
    sendToClient(&user_receiver, response_client_receiver);

    StructToClient response_client_sender;
    response_client_sender.opcode = SJMP_CODE;
    response_client_sender.data.content.joinMpStatus.status = message.data.content.joinMpStatus.status;

    std::ifstream keyFile_fromClient(std::string(STORAGE_PATH) + toClient + ".key");
    std::stringstream keyStreamf;

    if (keyFile_fromClient) {
        for (std::string line; std::getline(keyFile_fromClient, line);) {
            keyStreamf << line << "\n";
        }
        keyFile_fromClient.close();
    } else {
        std::cerr << "Erreur : open file key user'" << std::endl;
    }

    size_t toClient_len = toClient.length();
    std::memset(response_client_sender.data.content.joinMpStatus.toClient, 0, sizeof(response_client_sender.data.content.joinMpStatus.toClient));
    std::memcpy(response_client_sender.data.content.joinMpStatus.toClient, toClient.c_str(), toClient_len);
    response_client_sender.data.content.joinMpStatus.toClient[toClient_len] = '\0';

    key = keyStreamf.str();
    key_len = key.length();
    std::memset(response_client_sender.data.content.joinMpStatus.publicKey, 0, sizeof(response_client_sender.data.content.joinMpStatus.publicKey));
    std::memcpy(response_client_sender.data.content.joinMpStatus.publicKey, key.c_str(), key_len);
    response_client_sender.data.content.joinMpStatus.publicKey[key_len] = '\0';

    sendToClient(client, response_client_sender);
}
