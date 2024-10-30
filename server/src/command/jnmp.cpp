#include "command.hpp"
#include <ctime>
#include <fstream>

/**
 * @brief Handles the JNMP command (Join Message private).
 * 
 * This function processes the JNMP command sent by a client, extracts the username
 * from the received message, and checks if the user exists. If the user is not connected,
 * the message is saved to a file with a timestamp. Otherwise, it sends a response
 * to the client with the status of the operation.
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
 * - If the user does not exist, an error response is sent back.
 * - If the user is not connected, the message is saved to a file named <username>_<timestamp> in the defined storage path.
 * - If the user is connected, a success response is sent back along with the username of the receiver.
 */
void Command::handleJnmpCommand(ClientData* client, const StuctToServ& message) {
    const std::string username_client_reciver(reinterpret_cast<const char*>(message.data.content.joinMp.toClient));

    //reponse commande to client sender (from client)
    StructToClient response_client_sender;
    response_client_sender.id = message.id;
    response_client_sender.opcode = NONE_CODE;

    bool userExist = isUserExists(username_client_reciver);
    if (!userExist) {
        response_client_sender.setStatus(CodeResponseStatus::Error, 500);
        sendToClient(client, response_client_sender);
        return;
    }
    auto optClient = isConnected(username_client_reciver);
    if (!optClient) {
        std::time_t currentTime = std::time(nullptr);
        std::string filePath = std::string(STORAGE_PATH) + username_client_reciver + "_" + std::to_string(currentTime);

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


    //reponse commande to client reciver (to client)
    StructToClient response_client_receiver;
    response_client_receiver.opcode = NJMP_CODE;

    const std::string username_client_sender(reinterpret_cast<const char*>(message.data.fromClient));
    std::memset(response_client_receiver.data.fromClient, 0, sizeof(response_client_receiver.data.fromClient));
    std::memcpy(response_client_receiver.data.fromClient, username_client_sender.c_str(), username_client_sender.length());
    response_client_receiver.data.fromClient[username_client_sender.length() ] = '\0';

    std::memset(response_client_receiver.data.content.joinMp.toClient, 0, sizeof(response_client_receiver.data.content.joinMp.toClient));
    std::memcpy(response_client_receiver.data.content.joinMp.toClient, username_client_reciver.c_str(), username_client_reciver.length());
    response_client_receiver.data.content.joinMp.toClient[username_client_reciver.length()] = '\0';

    std::cout << "\n\n"<< std::endl;
    std::cout << "Command handleJnmpCommand"<< std::endl;
    std::cout << "le client >" << username_client_sender << "< veut rejoindre le client >" << username_client_reciver << "<" << std::endl;
    std::cout << "toClient >" << response_client_receiver.data.content.joinMp.toClient << "< fromClient" << response_client_receiver.data.fromClient << std::endl;
    std::cout << "\n\n"<< std::endl;

    ClientData user_receiver = *optClient;
    sendToClient(&user_receiver, response_client_receiver);
}