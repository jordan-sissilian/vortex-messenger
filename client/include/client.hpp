#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "secure_server_communicator.hpp"
#include "rsa_encryption.hpp"
#include "crypted_msg.hpp"
#include "queu_manager.hpp"
#include "message_manager.hpp"

#include "user_controller.h"
#include "main_window.h"

#include <string>
#include <unordered_map>
#include <thread>
#include <functional>
#include <variant>
#include <sys/stat.h>
#include <queue>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <filesystem>
#include <sys/select.h>
#include <iostream>
#include <cstring>
#include <cerrno>

class UserController;
class MainWindow;

using CommandCallback = std::variant<
    std::function<void(int)>,
    std::function<void(int, const std::string&)>,
    std::function<void(int, const std::string&, const std::string&)>,
    std::monostate
>;

#ifdef __linux__
#define USER_CONFIG_DIR_PATH "/usr/share/clientcryptomadnessvortex/dir_user/config/"
#define CONTACT_DIR_PATH "/usr/share/clientcryptomadnessvortex/dir_user/contact/"
#define CONTACT_MP_DIR_PATH "/usr/share/clientcryptomadnessvortex/dir_user/conv/mp/"
#define KEY_CLIENT_FILE "/usr/share/clientcryptomadnessvortex/dir_user/config/key"
#elif __APPLE__
#define USER_CONFIG_DIR_PATH "/Users/Shared/ClientCryptoMadnessVortex/dir_user/config/"
#define CONTACT_DIR_PATH "/Users/Shared/ClientCryptoMadnessVortex/dir_user/contact/"
#define CONTACT_MP_DIR_PATH "/Users/Shared/ClientCryptoMadnessVortex/dir_user/conv/mp/"
#define KEY_CLIENT_FILE "/Users/Shared/ClientCryptoMadnessVortex/dir_user/config/key"
#endif


class Client {
public:
    struct EphemeralMessage {
        int id;
        std::chrono::time_point<std::chrono::steady_clock> timestamp;
        int duration;

        bool operator<(const EphemeralMessage& other) const {
            return (timestamp + std::chrono::seconds(duration)) > 
                   (other.timestamp + std::chrono::seconds(other.duration));
        }
    };

    Client(UserController *UserController, MainWindow *mainWindow);
    ~Client();
    void run();
    bool connectToServer(const std::string& server_ip, int port, const std::string& username);
    void handleCommand();
    void receiveMessages();
    void stop();
    void addEphemeralMessage(int id, int duration);

private:
    std::priority_queue<EphemeralMessage> msgQueue;
    std::mutex queueMutexEphemeralMsg;
    std::condition_variable cv;
    std::thread workerThread;
    bool stopThread;

    void processQueue();
    void processMessage(const StructToClient& message); // Traiter le message reçu
    void handleConnection(); // Gérer la connexion au serveur
    void handleKeyExchange(const StructToClient& message); // Gérer l'échange de clé
    void handleUserMessage(const StructToClient& message); // Gérer le message utilisateur
    void handleAck(const StructToClient& message); // Gérer l'accusé de réception
    void handleUserJoin(const StructToClient& message); // Gérer l'arrivée d'un nouvel utilisateur
    void handleJoinStatus(const StructToClient& message); // Gérer le statut de jointure


private:
    std::string username;
    int server_socket;
    bool isRunning_;
    std::mutex runningMutex;
    std::condition_variable runningCV;

    UserController *userController;
    MainWindow *mainWindow;

    std::unordered_map<uint32_t, CommandCallback> commandQueue;
    std::mutex queueMutex;
    std::condition_variable queueCv;

    MessageHandler *msgHandler_;

public:
    void addCommandToQueue(uint32_t id, CommandCallback callback);
    void processCommandQueue(const StructToClient& msg);
};

#endif
