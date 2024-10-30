#include "command.hpp"
#include "user_manager.hpp"

static std::string hashWithSHA256(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

void Command::handleChrpCommand(ClientData* client, const StuctToServ& message) {

    const std::string username(reinterpret_cast<const char*>(message.data.content.challengeResponse.clientName));
    const std::string challengeHashClient(reinterpret_cast<const char*>(message.data.content.challengeResponse.challengeHash));

    StructToClient response;
    response.id = message.id;
    response.opcode = NONE_CODE;

    if (username.empty()) {
        response.setStatus(CodeResponseStatus::Error, 550);
        sendToClient(client, response);
        return;
    }

    UserManager userManager(username);

    if (!userManager.userExist()) {
        response.setStatus(CodeResponseStatus::Error, 551);
        sendToClient(client, response);
        return;
    }

    std::string salt = userManager.getSaltUser(username);
    std::string passwordHashed = userManager.getPasswordHashedUser(username);

    std::string hashedPassword = hashWithSHA256(passwordHashed + salt);
    std::string combined = hashedPassword + client->challengeTmp;
    std::string challengeHash = hashWithSHA256(combined);

    if (challengeHashClient != challengeHash) {
        response.setStatus(CodeResponseStatus::Ok, 552);
        sendToClient(client, response);
        return;
    }

    client->username = username;
    client->authenticated = true;

    response.setStatus(CodeResponseStatus::Ok, 250);
    sendToClient(client, response);
}
