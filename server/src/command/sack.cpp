#include "command.hpp"

//todo revoir
void Command::handleSackCommand(ClientData* client, const StuctToServ& message) {
    const std::string username(reinterpret_cast<const char*>(message.data.content.ack.toClient));

    StructToClient response_client_receiver;
    response_client_receiver.opcode = RACK_CODE;

    auto optClient = isConnected(username);
    if (!optClient) {
        std::time_t currentTime = std::time(nullptr);
        std::string filePath = std::string(QUEU_PATH) + username + "_" + std::to_string(currentTime);

        std::ofstream outputFile(filePath, std::ios::binary);
        if (outputFile.is_open()) {
            outputFile.write(reinterpret_cast<const char*>(&message), sizeof(StuctToServ));
            outputFile.close();
        }
        return;
    }

    ClientData user_receiver = *optClient;
    memcpy(&response_client_receiver.data.content.ack, &message.data.content.ack, sizeof(ack_t));
    sendToClient(&user_receiver, response_client_receiver);
}
