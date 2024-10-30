#include "command.hpp"

/**
 * @brief Handles the client disconnection command (DECO).
 * 
 * This function processes the DECO command sent by a client and disconnects the client from the server.
 * It ensures that the client is properly disconnected and resets their session data.
 * 
 * @param client  A pointer to the ClientData structure representing the client's data sending the command.
 * 
 * @return void
 * 
 * @details
 * The following errors can occur and will send an error code to the client:
 * 
 *  - **550**: Invalid number of arguments. Exactly 1 argument is required:
 *    - The command name ("DECO").
 * 
 *  - **561**: Client is not connected.
 *    - The client is not connected to the server or their connection has already been terminated.
 * 
 *  - **562**: An unexpected error occurred during the disconnection process (server-side error).
 * 
 *  If disconnection is successful:
 * 
 *  - **250**: The client has been successfully disconnected and their session reset.
 */

void Command::handleDecoCommand(ClientData* client, const StuctToServ& message) {

    const std::string username("todo");

    StructToClient response;
    response.id = message.id;
    response.opcode = NONE_CODE;

    if (username.empty()) {
        response.setStatus(CodeResponseStatus::Error, 550);
        sendToClient(client, response);
        return;
    }

    if (!client->authenticated) {
        response.setStatus(CodeResponseStatus::Error, 561);
        sendToClient(client, response);
        return;
    }

    bool disconnected = true; // Todo
    if (!disconnected) {
        response.setStatus(CodeResponseStatus::Error, 562);
        sendToClient(client, response);
        return;
    }

    client->reset();
    response.setStatus(CodeResponseStatus::Ok, 250);
    sendToClient(client, response);
}