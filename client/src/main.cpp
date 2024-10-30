#include "client.hpp"
#include "main_window.h"
#include "user_controller.h"
#include <QApplication>
#include <thread>

QButtonGroup *globalButtonGroup = new QButtonGroup();

#include <sys/stat.h>
#include <string>
#include <vector>
#include <filesystem>

void createDirectories() {
    std::filesystem::path clientBaseDir = "/Users/Shared/ClientCryptoMadnessVortex";
    std::filesystem::path clientDirUser = clientBaseDir / "dir_user";

    try {
        std::filesystem::create_directories(clientDirUser);
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Erreur : " << e.what() << std::endl;
        return;
    }

    const std::vector<std::string> clientSubDirs = {
        "config",
        "contact",
        "conv",
        "conv/mp",
        "conv/room",
        "key",
        "queue"
    };

    for (const auto& subDir : clientSubDirs) {
        std::filesystem::path dirPath = clientDirUser / subDir;
        try {
            std::filesystem::create_directory(dirPath);
        } catch (const std::filesystem::filesystem_error& e) {
            return; 
        }
    }
}

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    createDirectories();

    UserController userController;

    MainWindow mainWindow(&userController);
    mainWindow.show();

    std::thread t([&userController, &mainWindow]() {
        Client client(&userController, &mainWindow);
        client.run();
    });

    app.exec();
    t.join();
    return 0;
}
