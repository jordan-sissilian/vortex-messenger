#include "command.hpp"
#include "user_manager.hpp"

/**
 * @brief Handles the account creation command (CREA).
 * 
 * This function processes the CREA command sent by a client and validates the provided information
 * before confirming the creation of the user account.
 * 
 * @param client  A pointer to the ClientData structure representing the client's data sending the command.
 * 
 * @return void
 * 
 * @details
 * The following errors can occur and will send an error code to the client:
 * 
 *  - **550**: The number of arguments is insufficient. At least 3 parameters are required:
 *    - The command name ("CREA").
 *    - A username.
 *    - A password.
 * 
 *  - **551**: The username is invalid.
 *    - It must be between 3 and 16 characters long.
 *    - It can only contain letters, numbers, and allowed characters (e.g., `_`, `-`).
 * 
 *  - **552**: The username is already taken. The user must choose a different one.
 * 
 *  - **560**: The password does not meet the security requirements.
 *    - The password must be at least 6 characters long.
 *    - It must contain at least one uppercase letter, one special character (e.g., `!`, `@`, `#`, etc.), and one number.
 * 
 *  - **561**: An unexpected error occurred during account creation (server-side error).
 * 
 *  If all validations pass successfully:
 * 
 *  - **250**: The account has been successfully created, and the client is informed of the successful operation.
 */
void Command::handleCreaCommand(ClientData* client, const StuctToServ& message) {

    const std::string username(reinterpret_cast<const char*>(message.data.content.crea.username));
    const std::string password(reinterpret_cast<const char*>(message.data.content.crea.password));

    StructToClient response;
    response.id = message.id;
    response.opcode = NONE_CODE;

    if (username.empty() || password.empty()) {
        response.setStatus(CodeResponseStatus::Error, 550);
        sendToClient(client, response);
        return;
    }

    if (username.length() < 3 || username.length() > 16) {
        response.setStatus(CodeResponseStatus::Error, 551);
        sendToClient(client, response);
        return;
    }

    if (!std::all_of(username.begin(), username.end(), [](char c) {
        return std::isalnum(c) || c == '_' || c == '-';
    })) {
        response.setStatus(CodeResponseStatus::Error, 551);
        sendToClient(client, response);
        return;
    }

    UserManager userManager(username);

    if (userManager.userExist()) {
        response.setStatus(CodeResponseStatus::Error, 552);
        sendToClient(client, response);
        return;
    }

    if (password.length() < 6) {
        response.setStatus(CodeResponseStatus::Error, 560);
        sendToClient(client, response);
        return;
    }

    bool hasUppercase = std::any_of(password.begin(), password.end(), ::isupper);
    bool hasSpecialChar = std::any_of(password.begin(), password.end(), [](char c) {
        return std::ispunct(c);
    });
    bool hasDigit = std::any_of(password.begin(), password.end(), ::isdigit);

    if (!hasUppercase || !hasSpecialChar || !hasDigit) {
        response.setStatus(CodeResponseStatus::Error, 560);
        sendToClient(client, response);
        return;
    }

    if (!userManager.createUser(password)) {
        response.setStatus(CodeResponseStatus::Error, 561);
        sendToClient(client, response);
        return;
    }

    response.setStatus(CodeResponseStatus::Ok, 250);
    sendToClient(client, response);
}
