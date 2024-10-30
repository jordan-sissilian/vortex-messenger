#include "command.hpp"
#include "user_manager.hpp"

void Command::handleAuthCommand(ClientData* client, const StuctToServ& message) {

    const std::string username(reinterpret_cast<const char*>(message.data.content.auth.username));

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

    auto generateRandomChallenge = [](size_t length) {
        const std::string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
        std::string challenge;

        std::srand(static_cast<unsigned int>(std::time(0)));
        for (size_t i = 0; i < length; ++i) {
            int index = std::rand() % characters.size();
            challenge += characters[index];
        }
        return challenge;
    };

    std::string randomChallenge = generateRandomChallenge(32);
    client->challengeTmp = randomChallenge;

    std::memcpy(response.data.content.challengeResponse.salt, salt.c_str(), salt.size());
    std::memcpy(response.data.content.challengeResponse.challenge, randomChallenge.c_str(), randomChallenge.size());

    client->username = username;
    client->authenticated = false;

    response.setStatus(CodeResponseStatus::Ok, 250);
    sendToClient(client, response);
}
