#include "command.hpp"

/**
 * @brief Handles the SDPK command (Send Public Key).
 * 
 * This function processes the SDPK command sent by a client, extracts the username
 * and the public key from the received message, and stores the public key in a text file
 * named after the username.
 * 
 * @param client  A pointer to the ClientData structure representing the client's data 
 *                sending the command.
 * @param message A reference to a StuctToServ structure containing the command data,
 *                including the username and the public key.
 * 
 * @return void
 * 
 * @details
 * 
 * If the key is successfully stored:
 * 
 * - The public key will be saved in a file named <username>.key in the defined storage path.
 */
void Command::handleSdpkCommand(ClientData* client, const StuctToServ& message) {
    const std::string username(reinterpret_cast<const char*>(message.data.content.sendPublicKey.clientName));
    const std::string key(reinterpret_cast<const char*>(message.data.content.sendPublicKey.publicKey));

    std::string filename = std::string(STORAGE_PATH) + username + ".key";

    std::ofstream outFile(filename);
    if (outFile) {
        outFile << key;
        outFile.close();

        StructToClient response_client_receiver;
        response_client_receiver.opcode = RKEY_CODE;
        client->communicator_.username_ = username;
        unsigned char *newKey = client->communicator_.createNewKey();

        RSAEncryption clientEncryptKey;
        clientEncryptKey.encrypt(response_client_receiver.data.content.rotateKey.key, newKey, PUBLIC_KEY_FILE_USER(username));

        sendToClient(client, response_client_receiver);
        client->communicator_.rotateKey(newKey);
    } else {
        std::cerr << "Error opening file: " << filename << std::endl;
    }
}
